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
#include "bumblebee/function/aggregate/Sum.hpp"
#include "bumblebee/function/aggregate/Count.hpp"
#include "bumblebee/execution/PartitionedAggHT.hpp"

using namespace bumblebee;


class PartitionedAggHTTest : public ::testing::Test {
protected:

    vector<LogicalType> tLeft{PhysicalType::SMALLINT, PhysicalType::UINTEGER, PhysicalType::BIGINT};
    ClientContext context;

    DataChunk createChunkWithValue( vector<LogicalType> testTypes, idx_t count = 1, idx_t offset=0 ) {
        DataChunk chunk;
        chunk.initialize(testTypes);
        chunk.setCapacity(count);
        chunk.resize(count);
        for (idx_t i = 0; i < count; ++i) {
            for (idx_t j = 0; j < testTypes.size(); ++j) {
                auto value = Value((int64_t) ((i+offset)*10*j));
                chunk.setValue(j, i, value.cast(testTypes[j].getPhysicalType()));
            }
        }
        chunk.setCardinality(count);
        return chunk;
    }

    void addChunkToHT(distinct_ht_ptr_t& ht, DataChunk &chunk, idx_t capacity = MORSEL_SIZE,
            bool resize = false) {

        if (!ht)
            ht = distinct_ht_ptr_t(new PRLHashTable(*context.bufferManager_, tLeft, capacity, resize));

        Vector hash(LogicalTypeId::HASH, chunk.getSize());
        chunk.hash(hash);
        ht->addChunk(hash, chunk);
    }

    void addHTToPartitionedHT(partitioned_agg_ht_ptr_t& pht, distinct_ht_ptr_t& ht, vector<idx_t> groups,
            vector<idx_t> payloads, vector<AggregateFunction*> functions, idx_t capacity = MORSEL_SIZE,
            bool resize = false, bool distinct = true) {
        if (!pht)
            pht = partitioned_agg_ht_ptr_t(new PartitionedAggHT(context, groups, payloads, functions, 0));
        if (distinct)
            pht->setDistinct();
        DataChunk chunk;
        chunk.initialize(ht->getTypes());
        idx_t offset = 0;
        while (true) {
            offset += ht->scan(offset, chunk);
            pht->addChunk(chunk);
            if (offset >= ht->getSize()) break;
        }
    }

    // Scan all entries from a PartitionedAggHT and return group->agg mappings.
    // groupCols specifies which group column types to use.
    // Returns a map from stringified group tuple to aggregate value.
    std::unordered_map<string, int64_t> scanAll(PartitionedAggHT& pht,
            const vector<LogicalType>& groupTypes, const vector<AggregateFunction*>& functions) {
        vector<LogicalType> aggTypes;
        for (auto* f : functions)
            aggTypes.push_back(f->result_);

        DataChunk groups, aggResults;
        groups.initialize(groupTypes);
        aggResults.initialize(aggTypes);

        std::unordered_map<string, int64_t> result;
        idx_t offset = 0;
        while (true) {
            idx_t scanned = pht.scanWithAggregates(offset, groups, aggResults);
            if (scanned == 0) break;
            for (idx_t i = 0; i < scanned; ++i) {
                string key;
                for (idx_t g = 0; g < groupTypes.size(); ++g)
                    key += groups.getValue(g, i).toString() + ",";
                auto aggVal = aggResults.getValue(0, i).cast(PhysicalType::BIGINT).getNumericValue<int64_t>();
                result[key] = aggVal;
            }
            offset += scanned;
        }
        return result;
    }

    // Scan all entries and return total count of rows
    idx_t scanAllCount(PartitionedAggHT& pht,
            const vector<LogicalType>& groupTypes, const vector<AggregateFunction*>& functions) {
        vector<LogicalType> aggTypes;
        for (auto* f : functions)
            aggTypes.push_back(f->result_);

        DataChunk groups, aggResults;
        groups.initialize(groupTypes);
        aggResults.initialize(aggTypes);

        idx_t total = 0;
        idx_t offset = 0;
        while (true) {
            idx_t scanned = pht.scanWithAggregates(offset, groups, aggResults);
            if (scanned == 0) break;
            total += scanned;
            offset += scanned;
        }
        return total;
    }
};


TEST_F(PartitionedAggHTTest, AddChunk_DuplicateGroupsAggregatesCorrectlyWithDistinct) {
    // Group by cols [1,2]; SUM over col 2 (BIGINT). We'll add the *same* rows twice.
    vector<idx_t> groupIndexTypes = {1, 2};
    vector<idx_t> payloadIndexTypes = {2};
    auto aggFunc = SumFunc().getFunction({tLeft[payloadIndexTypes[0]]});
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    // Build the chunk with 8 rows
    DataChunk chunk = createChunkWithValue(tLeft, 8);

    // Create HT and add the same chunk twice (duplicates)
    distinct_ht_ptr_t ht1, ht2;
    addChunkToHT(ht1, chunk, 16, false);
    addChunkToHT(ht2, chunk, 16, false);

    partitioned_agg_ht_ptr_t pht;
    addHTToPartitionedHT(pht, ht1, groupIndexTypes, payloadIndexTypes, functions);
    addHTToPartitionedHT(pht, ht2, groupIndexTypes, payloadIndexTypes, functions);

    pht->finalize();

    // Scan all groups; with distinct, SUM should NOT double
    vector<LogicalType> groupTypes;
    for (auto g : groupIndexTypes)
        groupTypes.push_back(tLeft[g]);
    auto result = scanAll(*pht, groupTypes, functions);

    EXPECT_EQ(result.size(), chunk.getSize());
    for (idx_t i = 0; i < chunk.getSize(); ++i) {
        string key;
        for (auto g : groupIndexTypes)
            key += chunk.getValue(g, i).toString() + ",";
        int64_t expected = chunk.data_[payloadIndexTypes[0]].getValue(i).cast(PhysicalType::BIGINT).getNumericValue<int64_t>();
        ASSERT_TRUE(result.count(key)) << "Missing group " << key;
        EXPECT_EQ(result[key], expected) << "Row " << i << " mismatch";
    }
}


TEST_F(PartitionedAggHTTest, AddChunk_DuplicateGroupsAggregatesCorrectlyWithoutDistinct) {
    // Group by cols [1,2]; SUM over col 2 (BIGINT). We'll add the *same* rows twice.
    vector<idx_t> groupIndexTypes = {1, 2};
    vector<idx_t> payloadIndexTypes = {2};
    auto aggFunc = SumFunc().getFunction({tLeft[payloadIndexTypes[0]]});
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    // Build the chunk with 8 rows
    DataChunk chunk = createChunkWithValue(tLeft, 8);

    // Create HT and add the same chunk twice (duplicates)
    distinct_ht_ptr_t ht1, ht2;
    addChunkToHT(ht1, chunk, 16, false);
    addChunkToHT(ht2, chunk, 16, false);

    partitioned_agg_ht_ptr_t pht;
    addHTToPartitionedHT(pht, ht1, groupIndexTypes, payloadIndexTypes, functions, STANDARD_VECTOR_SIZE, false, false);
    addHTToPartitionedHT(pht, ht2, groupIndexTypes, payloadIndexTypes, functions, STANDARD_VECTOR_SIZE, false, false);

    pht->finalize();

    // Scan all groups; without distinct, SUM should be doubled
    vector<LogicalType> groupTypes;
    for (auto g : groupIndexTypes)
        groupTypes.push_back(tLeft[g]);
    auto result = scanAll(*pht, groupTypes, functions);

    EXPECT_EQ(result.size(), chunk.getSize());
    for (idx_t i = 0; i < chunk.getSize(); ++i) {
        string key;
        for (auto g : groupIndexTypes)
            key += chunk.getValue(g, i).toString() + ",";
        int64_t expected = chunk.data_[payloadIndexTypes[0]].getValue(i).cast(PhysicalType::BIGINT).getNumericValue<int64_t>() * 2;
        ASSERT_TRUE(result.count(key)) << "Missing group " << key;
        EXPECT_EQ(result[key], expected) << "Row " << i << " mismatch";
    }
}

// Columns-with-same-values test
TEST_F(PartitionedAggHTTest, DistinctColumns_AggregatesCorrectly1) {
    vector<idx_t> groups = {0, 1};
    vector<idx_t> payloads = {2};
    auto aggFunc = SumFunc().getFunction({tLeft[payloads[0]]}); // Sum over BIGINT
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    const idx_t N = 12;

    DataChunk chunk;
    chunk.initialize(tLeft);
    chunk.setCapacity(N);
    chunk.resize(N);
    for (idx_t i = 0; i < N; ++i) {
        int64_t v = static_cast<int64_t>(100);
        for (idx_t j = 0; j < tLeft.size(); ++j) {
            chunk.setValue(j, i, Value(v * j).cast(tLeft[j].getPhysicalType()));
        }
    }
    chunk.setCardinality(N);

    distinct_ht_ptr_t ht1;
    addChunkToHT(ht1, chunk, 32, false);
    partitioned_agg_ht_ptr_t pht;
    addHTToPartitionedHT(pht, ht1, groups, payloads, functions);

    pht->finalize();

    vector<LogicalType> groupTypes;
    for (auto g : groups)
        groupTypes.push_back(tLeft[g]);
    auto result = scanAll(*pht, groupTypes, functions);

    // All rows have the same group key, so only 1 group after distinct
    ASSERT_EQ(result.size(), 1);
    // SUM should be 200 (value is 200 for col 2 which is 100*2)
    for (auto& [key, val] : result) {
        EXPECT_EQ(val, 200);
    }
}


TEST_F(PartitionedAggHTTest, Partition_Shift62) {
    vector<idx_t> groupIndexTypes = {0, 2};
    vector<idx_t> payloadIndexTypes = {2};
    auto aggFunc = SumFunc().getFunction({tLeft[payloadIndexTypes[0]]});
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    DataChunk chunk = createChunkWithValue(tLeft, 16);
    distinct_ht_ptr_t ht;
    addChunkToHT(ht, chunk, 32, false);

    EXPECT_EQ(ht->getSize(), 16);

    std::vector<distinct_ht_ptr_t> partitions(4);
    ht->partition(partitions, 62);
    auto pSize = 0;
    for (auto& p : partitions)
        if (p) pSize += p->getSize();
    EXPECT_EQ(pSize, ht->getSize());

    // now combine all the partitions
    distinct_ht_ptr_t htFinal;
    for (auto& p : partitions) {
        if (!p) continue;
        if (!htFinal) {
            htFinal = std::move(p);
            continue;
        };
        htFinal->combine(*p);
        p = nullptr;
    }
    EXPECT_EQ(htFinal->getSize(), ht->getSize());
}


// ====== scanWithAggregates tests ======

// Test: scan on an empty table returns 0
TEST_F(PartitionedAggHTTest, ScanWithAggregates_EmptyTable) {
    vector<idx_t> groups = {0};
    vector<idx_t> payloads = {2};
    auto aggFunc = SumFunc().getFunction({tLeft[payloads[0]]});
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    partitioned_agg_ht_ptr_t pht(new PartitionedAggHT(context, groups, payloads, functions, 0));

    vector<LogicalType> groupTypes = {tLeft[0]};
    vector<LogicalType> aggTypes = {((AggregateFunction*)aggFunc.get())->result_};
    DataChunk grp, agg;
    grp.initialize(groupTypes);
    agg.initialize(aggTypes);

    idx_t scanned = pht->scanWithAggregates(0, grp, agg);
    EXPECT_EQ(scanned, 0);
}

// Test: scan after finalize returns correct results
TEST_F(PartitionedAggHTTest, ScanWithAggregates_AfterFinalize) {
    vector<idx_t> groups = {0, 1};
    vector<idx_t> payloads = {2};
    auto aggFunc = SumFunc().getFunction({tLeft[payloads[0]]});
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    DataChunk chunk = createChunkWithValue(tLeft, 16);

    distinct_ht_ptr_t ht;
    addChunkToHT(ht, chunk, 32, false);
    partitioned_agg_ht_ptr_t pht;
    addHTToPartitionedHT(pht, ht, groups, payloads, functions);

    pht->finalize();
    EXPECT_TRUE(pht->isReady());

    vector<LogicalType> groupTypes;
    for (auto g : groups)
        groupTypes.push_back(tLeft[g]);
    auto totalRows = scanAllCount(*pht, groupTypes, functions);
    EXPECT_EQ(totalRows, pht->getSize());
}

// Test: scan without finalize returns same results as with finalize
TEST_F(PartitionedAggHTTest, ScanWithAggregates_WithoutFinalize) {
    vector<idx_t> groups = {0, 1};
    vector<idx_t> payloads = {2};
    auto aggFunc = SumFunc().getFunction({tLeft[payloads[0]]});
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    DataChunk chunk = createChunkWithValue(tLeft, 16);

    // Build two identical PHTs
    distinct_ht_ptr_t ht1, ht2;
    addChunkToHT(ht1, chunk, 32, false);
    addChunkToHT(ht2, chunk, 32, false);

    partitioned_agg_ht_ptr_t pht1, pht2;
    addHTToPartitionedHT(pht1, ht1, groups, payloads, functions);
    addHTToPartitionedHT(pht2, ht2, groups, payloads, functions);

    pht1->finalize();
    // pht2 is NOT finalized

    vector<LogicalType> groupTypes;
    for (auto g : groups)
        groupTypes.push_back(tLeft[g]);

    auto result1 = scanAll(*pht1, groupTypes, functions);
    auto result2 = scanAll(*pht2, groupTypes, functions);

    EXPECT_EQ(result1.size(), result2.size());
    for (auto& [key, val] : result1) {
        ASSERT_TRUE(result2.count(key)) << "Missing group " << key << " in non-finalized scan";
        EXPECT_EQ(result2[key], val) << "Mismatch for group " << key;
    }
}

// Test: some partitions are empty (few distinct groups, many partitions)
TEST_F(PartitionedAggHTTest, ScanWithAggregates_SomePartitionsEmpty) {
    // Group by col 1 (UINTEGER) which has distinct values, unlike col 0 which is always 0
    vector<idx_t> groups = {1};
    vector<idx_t> payloads = {2};
    auto aggFunc = SumFunc().getFunction({tLeft[payloads[0]]});
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    // 4 distinct groups, 64 partitions -> most partitions will be empty
    DataChunk chunk = createChunkWithValue(tLeft, 4);

    distinct_ht_ptr_t ht;
    addChunkToHT(ht, chunk, 16, false);

    partitioned_agg_ht_ptr_t pht(new PartitionedAggHT(context, groups, payloads, functions, 0, 64));
    pht->setDistinct();
    DataChunk scanChunk;
    scanChunk.initialize(ht->getTypes());
    idx_t offset = 0;
    while (true) {
        offset += ht->scan(offset, scanChunk);
        pht->addChunk(scanChunk);
        if (offset >= ht->getSize()) break;
    }

    EXPECT_FALSE(pht->isReady());

    vector<LogicalType> groupTypes = {tLeft[1]};
    auto totalRows = scanAllCount(*pht, groupTypes, functions);
    EXPECT_EQ(totalRows, 4);
}

// Test: uneven distribution across partitions
TEST_F(PartitionedAggHTTest, ScanWithAggregates_UnevenDistribution) {
    vector<idx_t> groups = {0, 1};
    vector<idx_t> payloads = {2};
    auto aggFunc = SumFunc().getFunction({tLeft[payloads[0]]});
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    // Create a chunk with 100 rows where some group keys repeat heavily
    DataChunk chunk = createChunkWithValue(tLeft, 100);

    distinct_ht_ptr_t ht;
    addChunkToHT(ht, chunk, 256, false);
    partitioned_agg_ht_ptr_t pht;
    addHTToPartitionedHT(pht, ht, groups, payloads, functions);

    // Scan without finalize
    vector<LogicalType> groupTypes;
    for (auto g : groups)
        groupTypes.push_back(tLeft[g]);
    auto totalRows = scanAllCount(*pht, groupTypes, functions);
    EXPECT_EQ(totalRows, pht->getSize());
}

// Test: equally distributed data
TEST_F(PartitionedAggHTTest, ScanWithAggregates_EquallyDistributed) {
    vector<idx_t> groups = {0, 1};
    vector<idx_t> payloads = {2};
    auto aggFunc = SumFunc().getFunction({tLeft[payloads[0]]});
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    // 200 distinct rows spread across 4 partitions
    DataChunk chunk = createChunkWithValue(tLeft, 200);

    distinct_ht_ptr_t ht;
    addChunkToHT(ht, chunk, 512, false);

    partitioned_agg_ht_ptr_t pht(new PartitionedAggHT(context, groups, payloads, functions, 0, 4));
    pht->setDistinct();
    DataChunk scanChunk;
    scanChunk.initialize(ht->getTypes());
    idx_t offset = 0;
    while (true) {
        offset += ht->scan(offset, scanChunk);
        pht->addChunk(scanChunk);
        if (offset >= ht->getSize()) break;
    }

    vector<LogicalType> groupTypes;
    for (auto g : groups)
        groupTypes.push_back(tLeft[g]);

    // Scan with finalize
    auto phtCopyForFinalize = partitioned_agg_ht_ptr_t(new PartitionedAggHT(context, groups, payloads, functions, 0, 4));
    phtCopyForFinalize->setDistinct();
    distinct_ht_ptr_t ht2;
    addChunkToHT(ht2, chunk, 512, false);
    DataChunk scanChunk2;
    scanChunk2.initialize(ht2->getTypes());
    idx_t offset2 = 0;
    while (true) {
        offset2 += ht2->scan(offset2, scanChunk2);
        phtCopyForFinalize->addChunk(scanChunk2);
        if (offset2 >= ht2->getSize()) break;
    }
    phtCopyForFinalize->finalize();

    auto resultFinalized = scanAll(*phtCopyForFinalize, groupTypes, functions);
    auto resultNotFinalized = scanAll(*pht, groupTypes, functions);

    EXPECT_EQ(resultFinalized.size(), resultNotFinalized.size());
    for (auto& [key, val] : resultFinalized) {
        ASSERT_TRUE(resultNotFinalized.count(key)) << "Missing group " << key;
        EXPECT_EQ(resultNotFinalized[key], val) << "Mismatch for group " << key;
    }
}

// Test: multiple scan batches (data exceeds STANDARD_VECTOR_SIZE)
TEST_F(PartitionedAggHTTest, ScanWithAggregates_MultipleBatches) {
    vector<idx_t> groups = {0, 1};
    vector<idx_t> payloads = {2};
    auto aggFunc = SumFunc().getFunction({tLeft[payloads[0]]});
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    // Insert enough data to require multiple scan batches
    // STANDARD_VECTOR_SIZE is typically 1024, so 2000+ rows will need multiple calls
    idx_t totalRows = STANDARD_VECTOR_SIZE * 2;

    partitioned_agg_ht_ptr_t pht(new PartitionedAggHT(context, groups, payloads, functions, totalRows, 4));
    pht->setDistinct();

    // Add in batches
    for (idx_t batch = 0; batch < totalRows; batch += STANDARD_VECTOR_SIZE) {
        idx_t batchSize = minValue((idx_t)STANDARD_VECTOR_SIZE, totalRows - batch);
        DataChunk chunk = createChunkWithValue(tLeft, batchSize, batch);
        distinct_ht_ptr_t ht;
        addChunkToHT(ht, chunk, nextPowerOfTwo(batchSize * 2), false);
        DataChunk scanChunk;
        scanChunk.initialize(ht->getTypes());
        idx_t offset = 0;
        while (true) {
            offset += ht->scan(offset, scanChunk);
            pht->addChunk(scanChunk);
            if (offset >= ht->getSize()) break;
        }
    }

    // Verify scan without finalize returns all rows
    vector<LogicalType> groupTypes;
    for (auto g : groups)
        groupTypes.push_back(tLeft[g]);
    auto count = scanAllCount(*pht, groupTypes, functions);
    EXPECT_EQ(count, pht->getSize());

    // Verify no single scan call returns more than STANDARD_VECTOR_SIZE
    vector<LogicalType> aggTypes = {((AggregateFunction*)aggFunc.get())->result_};
    DataChunk grp, agg;
    grp.initialize(groupTypes);
    agg.initialize(aggTypes);

    idx_t offset = 0;
    while (true) {
        idx_t scanned = pht->scanWithAggregates(offset, grp, agg);
        if (scanned == 0) break;
        EXPECT_LE(scanned, STANDARD_VECTOR_SIZE);
        offset += scanned;
    }
}

// Test: scan with offset past all data returns 0
TEST_F(PartitionedAggHTTest, ScanWithAggregates_OffsetPastEnd) {
    vector<idx_t> groups = {0};
    vector<idx_t> payloads = {2};
    auto aggFunc = SumFunc().getFunction({tLeft[payloads[0]]});
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    DataChunk chunk = createChunkWithValue(tLeft, 8);
    distinct_ht_ptr_t ht;
    addChunkToHT(ht, chunk, 16, false);

    partitioned_agg_ht_ptr_t pht;
    addHTToPartitionedHT(pht, ht, groups, payloads, functions);

    vector<LogicalType> groupTypes = {tLeft[0]};
    vector<LogicalType> aggTypes = {((AggregateFunction*)aggFunc.get())->result_};
    DataChunk grp, agg;
    grp.initialize(groupTypes);
    agg.initialize(aggTypes);

    // Use an offset way past the data
    idx_t scanned = pht->scanWithAggregates(100000, grp, agg);
    EXPECT_EQ(scanned, 0);
}

// Test: single group (all rows have the same key)
TEST_F(PartitionedAggHTTest, ScanWithAggregates_SingleGroup) {
    vector<idx_t> groups = {0, 1};
    vector<idx_t> payloads = {2};
    auto aggFunc = SumFunc().getFunction({tLeft[payloads[0]]});
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    // All rows identical -> single group after distinct
    const idx_t N = 50;
    DataChunk chunk;
    chunk.initialize(tLeft);
    chunk.setCapacity(N);
    chunk.resize(N);
    for (idx_t i = 0; i < N; ++i) {
        for (idx_t j = 0; j < tLeft.size(); ++j) {
            chunk.setValue(j, i, Value((int64_t)(42 * j)).cast(tLeft[j].getPhysicalType()));
        }
    }
    chunk.setCardinality(N);

    distinct_ht_ptr_t ht;
    addChunkToHT(ht, chunk, 128, false);
    partitioned_agg_ht_ptr_t pht;
    addHTToPartitionedHT(pht, ht, groups, payloads, functions);

    // Should have exactly 1 group
    vector<LogicalType> groupTypes;
    for (auto g : groups)
        groupTypes.push_back(tLeft[g]);
    auto result = scanAll(*pht, groupTypes, functions);
    EXPECT_EQ(result.size(), 1);
    // SUM of col2 = 42*2 = 84 (single entry, not multiplied)
    for (auto& [key, val] : result) {
        EXPECT_EQ(val, 84);
    }
}

// Test: large cardinality without finalize spanning many partitions
TEST_F(PartitionedAggHTTest, ScanWithAggregates_LargeCardinalityWithoutFinalize) {
    vector<idx_t> groups = {0, 1};
    vector<idx_t> payloads = {2};
    auto aggFunc = SumFunc().getFunction({tLeft[payloads[0]]});
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    idx_t totalRows = 5000;

    partitioned_agg_ht_ptr_t pht(new PartitionedAggHT(context, groups, payloads, functions, totalRows, 64));
    pht->setDistinct();

    for (idx_t batch = 0; batch < totalRows; batch += STANDARD_VECTOR_SIZE) {
        idx_t batchSize = minValue((idx_t)STANDARD_VECTOR_SIZE, totalRows - batch);
        DataChunk chunk = createChunkWithValue(tLeft, batchSize, batch);
        distinct_ht_ptr_t ht;
        addChunkToHT(ht, chunk, nextPowerOfTwo(batchSize * 2), false);
        DataChunk scanChunk;
        scanChunk.initialize(ht->getTypes());
        idx_t offset = 0;
        while (true) {
            offset += ht->scan(offset, scanChunk);
            pht->addChunk(scanChunk);
            if (offset >= ht->getSize()) break;
        }
    }

    EXPECT_FALSE(pht->isReady());

    vector<LogicalType> groupTypes;
    for (auto g : groups)
        groupTypes.push_back(tLeft[g]);
    auto count = scanAllCount(*pht, groupTypes, functions);
    EXPECT_EQ(count, pht->getSize());
}

// ====== getPayloadsTypes tests ======

// Test: getPayloadsTypes returns aggregate result types, not input payload types.
// COUNT on SMALLINT input should return UBIGINT, not SMALLINT.
TEST_F(PartitionedAggHTTest, GetPayloadsTypes_ReturnsResultTypesNotInputTypes) {
    // Group by col 1 (UINTEGER), COUNT over col 0 (SMALLINT)
    vector<idx_t> groups = {1};
    vector<idx_t> payloads = {0}; // SMALLINT column
    auto countFunc = CountFunc().getFunction({tLeft[payloads[0]]}); // COUNT(SMALLINT) → UBIGINT
    vector<AggregateFunction*> functions = {(AggregateFunction*)countFunc.get()};

    DataChunk chunk = createChunkWithValue(tLeft, 8);

    distinct_ht_ptr_t ht;
    addChunkToHT(ht, chunk, 16, false);
    partitioned_agg_ht_ptr_t pht;
    addHTToPartitionedHT(pht, ht, groups, payloads, functions);

    auto resultTypes = pht->getPayloadsTypes();
    ASSERT_EQ(resultTypes.size(), 1);
    // Must be the aggregate RESULT type (UBIGINT), not input type (SMALLINT)
    EXPECT_EQ(resultTypes[0].type(), LogicalTypeId::UBIGINT);
    EXPECT_NE(resultTypes[0].type(), LogicalTypeId::SMALLINT);
}

// Test: scanWithAggregates works correctly when aggregate result type differs from input type.
// COUNT(SMALLINT) produces UBIGINT results - scan must use the right types.
TEST_F(PartitionedAggHTTest, ScanWithAggregates_CountDifferentResultType) {
    // Group by col 1 (UINTEGER), COUNT over col 0 (SMALLINT)
    vector<idx_t> groups = {1};
    vector<idx_t> payloads = {0};
    auto countFunc = CountFunc().getFunction({tLeft[payloads[0]]});
    vector<AggregateFunction*> functions = {(AggregateFunction*)countFunc.get()};

    DataChunk chunk = createChunkWithValue(tLeft, 16);

    distinct_ht_ptr_t ht;
    addChunkToHT(ht, chunk, 32, false);
    partitioned_agg_ht_ptr_t pht;
    addHTToPartitionedHT(pht, ht, groups, payloads, functions);

    pht->finalize();

    // Use getPayloadsTypes() (aggregate result types) for the scan DataChunk
    auto aggTypes = pht->getPayloadsTypes();
    ASSERT_EQ(aggTypes.size(), 1);
    EXPECT_EQ(aggTypes[0].type(), LogicalTypeId::UBIGINT);

    vector<LogicalType> groupTypes = {tLeft[1]};
    DataChunk grp, agg;
    grp.initialize(groupTypes);
    agg.initialize(aggTypes);

    idx_t totalScanned = 0;
    idx_t offset = 0;
    while (true) {
        idx_t scanned = pht->scanWithAggregates(offset, grp, agg);
        if (scanned == 0) break;
        totalScanned += scanned;
        offset += scanned;
    }
    EXPECT_EQ(totalScanned, pht->getSize());
}

// Test: scanWithAggregates without finalize using COUNT (result type != input type)
TEST_F(PartitionedAggHTTest, ScanWithAggregates_CountWithoutFinalize) {
    vector<idx_t> groups = {1};
    vector<idx_t> payloads = {0}; // SMALLINT
    auto countFunc = CountFunc().getFunction({tLeft[payloads[0]]});
    vector<AggregateFunction*> functions = {(AggregateFunction*)countFunc.get()};

    DataChunk chunk = createChunkWithValue(tLeft, 32);

    distinct_ht_ptr_t ht;
    addChunkToHT(ht, chunk, 64, false);

    partitioned_agg_ht_ptr_t pht(new PartitionedAggHT(context, groups, payloads, functions, 0, 4));
    pht->setDistinct();
    DataChunk scanChunk;
    scanChunk.initialize(ht->getTypes());
    idx_t offset = 0;
    while (true) {
        offset += ht->scan(offset, scanChunk);
        pht->addChunk(scanChunk);
        if (offset >= ht->getSize()) break;
    }

    EXPECT_FALSE(pht->isReady());

    auto aggTypes = pht->getPayloadsTypes();
    vector<LogicalType> groupTypes = {tLeft[1]};
    DataChunk grp, agg;
    grp.initialize(groupTypes);
    agg.initialize(aggTypes);

    idx_t totalScanned = 0;
    offset = 0;
    while (true) {
        idx_t scanned = pht->scanWithAggregates(offset, grp, agg);
        if (scanned == 0) break;
        totalScanned += scanned;
        offset += scanned;
    }
    EXPECT_EQ(totalScanned, pht->getSize());
}