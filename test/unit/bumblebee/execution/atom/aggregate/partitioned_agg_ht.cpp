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
#include "bumblebee/execution/AggregateChunkOneHashTable.hpp"
#include "bumblebee/function/aggregate/Sum.hpp"
#include "bumblebee/execution/PartitionedAggHT.hpp"

using namespace bumblebee;

using agg_ht_ptr = AggregateChunkOneHashTable::agg_ht_ptr_t;

class PartitionedAggHTTest : public ::testing::Test {
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
    void addChunkToHT(distinct_ht_ptr_t& ht, DataChunk &chunk, idx_t capacity = MORSEL_SIZE,
            bool resize = false) {

        if (!ht)
            ht = distinct_ht_ptr_t(new ChunkOneHashTable(tLeft, capacity, resize));

        Vector hash(UBIGINT, chunk.getSize());
        chunk.hash(hash);
        ht->addChunk(hash, chunk);
    }
    void addHTToPartitionedHT(partitioned_agg_ht_ptr_t& pht, distinct_ht_ptr_t& ht, vector<idx_t> groups,
            vector<idx_t> payloads, vector<AggregateFunction*> functions, idx_t capacity = MORSEL_SIZE,
            bool resize = false) {
        if (!pht)
            pht = partitioned_agg_ht_ptr_t(new PartitionedAggHT(groups, payloads, functions));
        pht->partitionHT(ht);
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


TEST_F(PartitionedAggHTTest, AddChunk_DuplicateGroupsAggregatesCorrectlyWithDistinc) {
    // Group by cols [1,2]; SUM over col 2 (BIGINT). We'll add the *same* rows twice.
    vector<idx_t> groupIndexTypes = {1, 2};
    vector<idx_t> payloadIndexTypes = {2};
    auto aggFunc = SumFunc::getFunction(tLeft[payloadIndexTypes[0]]);
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    // Build the chunk with 8 rows
    DataChunk chunk = createChunkWithValue(tLeft, 8 );

    // Create HT and add the same chunk twice (duplicates)
    distinct_ht_ptr_t ht1,ht2;
    addChunkToHT(ht1, chunk, 16, false);
    addChunkToHT(ht2, chunk, 16, false);

    partitioned_agg_ht_ptr_t pht;
    addHTToPartitionedHT(pht, ht1, groupIndexTypes, payloadIndexTypes, functions);
    addHTToPartitionedHT(pht, ht2, groupIndexTypes, payloadIndexTypes, functions);

    // build the partitions
    for (idx_t i = 0; i < pht->getNumPartitions(); ++i) {
        pht->aggregatePartition(i);
    }
    pht->finalize();

    agg_ht_ptr_t& aht = pht->getAggregateHT();
    // Probe with the original groups; the SUM should be doubled of payload column
    DataChunk result = probeToHT(aht, chunk, groupIndexTypes, functions);


    EXPECT_EQ(result.columnCount(), 1);
    EXPECT_EQ(result.getSize(), chunk.getSize());
    for (idx_t i = 0; i < result.getSize(); ++i) {
        // Expected =  (i * 10)
        int64_t expected = chunk.data_[payloadIndexTypes[0]].getValue(i).cast(BIGINT).getNumericValue<int64_t>();
        int64_t got = result.data_[0].getValue(i).cast(BIGINT).getNumericValue<int64_t>();
        EXPECT_EQ(got, expected) << "Row " << i << " mismatch";
    }
}



// Columns-with-same-values test
TEST_F(PartitionedAggHTTest, DistincColumns_AggregatesCorrectly1) {
    // We will group by cols [0 and 1]
    vector<idx_t> groups = {0,1};
    vector<idx_t> payloads = {2};
    auto aggFunc = SumFunc::getFunction(tLeft[payloads[0]]); // Sum over BIGINT
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    const idx_t N = 12;

    DataChunk chunk;
    chunk.initialize(tLeft);
    chunk.setCapacity(N);
    chunk.resize(N);
    for (idx_t i = 0; i < N; ++i) {
        int64_t v = static_cast<int64_t>(100);
        for (idx_t j = 0; j < tLeft.size(); ++j) {
            chunk.setValue(j, i, Value(v*j).cast(tLeft[j])); // make all columns identical
        }
    }
    chunk.setCardinality(N);

    std::cout << chunk.toString() <<std::endl;

    distinct_ht_ptr_t ht1;
    addChunkToHT(ht1, chunk, 32, false);
    partitioned_agg_ht_ptr_t pht;
    addHTToPartitionedHT(pht, ht1, groups, payloads, functions);
    // build the partitions and finalize
    for (idx_t i = 0; i < pht->getNumPartitions(); ++i) {
        pht->aggregatePartition(i);
    }
    pht->finalize();


    agg_ht_ptr_t& aht = pht->getAggregateHT();
    // Probe with the original groups; the SUM should be doubled of payload column
    DataChunk result = probeToHT(aht, chunk, groups, functions);

    ASSERT_EQ(result.getSize(), chunk.getSize());
    // values should be 200 as all the values are duplicates
    for (idx_t i = 0; i < result.getSize(); ++i) {
        int64_t expected = 200;
        int64_t got = result.data_[0].getValue(i).cast(BIGINT).getNumericValue<int64_t>();
        EXPECT_EQ(got, expected) << "Row " << i << " mismatch";
    }

}



TEST_F(PartitionedAggHTTest, Partition_Shift62) {
    vector<idx_t> groupIndexTypes = {0,2};
    vector<idx_t> payloadIndexTypes = {2};
    auto aggFunc = SumFunc::getFunction(tLeft[payloadIndexTypes[0]]);
    vector<AggregateFunction*> functions = {(AggregateFunction*)aggFunc.get()};

    DataChunk chunk = createChunkWithValue(tLeft, 16);
    distinct_ht_ptr_t ht;
    addChunkToHT(ht, chunk, 32, false);

    EXPECT_EQ(ht->getSize(), 16);

    // Prepare 2 partitions; with shift=63, index should always be 1 for non-empty buckets.
    std::vector<distinct_ht_ptr_t> partitions(4);
    ht->partition(partitions, 62);
    auto pSize = 0;
    for (auto& p: partitions)
        if (p)pSize += p->getSize();
    EXPECT_EQ(pSize, ht->getSize());

    // now combine all the partitions
    distinct_ht_ptr_t htFinal;
    for (auto& p: partitions) {
        if (!p)continue;
        if (!htFinal) {
            htFinal = std::move(p);
            continue;
        };
        htFinal->combine(*p);
        p = nullptr;
    }
    EXPECT_EQ(htFinal->getSize(), ht->getSize());
}