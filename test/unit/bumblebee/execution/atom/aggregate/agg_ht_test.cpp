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
#include "bumblebee/function/aggregate/Sum.hpp"
#include  "bumblebee/execution/AggregatePRLHashTable.hpp"
using namespace bumblebee;

using agg_ht_ptr = std::unique_ptr<AggregatePRLHashTable>;

class AggRLHashTableTest : public ::testing::Test {
    // Creates and returns a DataChunk initialized with a predefined set of column types: INTEGER, UINTEGER, and BIGINT.
    // For each of the count rows, it populates the columns with incrementing values:
    // Column 0 (INTEGER): sequential int32_t values starting from 0.
    // Column 1 (UINTEGER): uint32_t values equal to i * 10.
    // Column 2 (BIGINT): int64_t values equal to i * 100.
    // The function sets the cardinality of the chunk to count and returns it.
    // This utility is primarily used for generating consistent and type-diverse data for testing the ChunkCollection class.
protected:

    vector<PhysicalType> tLeft{PhysicalType::SMALLINT, PhysicalType::UINTEGER, PhysicalType::BIGINT};
    ClientContext clientContext;

    DataChunk createChunkWithValue( vector<PhysicalType> testTypes, idx_t count = 1, idx_t offset=0 ) {
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
        vector<LogicalType> groupTypes, payloadTypes;
        for (auto g : groups)
            groupTypes.push_back(chunk.getTypes()[g]);
        for (auto p : payloads)
            payloadTypes.push_back(chunk.getTypes()[p]);

        if (!ht)
            ht = agg_ht_ptr(new AggregatePRLHashTable(*clientContext.bufferManager_, groupTypes, capacity, resize,  functions));

        group.initialize(groupTypes);
        payload.initialize(payloadTypes);
        group.reference(chunk, groups);
        payload.reference(chunk, payloads);
        Vector hash(LogicalTypeId::HASH, group.getSize());
        group.hash(hash);
        ht->addChunk(hash, group, payload);
    }


    DataChunk probeToHT(agg_ht_ptr& ht, DataChunk &chunk, vector<idx_t> groups, vector<AggregateFunction*> functions) {
        vector<LogicalType> groupTypes, payloadTypes;
        for (auto g : groups)
            groupTypes.push_back(chunk.getTypes()[g]);
        for (auto f : functions)
            payloadTypes.push_back(f->result_);

        DataChunk group, payload;
        group.initialize(groupTypes);
        group.reference(chunk, groups);
        payload.initialize(payloadTypes);
        payload.setCardinality(group.getSize());

        Vector hash(LogicalTypeId::HASH, group.getSize());
        group.hash(hash);

        SelectionVector sel(group.getSize());
        ht->fetchAggregates(hash, group, payload,sel);
        return payload;
    }

};

TEST_F(AggRLHashTableTest, AggHTSimpleTest) {
    vector<idx_t> groupIndexTypes = {1,2};
    vector<idx_t> payloadIndexTypes = {0};
    DataChunk chunk = createChunkWithValue(tLeft, 10);

    auto aggFunc = SumFunc().getFunction({tLeft[payloadIndexTypes[0]]});
    vector functions = {(AggregateFunction*)aggFunc.get()};
    agg_ht_ptr ht;
    addChunkToAHT(ht, chunk, groupIndexTypes, payloadIndexTypes, functions, 32);


    DataChunk payload = probeToHT(ht, chunk, groupIndexTypes, functions);
    // check that is all 0
    for (idx_t i = 0; i < payload.getSize(); i++)
        EXPECT_EQ(payload.data_[0].getValue(i).cast(PhysicalType::BIGINT).getNumericValue<int64_t>(), 0 );
}


TEST_F(AggRLHashTableTest, AddChunk_DuplicateGroupsAggregatesCorrectlyNoDistinc) {
    // Group by cols [1,2]; SUM over col 2 (BIGINT). We'll add the *same* rows twice.
    vector<idx_t> groupIndexTypes = {1, 2};
    vector<idx_t> payloadIndexTypes = {2};
    auto aggFunc = SumFunc().getFunction({tLeft[payloadIndexTypes[0]]});
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    // Build the chunk with 8 rows
    DataChunk chunk = createChunkWithValue(tLeft, 8 );

    // Create HT and add the same chunk twice (duplicates)
    agg_ht_ptr ht;
    // call with no distinc cols
    addChunkToAHT(ht, chunk, groupIndexTypes, payloadIndexTypes ,functions, 32);
    addChunkToAHT(ht, chunk, groupIndexTypes, payloadIndexTypes, functions, 32);


    // Probe with the original groups; the SUM should be doubled of payload column
    DataChunk result = probeToHT(ht, chunk, groupIndexTypes, functions);

    EXPECT_EQ(result.columnCount(), 1);
    EXPECT_EQ(result.getSize(), chunk.getSize());
    for (idx_t i = 0; i < result.getSize(); ++i) {
        // Expected = 2 * (i * 10)
        int64_t expected = 2 * chunk.data_[payloadIndexTypes[0]].getValue(i).cast(PhysicalType::BIGINT).getNumericValue<int64_t>();
        int64_t got = result.data_[0].getValue(i).cast(PhysicalType::BIGINT).getNumericValue<int64_t>();
        EXPECT_EQ(got, expected) << "Row " << i << " mismatch";
    }
}


TEST_F(AggRLHashTableTest, FetchAggregates_UnseenGroupReturnsZeroAndCreatesEntry) {
    // Build an HT with groups for i in [0..9], then probe with a *new* group (offset=100)
    vector<idx_t> groupIndexTypes = {1, 2};
    vector<idx_t> payloadIndexTypes = {2}; // SUM over BIGINT
    auto aggFunc = SumFunc().getFunction({tLeft[payloadIndexTypes[0]]});
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    // Base table with 10 groups
    DataChunk base = createChunkWithValue(tLeft, 10);
    agg_ht_ptr ht;
    addChunkToAHT(ht, base, groupIndexTypes, payloadIndexTypes, functions, /*capacity=*/32);

    // Probe with unseen groups (same shape, but offset so keys don't exist yet)
    DataChunk unseen = createChunkWithValue(tLeft, 5, /*offset=*/100);
    DataChunk payload = probeToHT(ht, unseen, groupIndexTypes, functions);

    EXPECT_EQ(payload.getSize(), 0);
}



TEST_F(AggRLHashTableTest, FetchAggregates_UnseenAndSeeGroup) {
    // Build an HT with groups for i in [0..9], then probe with a *new* group (offset=100)
    vector<idx_t> groupIndexTypes = {1, 2};
    vector<idx_t> payloadIndexTypes = {2}; // SUM over BIGINT
    auto aggFunc = SumFunc().getFunction({tLeft[payloadIndexTypes[0]]});
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    // Base table with 10 groups
    DataChunk base = createChunkWithValue(tLeft, 10);
    agg_ht_ptr ht;
    addChunkToAHT(ht, base, groupIndexTypes, payloadIndexTypes, functions, /*capacity=*/32);

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
        int64_t expected = seenAndUnseen.data_[payloadIndexTypes[0]].getValue(i).cast(PhysicalType::BIGINT).getNumericValue<int64_t>();
        int64_t got = payload.data_[0].getValue(i).cast(PhysicalType::BIGINT).getNumericValue<int64_t>();
        EXPECT_EQ(got, expected) << "Row " << i << " mismatch";
    }
}

TEST_F(AggRLHashTableTest, Scan_IteratesAllGroupsInBatchesAndBoundary) {
    // Create many unique groups then walk them via scan().
    vector<idx_t> groupIndexTypes = {1, 2};
    vector<idx_t> payloadIndexTypes = {0}; // payload not relevant for scan
    auto aggFunc = SumFunc().getFunction({tLeft[payloadIndexTypes[0]]});
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    // Make 50 unique rows => 50 groups
    DataChunk chunk = createChunkWithValue(tLeft, 50);
    agg_ht_ptr ht;
    addChunkToAHT(ht, chunk, groupIndexTypes, payloadIndexTypes, functions, 128);

    // Total entries should be 50
    EXPECT_EQ(ht->getSize(), 50);

    // Prepare a result chunk with the same group types for scanning
    DataChunk out;
    out.initialize((vector<PhysicalType>){tLeft[groupIndexTypes[0]], tLeft[groupIndexTypes[1]]});

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


TEST_F(AggRLHashTableTest, AddChunk_DuplicateGroupsAggregatesCombine) {
    // Group by cols [1,2]; SUM over col 2 (BIGINT). We'll add the *same* rows twice.
    vector<idx_t> groupIndexTypes = {1, 2};
    vector<idx_t> payloadIndexTypes = {2};
    auto aggFunc = SumFunc().getFunction({tLeft[payloadIndexTypes[0]]});
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

    std::cout << ht1->toString(false) << std::endl;

    // Probe with the original groups; the SUM should be doubled of payload column
    DataChunk result = probeToHT(ht1, chunk, groupIndexTypes, functions);


    EXPECT_EQ(result.columnCount(), 1);
    EXPECT_EQ(result.getSize(), chunk.getSize());
    for (idx_t i = 0; i < result.getSize(); ++i) {
        // Expected = 2 * (i * 10)
        int64_t expected = 2 * chunk.data_[payloadIndexTypes[0]].getValue(i).cast(PhysicalType::BIGINT).getNumericValue<int64_t>();
        int64_t got = result.data_[0].getValue(i).cast(PhysicalType::BIGINT).getNumericValue<int64_t>();
        EXPECT_EQ(got, expected) << "Row " << i << " mismatch";
    }
}


TEST_F(AggRLHashTableTest, AddChunk_NoGroups) {
    // Group by cols [1,2]; SUM over col 2 (BIGINT). We'll add the *same* rows twice.
    vector<idx_t> groupIndexTypes = {};
    vector<idx_t> payloadIndexTypes = {2};
    auto aggFunc = SumFunc().getFunction({tLeft[payloadIndexTypes[0]]});
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    // Build the chunk with 8 rows
    DataChunk chunk = createChunkWithValue(tLeft, 8 );

    // Create HT and add the same chunk twice (duplicates)
    agg_ht_ptr ht1;
    addChunkToAHT(ht1, chunk, groupIndexTypes, payloadIndexTypes, functions, 32, true);
    addChunkToAHT(ht1, chunk, groupIndexTypes, payloadIndexTypes, functions, 32, true);


    // fetch the results
    DataChunk result;
    result.initialize((vector<PhysicalType>){tLeft[payloadIndexTypes[0]]});
    result.setCapacity(1);
    ht1->fetchAggregates(result);
    EXPECT_EQ(result.getSize(), 1);
    auto expected = 560 * 2; // sum twice of all values in third column 0, 20, 40, 60, 80, 100, 120, 140
    EXPECT_EQ(result.getValue(0,0).cast(PhysicalType::BIGINT).getNumericValue<int64_t>(), expected);

}

TEST_F(AggRLHashTableTest, ScanWithAggregates_IteratesAllGroupsWithResults) {
    // Test scanWithAggregates: scan the hash table and get both group values and aggregate results
    vector<idx_t> groupIndexTypes = {1, 2};
    vector<idx_t> payloadIndexTypes = {0}; // SUM over SMALLINT (col 0)
    auto aggFunc = SumFunc().getFunction({tLeft[payloadIndexTypes[0]]});
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    // Create 10 unique groups
    DataChunk chunk = createChunkWithValue(tLeft, 10);
    agg_ht_ptr ht;
    addChunkToAHT(ht, chunk, groupIndexTypes, payloadIndexTypes, functions, 32);

    EXPECT_EQ(ht->getSize(), 10);

    // Prepare output chunks for scanning
    vector<LogicalType> groupTypes = {tLeft[groupIndexTypes[0]], tLeft[groupIndexTypes[1]]};
    DataChunk groups;
    groups.initialize(groupTypes);
    Vector aggResult(aggFunc->result_);

    // Scan all entries at once
    idx_t scanned = ht->scanWithAggregates(0, groups, aggResult, 0);
    EXPECT_EQ(scanned, 10);
    EXPECT_EQ(groups.getSize(), 10);

    // Verify we got all group values and correct aggregate results
    // The aggregate result for each group should match what we'd get from fetchAggregates
    DataChunk probeResult = probeToHT(ht, chunk, groupIndexTypes, functions);
    EXPECT_EQ(probeResult.getSize(), 10);

    // Both methods should return the same aggregate values (order may differ due to hash table)
    // Sum all aggregate results from both methods - they should be equal
    int64_t scanSum = 0, probeSum = 0;
    for (idx_t i = 0; i < 10; ++i) {
        scanSum += aggResult.getValue(i).cast(PhysicalType::BIGINT).getNumericValue<int64_t>();
        probeSum += probeResult.data_[0].getValue(i).cast(PhysicalType::BIGINT).getNumericValue<int64_t>();
    }
    EXPECT_EQ(scanSum, probeSum);
}

TEST_F(AggRLHashTableTest, ScanWithAggregates_BatchIteration) {
    // Test scanWithAggregates with batched iteration (smaller batch size)
    vector<idx_t> groupIndexTypes = {1, 2};
    vector<idx_t> payloadIndexTypes = {2}; // SUM over BIGINT (col 2)
    auto aggFunc = SumFunc().getFunction({tLeft[payloadIndexTypes[0]]});
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    // Create 25 unique groups
    DataChunk chunk = createChunkWithValue(tLeft, 25);
    agg_ht_ptr ht;
    addChunkToAHT(ht, chunk, groupIndexTypes, payloadIndexTypes, functions, 64);

    EXPECT_EQ(ht->getSize(), 25);

    // Scan in batches of 10
    vector<LogicalType> groupTypes = {tLeft[groupIndexTypes[0]], tLeft[groupIndexTypes[1]]};
    idx_t totalScanned = 0;
    idx_t offset = 0;
    int64_t totalAggSum = 0;

    while (offset < ht->getSize()) {
        DataChunk groups;
        groups.initialize(groupTypes);
        Vector aggResult(aggFunc->result_);

        idx_t scanned = ht->scanWithAggregates(offset, groups, aggResult, 0, 10);
        EXPECT_GT(scanned, 0);
        totalScanned += scanned;
        offset += scanned;

        // Accumulate aggregate sum
        for (idx_t i = 0; i < scanned; ++i) {
            totalAggSum += aggResult.getValue(i).cast(PhysicalType::BIGINT).getNumericValue<int64_t>();
        }
    }

    EXPECT_EQ(totalScanned, 25);

    // Verify total sum matches expected
    // Each row i has payload value = i * 20 (col 2 = i * 10 * 2)
    int64_t expectedSum = 0;
    for (idx_t i = 0; i < 25; ++i) {
        expectedSum += i * 20;
    }
    EXPECT_EQ(totalAggSum, expectedSum);
}

TEST_F(AggRLHashTableTest, ScanWithAggregates_AllPayloads) {
    // Test scanWithAggregates that returns all aggregate results in a DataChunk
    // Use multiple aggregation functions on different columns
    vector<idx_t> groupIndexTypes = {1};  // Group by col 1 (UINTEGER)
    vector<idx_t> payloadIndexTypes = {0, 2};  // SUM over col 0 (SMALLINT) and col 2 (BIGINT)

    auto aggFunc1 = SumFunc().getFunction({tLeft[0]});  // SUM(SMALLINT)
    auto aggFunc2 = SumFunc().getFunction({tLeft[2]});  // SUM(BIGINT)
    vector<AggregateFunction*> functions = {
        (AggregateFunction*)aggFunc1.get(),
        (AggregateFunction*)aggFunc2.get()
    };

    // Create 8 unique groups
    DataChunk chunk = createChunkWithValue(tLeft, 8);

    // Build hash table with multiple payloads
    vector<LogicalType> groupTypes = {tLeft[groupIndexTypes[0]]};
    vector<LogicalType> payloadTypes = {tLeft[payloadIndexTypes[0]], tLeft[payloadIndexTypes[1]]};

    agg_ht_ptr ht(new AggregatePRLHashTable(*clientContext.bufferManager_, groupTypes, 32, false, functions));

    DataChunk groups, payload;
    groups.initialize(groupTypes);
    payload.initialize(payloadTypes);
    groups.reference(chunk, groupIndexTypes);
    payload.reference(chunk, payloadIndexTypes);
    Vector hash(LogicalTypeId::HASH, groups.getSize());
    groups.hash(hash);
    ht->addChunk(hash, groups, payload);

    EXPECT_EQ(ht->getSize(), 8);

    // Prepare output chunks for scanning
    DataChunk outGroups;
    outGroups.initialize(groupTypes);
    DataChunk aggResults;
    aggResults.initialize({aggFunc1->result_, aggFunc2->result_});

    // Scan all entries at once
    idx_t scanned = ht->scanWithAggregates(0, outGroups, aggResults);
    EXPECT_EQ(scanned, 8);
    EXPECT_EQ(outGroups.getSize(), 8);
    EXPECT_EQ(aggResults.getSize(), 8);
    EXPECT_EQ(aggResults.columnCount(), 2);

    // Verify both aggregate columns have values
    int64_t sum1 = 0, sum2 = 0;
    for (idx_t i = 0; i < scanned; ++i) {
        sum1 += aggResults.data_[0].getValue(i).cast(PhysicalType::BIGINT).getNumericValue<int64_t>();
        sum2 += aggResults.data_[1].getValue(i).cast(PhysicalType::BIGINT).getNumericValue<int64_t>();
    }

    // Expected sums based on createChunkWithValue formula: col j at row i = (i * 10 * j)
    // col 0: all 0s (j=0), col 2: 0, 20, 40, 60, 80, 100, 120, 140 (j=2)
    EXPECT_EQ(sum1, 0);  // sum of col 0 = all zeros
    int64_t expectedSum2 = 0;
    for (idx_t i = 0; i < 8; ++i) {
        expectedSum2 += i * 20;  // col 2 values
    }
    EXPECT_EQ(sum2, expectedSum2);
}

TEST_F(AggRLHashTableTest, ScanWithAggregates_EmptyHashTable) {
    // Test scanWithAggregates on an empty hash table
    vector<idx_t> groupIndexTypes = {1, 2};
    vector<idx_t> payloadIndexTypes = {0};
    auto aggFunc = SumFunc().getFunction({tLeft[payloadIndexTypes[0]]});
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    // Create hash table but don't add any data
    vector<LogicalType> groupTypes = {tLeft[groupIndexTypes[0]], tLeft[groupIndexTypes[1]]};
    agg_ht_ptr ht(new AggregatePRLHashTable(*clientContext.bufferManager_, groupTypes, 32, false, functions));

    EXPECT_EQ(ht->getSize(), 0);

    // Try to scan empty hash table
    DataChunk groups;
    groups.initialize(groupTypes);
    Vector aggResult(aggFunc->result_);

    idx_t scanned = ht->scanWithAggregates(0, groups, aggResult, 0);
    EXPECT_EQ(scanned, 0);
    EXPECT_EQ(groups.getSize(), 0);
}

TEST_F(AggRLHashTableTest, ScanWithAggregates_SingleEntry) {
    // Test scanWithAggregates with exactly 1 group (edge case)
    vector<idx_t> groupIndexTypes = {1, 2};
    vector<idx_t> payloadIndexTypes = {0};
    auto aggFunc = SumFunc().getFunction({tLeft[payloadIndexTypes[0]]});
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    // Create chunk with 1 unique row
    DataChunk chunk = createChunkWithValue(tLeft, 1);
    agg_ht_ptr ht;
    addChunkToAHT(ht, chunk, groupIndexTypes, payloadIndexTypes, functions, 32);

    EXPECT_EQ(ht->getSize(), 1);

    // Scan the single entry
    vector<LogicalType> groupTypes = {tLeft[groupIndexTypes[0]], tLeft[groupIndexTypes[1]]};
    DataChunk groups;
    groups.initialize(groupTypes);
    Vector aggResult(aggFunc->result_);

    idx_t scanned = ht->scanWithAggregates(0, groups, aggResult, 0);
    EXPECT_EQ(scanned, 1);
    EXPECT_EQ(groups.getSize(), 1);

    // Verify the aggregate result matches probe result
    DataChunk probeResult = probeToHT(ht, chunk, groupIndexTypes, functions);
    EXPECT_EQ(probeResult.getSize(), 1);
    EXPECT_EQ(aggResult.getValue(0).cast(PhysicalType::BIGINT).getNumericValue<int64_t>(),
              probeResult.data_[0].getValue(0).cast(PhysicalType::BIGINT).getNumericValue<int64_t>());
}

TEST_F(AggRLHashTableTest, ScanWithAggregates_OffsetPastEnd) {
    // Test scanWithAggregates with offset >= entries (should return 0)
    vector<idx_t> groupIndexTypes = {1, 2};
    vector<idx_t> payloadIndexTypes = {0};
    auto aggFunc = SumFunc().getFunction({tLeft[payloadIndexTypes[0]]});
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    // Create 5 unique groups
    DataChunk chunk = createChunkWithValue(tLeft, 5);
    agg_ht_ptr ht;
    addChunkToAHT(ht, chunk, groupIndexTypes, payloadIndexTypes, functions, 32);

    EXPECT_EQ(ht->getSize(), 5);

    vector<LogicalType> groupTypes = {tLeft[groupIndexTypes[0]], tLeft[groupIndexTypes[1]]};
    DataChunk groups;
    groups.initialize(groupTypes);
    Vector aggResult(aggFunc->result_);

    // Scan with offset = entries (should return 0)
    idx_t scanned = ht->scanWithAggregates(5, groups, aggResult, 0);
    EXPECT_EQ(scanned, 0);
    EXPECT_EQ(groups.getSize(), 0);

    // Scan with offset > entries (should return 0)
    scanned = ht->scanWithAggregates(100, groups, aggResult, 0);
    EXPECT_EQ(scanned, 0);
    EXPECT_EQ(groups.getSize(), 0);
}