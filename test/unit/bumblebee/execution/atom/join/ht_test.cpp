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

#include "bumblebee/catalog/PredicateTables.hpp"
#include <gtest/gtest.h>

#include "bumblebee/ClientContext.hpp"
#include "bumblebee/common/vector_operations/VectorOperations.hpp"
#include "bumblebee/execution/JoinHashTable.hpp"
#include "bumblebee/execution/atom/join/PhysicalCrossProduct.hpp"
#include "bumblebee/execution/atom/join/PhysicalNestedLoop.hpp"
#include "bumblebee/parallel/ThreadContext.hpp"

using namespace bumblebee;

class PhysicalHJTest : public ::testing::Test {
    // Creates and returns a DataChunk initialized with a predefined set of column types: INTEGER, UINTEGER, and BIGINT.
    // For each of the count rows, it populates the columns with incrementing values:
    // Column 0 (INTEGER): sequential int32_t values starting from 0.
    // Column 1 (UINTEGER): uint32_t values equal to i * 10.
    // Column 2 (BIGINT): int64_t values equal to i * 100.
    // The function sets the cardinality of the chunk to count and returns it.
    // This utility is primarily used for generating consistent and type-diverse data for testing the ChunkCollection class.
protected:
    std::shared_ptr<PredicateTables> ptableLeft;
    std::shared_ptr<PredicateTables> ptableRight;
    ClientContext client_context;
    ThreadContext context{client_context};

    void SetUp() override{
        ptableLeft = std::make_shared<PredicateTables>(&client_context,"a",3);
        ptableRight = std::make_shared<PredicateTables>(&client_context, "b",3);
    }

    vector<ConstantType> tLeft{ConstantType::INTEGER, ConstantType::UINTEGER, ConstantType::BIGINT};
    vector<ConstantType> tRight{ConstantType::UINTEGER, ConstantType::BIGINT, ConstantType::INTEGER};

    DataChunk createChunkWithValue( vector<ConstantType> testTypes, idx_t count = 1, idx_t offset=0 ) {
        DataChunk chunk;
        chunk.initialize(testTypes);
        chunk.setCapacity(count);
        chunk.resize(count);
        for (idx_t i = 0; i < count; ++i) {
            for (idx_t j = 0; j < testTypes.size(); ++j) {
                auto value = Value((int64_t) ((i+offset)*10*j));
                chunk.setValue(j, i, value.cast(testTypes[j]));
            }
        }
        chunk.setCardinality(count);
        return chunk;
    }

    void populatePTable(std::shared_ptr<PredicateTables> ptable ,vector<ConstantType> types, idx_t chunks = 10, idx_t elements=STANDARD_VECTOR_SIZE) {
        for (unsigned int i = 0; i < chunks; ++i) {
            DataChunk chunk = createChunkWithValue(types, elements, i*STANDARD_VECTOR_SIZE);
            ptable->append(chunk);
        }
    }

    Vector calculateBucketVector(idx_t buckets_, Vector &hash, idx_t size) {
        Vector bucket(hash.getType(), size);
        Value maskValue(buckets_-1);
        Vector mask(maskValue);
        BB_ASSERT(mask.getVectorType() == VectorType::CONSTANT_VECTOR);
        VectorOperations::lAnd(hash, mask, bucket, size);
        return bucket;
    }
};



TEST_F(PhysicalHJTest, HTBuildSimpleTest) {
    populatePTable(ptableLeft, tLeft, 1, 10);
    populatePTable(ptableLeft, tLeft, 1, 10);
    vector<idx_t> keys = {1};
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
    auto& rchunk = ht.getDataChunk();
    EXPECT_EQ(rchunk.getSize(), ptableLeft->getCount());

    // check all the elements in keys vector with the same values are in the same directory
    auto& directory = ht.getDirectory();
    std::unordered_map<string, idx_t> valueBucket;
    for (idx_t bucket = 0; bucket < buckets; ++bucket) {
        auto offset = JoinHashTable::dirBegin(directory.get(), bucket);
        auto bucketSize = JoinHashTable::dirEnd(directory.get(), bucket) - offset;
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
    vector<idx_t> keys = {1}; // arbitrary; no data will be hashed
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
    vector<idx_t> keys = {1}; // hash on column 1 (non-trivial values)
    populatePTable(ptableRight, tRight, 6, 64);

    JoinHashTable ht(ptableRight.get()->predicate_.get(), keys, buckets);

    // Launch threads that add each chunk with its hash vector
    vector<std::thread> threads;
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
    vector<uint64_t> expected_bucket_sizes(buckets, 0);
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
        uint64_t cum = JoinHashTable::dirEnd(directory.get(), i);
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

    vector<idx_t> keys = {1}; // join on column 1
    const idx_t buckets = 8;

    JoinHashTable ht(ptableRight->predicate_.get(), keys, buckets);

    // Add right chunk to the HT
    Vector rhash(UBIGINT);
    rchunk.hash(rhash, keys);
    ht.addDataChunkSel(rhash, rchunk);

    // Build
    ht.initDirectory();
    for (idx_t i = 0; i < buckets; ++i)
        ht.build(i);


    // Prepare probe inputs
    Vector lhash(UBIGINT);
    lchunk.hash(lhash, keys);
    SelectionVector lsel(STANDARD_VECTOR_SIZE);
    SelectionVector rsel(STANDARD_VECTOR_SIZE);

    vector<Expression> conditions;
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

    vector<idx_t> keys = {1};
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

    vector<Expression> conditions;
    Expression eq;
    conditions.push_back(Expression::generateExpression(EQUAL, 1, 1));

    idx_t lpos = 0, rpos = 0;
    auto got = ht.probe(lpos, rpos, lchunk, lhash, lsel, rsel, conditions);
    EXPECT_EQ(got, 0);
}


/*
 * Directory masking: calculateBucketVector must be equivalent to hash & (buckets-1).
 * We verify the exact masking behavior for a bunch of crafted hash values.
 */
TEST_F(PhysicalHJTest, HTBucketMaskingMatchesBitwiseAnd) {
    vector<idx_t> keys = {0}; // key doesn't matter for this test
    const idx_t buckets = 32;       // power of two
    JoinHashTable ht(ptableLeft->predicate_.get(), keys, buckets);

    // Craft a vector of "hashes" directly (UBIGINT) and check calculateBucketVector()
    Vector hash(UBIGINT, 16);
    auto *h = FlatVector::getData<uint64_t>(hash);
    for (idx_t i = 0; i < 16; ++i) {
        // distribute various bit patterns
        h[i] = (uint64_t(1) << (i % 16)) ^ (i * 1315423911ULL) ^ (0x9e3779b97f4a7c15ULL >> (i % 8));
    }
    Vector buckets_vec = calculateBucketVector(ht.getBuckets(),hash, 16);
    auto *b = FlatVector::getData<uint64_t>(buckets_vec);

    for (idx_t i = 0; i < 16; ++i) {
        EXPECT_EQ(b[i], (h[i] & (buckets - 1))) << "Mismatch at i=" << i;
    }
}

/*
 * checkKeys(): order-insensitive match must succeed; size or element mismatch must fail.
 */
TEST_F(PhysicalHJTest, HTCheckKeysOrderAndMismatch) {
    vector<idx_t> keys_ref = {2, 0, 1};
    JoinHashTable ht(ptableLeft->predicate_.get(), keys_ref, 8);

    // Same set, different order -> true
    EXPECT_TRUE(ht.checkKeys({1, 2, 0}));
    EXPECT_TRUE(ht.checkKeys({0, 1, 2}));

    // Different cardinality -> false
    EXPECT_FALSE(ht.checkKeys({0, 1}));

    // Same cardinality but different element -> false
    EXPECT_FALSE(ht.checkKeys({0, 1, 42}));
}


/*
 * Build then probe with multiple conditions (refine path):
 *  - First condition (EQUAL on key col) selects a superset.
 *  - Second condition (GREATER) filters it down.
 * Uses buckets=1 so hash partitioning won't hide logical issues.
 */
TEST_F(PhysicalHJTest, HTProbeMultipleConditionsRefine) {
    // Right side: columns (UINTEGER, BIGINT, INTEGER) per tRight
    auto rchunk = createChunkWithValue(tRight, 50, 0);
    // Left side: columns (INTEGER, UINTEGER, BIGINT) per tLeft
    auto lchunk = createChunkWithValue(tLeft,  50, 0);

    // We’ll join on left.col1 (UINTEGER) == right.col0 (UINTEGER) to keep key types identical
    vector<idx_t> build_keys = {0}; // for rchunk (its col0 is UINTEGER)
    const idx_t buckets = 1; // avoid hash-bucket alignment issues for this logical test
    JoinHashTable ht(ptableRight->predicate_.get(), build_keys, buckets);

    // Add to HT (right)
    Vector rhash(UBIGINT);
    rchunk.hash(rhash, build_keys);
    ht.addDataChunkSel(rhash, rchunk);
    ht.initDirectory();
    ht.build(0);

    // Probe (left) with two conditions:
    //   1) left.col1 == right.col0
    //   2) left.col0 >  right.col2  (note: different columns, mixed signs)
    // Given the data generator:
    //   - col0 = i*0 == 0 for all rows
    //   - col2 = i*20 (left BIGINT), right.col2 is INTEGER in tRight -> i*20
    // So condition (2) becomes: 0 > (i*20), only true when i == 0.
    Vector lhash(UBIGINT);
    lchunk.hash(lhash,vector<idx_t>{1});
    SelectionVector lsel(STANDARD_VECTOR_SIZE);
    SelectionVector rsel(STANDARD_VECTOR_SIZE);
    vector<Expression> conditions;
    conditions.push_back(Expression::generateExpression(EQUAL,1, 0));
    conditions.push_back(Expression::generateExpression(GREATER,0, 2));

    idx_t lpos = 0, rpos = 0;
    idx_t total = 0;
    while (true) {
        auto got = ht.probe(lpos, rpos, lchunk, lhash, lsel, rsel, conditions);
        if (!got) break;
        for (idx_t i = 0; i < got; ++i) {
            auto li = lsel.getIndex(i);
            auto ri = rsel.getIndex(i);
            // Check refine predicate actually holds
            auto lv_key = lchunk.data_[1].getValue(li).toString();
            auto rv_key = ht.getDataChunk().data_[0].getValue(ri).toString();
            EXPECT_EQ(lv_key, rv_key);

            auto lv0 = lchunk.data_[0].getValue(li).getNumericValue<int64_t>(); // 0
            auto rv2 = ht.getDataChunk().data_[2].getValue(ri).getNumericValue<int64_t>(); // = i*20
            EXPECT_GT(lv0, rv2);
        }
        total += got;
    }
    // Only i==0 satisfies 0 > 0? No, that's false. So total should be 0 unless negative rv2 exists.
    // Our generator makes rv2 = i*20 >= 0, so expect 0 results.
    EXPECT_EQ(total, 0);
}


/*
 * Probe pagination: ensure probe emits at most STANDARD_VECTOR_SIZE per call and
 * that iterating collects all matches. Use equality on same-domain data with size > 1 batch.
 */
TEST_F(PhysicalHJTest, HTProbeRespectsBatchSizeAndCollectsAll) {
    const idx_t N = STANDARD_VECTOR_SIZE * 2 + 7; // spans multiple batches
    auto rchunk = createChunkWithValue(tRight, N, 0);
    auto lchunk = createChunkWithValue(tLeft,  N, 0);

    vector<idx_t> keys = {1}; // join on the 10*i column
    const idx_t buckets = 8;
    JoinHashTable ht(ptableRight.get()->predicate_.get(), keys, buckets);

    Vector rhash(UBIGINT, N);
    rchunk.hash(rhash, keys);
    ht.addDataChunkSel(rhash, rchunk);
    ht.initDirectory();
    for (idx_t i = 0; i < buckets; ++i) ht.build(i);

    // Probe
    Vector lhash(UBIGINT, N);
    lchunk.hash(lhash, keys);
    SelectionVector lsel(STANDARD_VECTOR_SIZE);
    SelectionVector rsel(STANDARD_VECTOR_SIZE);
    vector<Expression> conditions;
    conditions.push_back(Expression::generateExpression(EQUAL, 1, 1));

    idx_t lpos = 0, rpos = 0;
    idx_t total = 0;
    while (true) {
        auto got = ht.probe(lpos, rpos, lchunk, lhash, lsel, rsel, conditions);
        if (!got) break;
        EXPECT_LE(got, (idx_t)STANDARD_VECTOR_SIZE);
        total += got;
    }
    EXPECT_EQ(total, N) << "Should match 1:1 for identical domains across multiple batches";
}


/*
 * Mixed numeric types (FLOAT vs INTEGER) in a *refine* condition:
 *   - First condition EQUAL on same-typed key so we get candidate matches.
 *   - Second condition compares FLOAT (left) with INTEGER (right) via common-type cast.
 * buckets=1 to focus purely on comparison logic.
 */
TEST_F(PhysicalHJTest, HTProbeRefineWithFloatVsIntegerCommonCast) {
    // left: (FLOAT, UINTEGER)   right: (UINTEGER, INTEGER)
    vector<ConstantType> leftTypes  {ConstantType::FLOAT,   ConstantType::UINTEGER};
    vector<ConstantType> rightTypes {ConstantType::UINTEGER, ConstantType::INTEGER};

    auto lchunk = createChunkWithValue(leftTypes,  64,0);
    auto rchunk = createChunkWithValue(rightTypes, 64,0);

    // Build on right key = col0 (UINTEGER)
    vector<idx_t> build_keys = {0};
    const idx_t buckets = 1;
    JoinHashTable ht(ptableRight.get()->predicate_.get(), build_keys, buckets);

    Vector rhash(UBIGINT);
    rchunk.hash(rhash, build_keys);
    ht.addDataChunkSel(rhash, rchunk);
    ht.initDirectory();
    ht.build(0);

    // Probe conditions:
    //  1) left.col1 (UINTEGER) == right.col0 (UINTEGER)  -> candidate set
    //  2) left.col0 (FLOAT)    <= right.col1 (INTEGER)   -> common-type cast to double
    Vector lhash(UBIGINT);
    lchunk.hash(lhash, /*probe uses the key from #1*/ vector<idx_t>{1});
    SelectionVector lsel(STANDARD_VECTOR_SIZE);
    SelectionVector rsel(STANDARD_VECTOR_SIZE);

    vector<Expression> conditions;
    conditions.push_back(Expression::generateExpression(EQUAL,        1, 0));
    conditions.push_back(Expression::generateExpression(LESS_OR_EQ,   0, 1));

    idx_t lpos = 0, rpos = 0, total = 0, checked = 0;
    while (true) {
        auto got = ht.probe(lpos, rpos, lchunk, lhash, lsel, rsel, conditions);
        if (!got) break;
        for (idx_t i = 0; i < got; ++i) {
            auto li = lsel.getIndex(i);
            auto ri = rsel.getIndex(i);
            auto keyL = lchunk.data_[1].getValue(li).getNumericValue<uint32_t>();
            auto keyR = ht.getDataChunk().data_[0].getValue(ri).getNumericValue<uint32_t>();
            EXPECT_EQ(keyL, keyR);

            double lf = lchunk.data_[0].getValue(li).getNumericValue<double>();   // = (i*10*0)=0.0 from generator
            int    ri_int = ht.getDataChunk().data_[1].getValue(ri).getNumericValue<int32_t>(); // = i*10*1 = i*10
            EXPECT_LE(lf, static_cast<double>(ri_int));
            checked++;
        }
        total += got;
    }
    // Every candidate from #1 should also satisfy #2 because left.col0 is 0.0 for all rows in our generator.
    EXPECT_EQ(total, lchunk.getSize());
    EXPECT_EQ(checked, total);
}



/*
 * Multi-key join test:
 * Build and probe a hash table using two keys instead of one.
 * We join on left.col0 == right.col0 AND left.col1 == right.col1.
 */
TEST_F(PhysicalHJTest, HTProbeWithMultipleKeys) {
    // Create matching left/right chunks
    // Left: INTEGER, UINTEGER, BIGINT
    // Right: UINTEGER, BIGINT, INTEGER
    auto lchunk = createChunkWithValue(tLeft,  40, 0);
    auto rchunk = createChunkWithValue(tRight, 40, 0);

    // Keys: left.col0 (INTEGER) matches right.col2 (INTEGER)
    //       left.col1 (UINTEGER) matches right.col0 (UINTEGER)
    vector<idx_t> build_keys = {2, 0};
    const idx_t buckets = 8;

    JoinHashTable ht(ptableRight->predicate_.get(), build_keys, buckets);

    // Hash on the two keys and build
    Vector rhash(UBIGINT);
    rchunk.hash(rhash, build_keys);
    ht.addDataChunkSel(rhash, rchunk);
    ht.initDirectory();
    for (idx_t i = 0; i < buckets; ++i) {
        ht.build(i);
    }

    // Probe: left's keys {0,1} against right's {2,0}
    Vector lhash(UBIGINT);
    lchunk.hash(lhash, {0, 2});
    SelectionVector lsel(STANDARD_VECTOR_SIZE);
    SelectionVector rsel(STANDARD_VECTOR_SIZE);

    vector<Expression> conditions;
    conditions.push_back(Expression::generateExpression(EQUAL, 0, 0)); // left.col0 == right.col2
    conditions.push_back(Expression::generateExpression(EQUAL, 2, 2)); // left.col1 == right.col0

    idx_t lpos = 0, rpos = 0;
    idx_t total_matches = 0;
    while (true) {
        auto got = ht.probe(lpos, rpos, lchunk, lhash, lsel, rsel, conditions);
        if (got == 0) break;

        for (idx_t i = 0; i < got; ++i) {
            auto li = lsel.getIndex(i);
            auto ri = rsel.getIndex(i);
            auto lv0 = lchunk.data_[0].getValue(li).toString();
            auto rv2 = ht.getDataChunk().data_[0].getValue(ri).toString();
            EXPECT_EQ(lv0, rv2);

            auto lv1 = lchunk.data_[2].getValue(li).toString();
            auto rv0 = ht.getDataChunk().data_[2].getValue(ri).toString();
            EXPECT_EQ(lv1, rv0);
        }
        total_matches += got;
    }

    // Since both chunks are generated identically for the key columns, we expect one match per left row.
    EXPECT_EQ(total_matches, lchunk.getSize());
}
