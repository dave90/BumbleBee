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

#include "bumblebee/common/Constants.hpp"
#include <gtest/gtest.h>

#include "bumblebee/ClientContext.hpp"
#include "bumblebee/catalog/PredicateTables.hpp"
#include "bumblebee/execution/AggregateChunkOneHashTable.hpp"
#include "bumblebee/function/aggregate/Sum.hpp"
#include "bumblebee/parallel/ThreadContext.hpp"

using namespace bumblebee;

using agg_ht_ptr = AggregateChunkOneHashTable::agg_ht_ptr_t;

class AggHTTest : public ::testing::Test {
    // Creates and returns a DataChunk initialized with a predefined set of column types: INTEGER, UINTEGER, and BIGINT.
    // For each of the count rows, it populates the columns with incrementing values:
    // Column 0 (INTEGER): sequential int32_t values starting from 0.
    // Column 1 (UINTEGER): uint32_t values equal to i * 10.
    // Column 2 (BIGINT): int64_t values equal to i * 100.
    // The function sets the cardinality of the chunk to count and returns it.
    // This utility is primarily used for generating consistent and type-diverse data for testing the ChunkCollection class.
protected:

    vector<ConstantType> tLeft{ConstantType::SMALLINT, ConstantType::UINTEGER, ConstantType::BIGINT};

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
    void addChunkToAHT(agg_ht_ptr& ht, DataChunk &chunk, vector<idx_t> groups,
            vector<idx_t> payloads, vector<AggregateFunction*> functions, idx_t capacity = MORSEL_SIZE,
            bool resize = false) {
        DataChunk group, payload;
        vector<ConstantType> groupTypes, payloadTypes;
        for (auto g : groups)
            groupTypes.push_back(chunk.getTypes()[g]);
        for (auto p : payloads)
            payloadTypes.push_back(chunk.getTypes()[p]);

        if (!ht)
            ht = agg_ht_ptr(new AggregateChunkOneHashTable(groupTypes, capacity, resize,  functions));

        group.initialize(groupTypes);
        payload.initialize(payloadTypes);
        group.reference(chunk, groups);
        payload.reference(chunk, payloads);
        Vector hash(UBIGINT, group.getSize());
        group.hash(hash);
        ht->addChunk(hash, group, payload);

    }


    DataChunk probeToHT(agg_ht_ptr& ht, DataChunk &chunk, vector<idx_t> groups, vector<AggregateFunction*> functions) {
        vector<ConstantType> groupTypes, payloadTypes;
        for (auto g : groups)
            groupTypes.push_back(chunk.getTypes()[g]);
        for (auto f : functions)
            payloadTypes.push_back(f->result_);

        DataChunk group, payload;
        group.initialize(groupTypes);
        group.reference(chunk, groups);
        payload.initialize(payloadTypes);
        payload.setCardinality(group.getSize());

        Vector hash(UBIGINT, group.getSize());
        group.hash(hash);

        SelectionVector sel(group.getSize());
        ht->fetchAggregates(hash, group, payload,sel);
        return payload;
    }

};

TEST_F(AggHTTest, AggHTSimpleTest) {
    vector<idx_t> groupIndexTypes = {1,2};
    vector<idx_t> payloadIndexTypes = {0};
    DataChunk chunk = createChunkWithValue(tLeft, 10);

    auto aggFunc = SumFunc::getFunction(tLeft[payloadIndexTypes[0]]);
    vector functions = {(AggregateFunction*)aggFunc.get()};
    agg_ht_ptr ht;
    addChunkToAHT(ht, chunk, groupIndexTypes, payloadIndexTypes, functions, 32);

    ht->finalize();

    DataChunk payload = probeToHT(ht, chunk, groupIndexTypes, functions);
    // check that is all 0
    for (idx_t i = 0; i < payload.getSize(); i++)
        EXPECT_EQ(payload.data_[0].getValue(i).cast(BIGINT).getNumericValue<int64_t>(), 0 );
}

TEST_F(AggHTTest, AddChunk_DuplicateGroupsAggregatesCorrectlyNoDistinc) {
    // Group by cols [1,2]; SUM over col 2 (BIGINT). We'll add the *same* rows twice.
    vector<idx_t> groupIndexTypes = {1, 2};
    vector<idx_t> payloadIndexTypes = {2};
    auto aggFunc = SumFunc::getFunction(tLeft[payloadIndexTypes[0]]);
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    // Build the chunk with 8 rows
    DataChunk chunk = createChunkWithValue(tLeft, 8 );

    // Create HT and add the same chunk twice (duplicates)
    agg_ht_ptr ht;
    // call with no distinc cols
    addChunkToAHT(ht, chunk, groupIndexTypes, payloadIndexTypes ,functions, 32);
    addChunkToAHT(ht, chunk, groupIndexTypes, payloadIndexTypes, functions, 32);

    // Finalize to flush aggregate states into payload
    ht->finalize();

    // Probe with the original groups; the SUM should be doubled of payload column
    DataChunk result = probeToHT(ht, chunk, groupIndexTypes, functions);

    EXPECT_EQ(result.columnCount(), 1);
    EXPECT_EQ(result.getSize(), chunk.getSize());
    for (idx_t i = 0; i < result.getSize(); ++i) {
        // Expected = 2 * (i * 10)
        int64_t expected = 2 * chunk.data_[payloadIndexTypes[0]].getValue(i).cast(BIGINT).getNumericValue<int64_t>();
        int64_t got = result.data_[0].getValue(i).cast(BIGINT).getNumericValue<int64_t>();
        EXPECT_EQ(got, expected) << "Row " << i << " mismatch";
    }
}


TEST_F(AggHTTest, FetchAggregates_UnseenGroupReturnsZeroAndCreatesEntry) {
    // Build an HT with groups for i in [0..9], then probe with a *new* group (offset=100)
    vector<idx_t> groupIndexTypes = {1, 2};
    vector<idx_t> payloadIndexTypes = {2}; // SUM over BIGINT
    auto aggFunc = SumFunc::getFunction(tLeft[payloadIndexTypes[0]]);
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    // Base table with 10 groups
    DataChunk base = createChunkWithValue(tLeft, 10);
    agg_ht_ptr ht;
    addChunkToAHT(ht, base, groupIndexTypes, payloadIndexTypes, functions, /*capacity=*/32);
    ht->finalize();

    // Probe with unseen groups (same shape, but offset so keys don't exist yet)
    DataChunk unseen = createChunkWithValue(tLeft, 5, /*offset=*/100);
    DataChunk payload = probeToHT(ht, unseen, groupIndexTypes, functions);

    EXPECT_EQ(payload.getSize(), 0);
}



TEST_F(AggHTTest, FetchAggregates_UnseenAndSeeGroup) {
    // Build an HT with groups for i in [0..9], then probe with a *new* group (offset=100)
    vector<idx_t> groupIndexTypes = {1, 2};
    vector<idx_t> payloadIndexTypes = {2}; // SUM over BIGINT
    auto aggFunc = SumFunc::getFunction(tLeft[payloadIndexTypes[0]]);
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    // Base table with 10 groups
    DataChunk base = createChunkWithValue(tLeft, 10);
    agg_ht_ptr ht;
    addChunkToAHT(ht, base, groupIndexTypes, payloadIndexTypes, functions, /*capacity=*/32);
    ht->finalize();


    // Probe with unseen  and see groups (same shape)
    DataChunk unseen = createChunkWithValue(tLeft, 5, /*offset=*/100);
    DataChunk seenAndUnseen = createChunkWithValue(tLeft, 5, /*offset=*/0);
    seenAndUnseen.setCapacity(10);
    seenAndUnseen.resize(10);
    seenAndUnseen.append(unseen);

    DataChunk payload = probeToHT(ht, seenAndUnseen, groupIndexTypes, functions);

    EXPECT_EQ(payload.getSize(), 5);
    // check first 5 rows should be equal to the seen chunk
    for (idx_t i = 0; i < payload.getSize(); ++i) {
        // Expected = 2 * (i * 10)
        int64_t expected = seenAndUnseen.data_[payloadIndexTypes[0]].getValue(i).cast(BIGINT).getNumericValue<int64_t>();
        int64_t got = payload.data_[0].getValue(i).cast(BIGINT).getNumericValue<int64_t>();
        EXPECT_EQ(got, expected) << "Row " << i << " mismatch";
    }
}

TEST_F(AggHTTest, Scan_IteratesAllGroupsInBatchesAndBoundary) {
    // Create many unique groups then walk them via scan().
    vector<idx_t> groupIndexTypes = {1, 2};
    vector<idx_t> payloadIndexTypes = {0}; // payload not relevant for scan
    auto aggFunc = SumFunc::getFunction(tLeft[payloadIndexTypes[0]]);
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    // Make 50 unique rows => 50 groups
    DataChunk chunk = createChunkWithValue(tLeft, 50);
    agg_ht_ptr ht;
    addChunkToAHT(ht, chunk, groupIndexTypes, payloadIndexTypes, functions, 128);

    // Total entries should be 50
    EXPECT_EQ(ht->getSize(), 50);

    // Prepare a result chunk with the same group types for scanning
    DataChunk out;
    out.initialize({tLeft[groupIndexTypes[0]], tLeft[groupIndexTypes[1]]});

    // Iterate from 0 until we've seen all groups
    idx_t visited = 0;
    idx_t pos = 0;
    while (pos < ht->getSize()) {
        idx_t got = ht->scan(pos, out);
        visited += got;
        pos += got;
    }
    EXPECT_EQ(visited, ht->getSize());

    // Boundary: scanning from last valid starting position should return exactly 1 row
    idx_t tail = ht->scan(ht->getSize() - 1, out);
    EXPECT_EQ(tail, 1);
}


TEST_F(AggHTTest, AddChunk_DuplicateGroupsAggregatesCombine) {
    // Group by cols [1,2]; SUM over col 2 (BIGINT). We'll add the *same* rows twice.
    vector<idx_t> groupIndexTypes = {1, 2};
    vector<idx_t> payloadIndexTypes = {2};
    auto aggFunc = SumFunc::getFunction(tLeft[payloadIndexTypes[0]]);
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    // Build the chunk with 8 rows
    DataChunk chunk = createChunkWithValue(tLeft, 8 );

    // Create HT and add the same chunk twice (duplicates)
    agg_ht_ptr ht1, ht2;
    addChunkToAHT(ht1, chunk, groupIndexTypes, payloadIndexTypes, functions, 16, true);
    addChunkToAHT(ht2, chunk, groupIndexTypes, payloadIndexTypes, functions, 16, true);

    // combine ht
    ht1->combine(*ht2);
    ht2 = nullptr;

    ht1->finalize();
    std::cout << ht1->toString(false) << std::endl;

    // Probe with the original groups; the SUM should be doubled of payload column
    DataChunk result = probeToHT(ht1, chunk, groupIndexTypes, functions);


    EXPECT_EQ(result.columnCount(), 1);
    EXPECT_EQ(result.getSize(), chunk.getSize());
    for (idx_t i = 0; i < result.getSize(); ++i) {
        // Expected = 2 * (i * 10)
        int64_t expected = 2 * chunk.data_[payloadIndexTypes[0]].getValue(i).cast(BIGINT).getNumericValue<int64_t>();
        int64_t got = result.data_[0].getValue(i).cast(BIGINT).getNumericValue<int64_t>();
        EXPECT_EQ(got, expected) << "Row " << i << " mismatch";
    }
}


TEST_F(AggHTTest, AddChunk_NoGroups) {
    // Group by cols [1,2]; SUM over col 2 (BIGINT). We'll add the *same* rows twice.
    vector<idx_t> groupIndexTypes = {};
    vector<idx_t> payloadIndexTypes = {2};
    auto aggFunc = SumFunc::getFunction(tLeft[payloadIndexTypes[0]]);
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    // Build the chunk with 8 rows
    DataChunk chunk = createChunkWithValue(tLeft, 8 );

    // Create HT and add the same chunk twice (duplicates)
    agg_ht_ptr ht1;
    addChunkToAHT(ht1, chunk, groupIndexTypes, payloadIndexTypes, functions, 32, true);
    addChunkToAHT(ht1, chunk, groupIndexTypes, payloadIndexTypes, functions, 32, true);

    ht1->finalize();

    // fetch the results
    DataChunk result;
    result.initializeEmpty({tLeft[payloadIndexTypes[0]]});
    result.setCapacity(1);
    ht1->fetchAggregates(result);
    EXPECT_EQ(result.getSize(), 1);
    auto expected = 560 * 2; // sum twice of all values in third column 0, 20, 40, 60, 80, 100, 120, 140
    EXPECT_EQ(result.getValue(0,0).cast(BIGINT).getNumericValue<int64_t>(), expected);

}