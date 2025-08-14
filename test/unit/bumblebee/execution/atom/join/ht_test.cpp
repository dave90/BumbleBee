/*
 * Copyright (C) 2025 Davide Fuscà
 *
 * This file is part of BumbleBee.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <numeric>
#include <thread>

#include "bumblebee/catalog/PredicateTables.h"
#include <gtest/gtest.h>

#include "bumblebee/ClientContext.h"
#include "bumblebee/execution/JoinHashTable.h"
#include "bumblebee/execution/atom/join/PhysicalCrossProduct.h"
#include "bumblebee/execution/atom/join/PhysicalNestedLoop.h"
#include "bumblebee/parallel/ThreadContext.h"

using namespace bumblebee;
using namespace std;

class PhysicalHJTest : public ::testing::Test {
    // Creates and returns a DataChunk initialized with a predefined set of column types: INTEGER, UINTEGER, and BIGINT.
    // For each of the count rows, it populates the columns with incrementing values:
    // Column 0 (INTEGER): sequential int32_t values starting from 0.
    // Column 1 (UINTEGER): uint32_t values equal to i * 10.
    // Column 2 (BIGINT): int64_t values equal to i * 100.
    // The function sets the cardinality of the chunk to count and returns it.
    // This utility is primarily used for generating consistent and type-diverse data for testing the ChunkCollection class.
protected:
    shared_ptr<PredicateTables> ptableLeft;
    shared_ptr<PredicateTables> ptableRight;
    ClientContext client_context;
    ThreadContext context{client_context};

    void SetUp() override{
        ptableLeft = make_shared<PredicateTables>("a",3);
        ptableRight = make_shared<PredicateTables>("b",3);
    }

    std::vector<ConstantType> tLeft{ConstantType::INTEGER, ConstantType::UINTEGER, ConstantType::BIGINT};
    std::vector<ConstantType> tRight{ConstantType::UINTEGER, ConstantType::BIGINT, ConstantType::INTEGER};

    DataChunk createChunkWithValue( std::vector<ConstantType> testTypes, idx_t count = 1, idx_t offset=0 ) {
        DataChunk chunk;
        chunk.initialize(testTypes);
        chunk.setCardinality(count);
        for (idx_t i = 0; i < count; ++i) {
            for (idx_t j = 0; j < testTypes.size(); ++j) {
                auto value = Value((int64_t) ((i+offset)*10*j));
                chunk.setValue(j, i, value.cast(testTypes[j]));
            }
        }
        chunk.setCardinality(count);
        return chunk;
    }

    void populatePTable(shared_ptr<PredicateTables> ptable ,std::vector<ConstantType> types, idx_t chunks = 10, idx_t elements=STANDARD_VECTOR_SIZE) {
        for (unsigned int i = 0; i < chunks; ++i) {
            DataChunk chunk = createChunkWithValue(types, elements, i*STANDARD_VECTOR_SIZE);
            ptable->append(chunk);
        }
    }
};



TEST_F(PhysicalHJTest, HTBuildSimpleTest) {
    populatePTable(ptableLeft, tLeft, 1, 10);
    populatePTable(ptableLeft, tLeft, 1, 10);
    std::vector<idx_t> keys = {1};
    auto buckets = 16;
    JoinHashTable ht(ptableLeft.get()->predicate_.get(), keys, buckets);

    for (idx_t i = 0; i < ptableLeft->chunkCount(); ++i) {
        auto& chunk = ptableLeft->getChunk(i);

        Vector hash(UBIGINT);
        chunk.hash(hash, keys);
        ht.addDataChunkSel(hash, chunk );
    }
    ht.initDirectory();
    for (idx_t i = 0; i < buckets; ++i) {
        ht.build(i);
    }
    cout << ht.toString() << endl;
    auto& rchunk = ht.getDataChunk();
    EXPECT_EQ(rchunk.getSize(), ptableLeft->getCount());

    // check all the elements in keys vector with the same values are in the same directory
    auto& directory = ht.getDirectory();
    unordered_map<string, idx_t> valueBucket;
    for (idx_t bucket = 0; bucket < buckets; ++bucket) {
        auto offset = (!bucket)? 0 : directory[bucket-1];
        auto bucketSize = directory[bucket] - offset;
        for (idx_t i = 0; i < bucketSize; ++i) {
            auto value = rchunk.data_[keys[0]].getValue(offset+i).toString();
            if (valueBucket.contains(value))
                EXPECT_EQ(valueBucket[value], bucket);
            else
                valueBucket[value] = bucket;
        }
    }
}

/*
 * Edge case: No chunks added. Directory should be allocated and fully zeroed.
 * We do not call build() here to avoid touching uninitialized storage.
 */
TEST_F(PhysicalHJTest, HTInitDirectoryEmptyInput) {
    std::vector<idx_t> keys = {1}; // arbitrary; no data will be hashed
    const idx_t buckets = 8;
    JoinHashTable ht(ptableLeft.get()->predicate_.get(), keys, buckets);

    // No addDataChunkSel calls
    ht.initDirectory();

    auto &directory = ht.getDirectory();
    for (idx_t i = 0; i < buckets; ++i) {
        EXPECT_EQ(directory[i], 0) << "Directory entry " << i << " should be zero for empty input";
    }

    // getDataChunk should report size 0
    EXPECT_EQ(ht.getDataChunk().getSize(), 0);
}

/*
 * Concurrent addDataChunkSel across multiple chunks and threads.
 * After initDirectory(), cumulative directory counts must match a manual bucket-counting pass.
 * We do NOT call build() here — we are only validating statistics & directory computation.
 */
TEST_F(PhysicalHJTest, HTBucketDistributionMatchesHashUnderConcurrency) {
    // Create several chunks on the "right" table and feed them concurrently.
    // Use a power-of-two bucket count with multiple buckets to stress distribution.
    const idx_t buckets = 16;
    std::vector<idx_t> keys = {1}; // hash on column 1 (non-trivial values)
    populatePTable(ptableRight, tRight, 6, 64);

    JoinHashTable ht(ptableRight.get()->predicate_.get(), keys, buckets);

    // Launch threads that add each chunk with its hash vector
    std::vector<std::thread> threads;
    for (idx_t c = 0; c < ptableRight->chunkCount(); ++c) {
        threads.emplace_back([&, c]() {
            auto &chunk = ptableRight->getChunk(c);
            Vector hash(UBIGINT);
            chunk.hash(hash, keys);
            ht.addDataChunkSel(hash, chunk);
        });
    }
    for (auto &th : threads) th.join();

    // Initialize directory from collected stats
    ht.initDirectory();

    // Manually recompute expected per-bucket counts (without building) and compare with directory deltas
    std::vector<uint64_t> expected_bucket_sizes(buckets, 0);
    for (idx_t c = 0; c < ptableRight->chunkCount(); ++c) {
        auto &chunk = ptableRight->getChunk(c);
        Vector hash(UBIGINT);
        chunk.hash(hash, keys);
        auto *h = FlatVector::getData<uint64_t>(hash);
        for (idx_t i = 0; i < chunk.getSize(); ++i) {
            auto b = h[i] & (buckets - 1);
            expected_bucket_sizes[b]++;
        }
    }

    auto &directory = ht.getDirectory();
    // directory[i] is cumulative; reconstruct sizes and compare
    uint64_t prev = 0;
    for (idx_t i = 0; i < buckets; ++i) {
        uint64_t cum = directory[i];
        uint64_t size_i = cum - prev;
        EXPECT_EQ(size_i, expected_bucket_sizes[i]) << "Bucket " << i << " size mismatch";
        prev = cum;
    }
}

/*
 * Edge case 3: Probe with EQUAL on single bucket and different comparable types (UINTEGER vs BIGINT).
 * Expect exactly one match per left row when right side contains the same values in its column 1.
 */
TEST_F(PhysicalHJTest, HTProbeEqualCommonTypeSingleBucket) {
    // Prepare right side (to build HT): one chunk with values on col1 spanning 0,10,20,...,190
    auto rchunk = createChunkWithValue(tRight, 20, 0); // right col1 is BIGINT, values = i*10
    // Prepare left side: one chunk with the same value domain on col1 (UINTEGER)
    auto lchunk = createChunkWithValue(tLeft, 20, 0);

    std::vector<idx_t> keys = {1}; // join on column 1
    const idx_t buckets = 8;

    JoinHashTable ht(ptableRight.get()->predicate_.get(), keys, buckets);

    // Add right chunk to the HT
    Vector rhash(UBIGINT);
    rchunk.hash(rhash, keys);
    ht.addDataChunkSel(rhash, rchunk);

    // Build
    ht.initDirectory();
    for (idx_t i = 0; i < buckets; ++i)
        ht.build(i);

    cout << ht.toString() << endl;

    // Prepare probe inputs
    Vector lhash(UBIGINT);
    lchunk.hash(lhash, keys);
    SelectionVector lsel(STANDARD_VECTOR_SIZE);
    SelectionVector rsel(STANDARD_VECTOR_SIZE);

    std::vector<Expression> conditions;
    conditions.push_back(Expression::generateExpression(EQUAL, 1, 1));

    idx_t lpos = 0, rpos = 0;
    idx_t total_matches = 0;
    while (true) {
        auto got = ht.probe(lpos, rpos, lchunk, lhash, lsel, rsel, conditions);
        if (got == 0) break;
        // Sanity: matched values must be equal (spot check inside this batch)
        for (idx_t i = 0; i < got; ++i) {
            auto li = lsel.getIndex(i);
            auto ri = rsel.getIndex(i);
            auto lv = lchunk.data_[1].getValue(li).toString();
            auto rv = ht.getDataChunk().data_[1].getValue(ri).toString();
            EXPECT_EQ(lv, rv);
        }
        total_matches += got;
    }

    // One match per left row (domains coincide)
    EXPECT_EQ(total_matches, lchunk.getSize());
}



/*
 * Edge case : Probe where there are zero matches (disjoint domains).
 * Expect probe to immediately return 0.
 */
TEST_F(PhysicalHJTest, HTProbeNoMatches) {
    // Right: values 0..(n-1) on col1
    auto rchunk = createChunkWithValue(tRight, /*count=*/32, /*offset=*/0);
    // Left: shifted so equality has no overlap on col1
    auto lchunk = createChunkWithValue(tLeft,  /*count=*/32, /*offset=*/1000);

    std::vector<idx_t> keys = {1};
    const idx_t buckets = 1;

    JoinHashTable ht(ptableRight.get()->predicate_.get(), keys, buckets);

    Vector rhash(UBIGINT);
    rchunk.hash(rhash, keys);
    ht.addDataChunkSel(rhash, rchunk);

    ht.initDirectory();
    ht.build(0);

    Vector lhash(UBIGINT);
    lchunk.hash(lhash, keys);
    SelectionVector lsel(STANDARD_VECTOR_SIZE);
    SelectionVector rsel(STANDARD_VECTOR_SIZE);

    std::vector<Expression> conditions;
    Expression eq;
    conditions.push_back(Expression::generateExpression(EQUAL, 1, 1));

    idx_t lpos = 0, rpos = 0;
    auto got = ht.probe(lpos, rpos, lchunk, lhash, lsel, rsel, conditions);
    EXPECT_EQ(got, 0);
}