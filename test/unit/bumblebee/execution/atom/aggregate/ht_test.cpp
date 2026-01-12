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

#include "../../../BumbleBaseTest.hpp"
#include "bumblebee/ClientContext.hpp"
#include "bumblebee/catalog/PredicateTables.hpp"
#include "bumblebee/function/aggregate/Sum.hpp"
#include "bumblebee/execution/PRLHashTable.hpp"

using namespace bumblebee;



class ProbeRLHTTest : public BumbleBaseTest {
    // Creates and returns a DataChunk initialized with a predefined set of column types: INTEGER, UINTEGER, and BIGINT.
    // For each of the count rows, it populates the columns with incrementing values:
    // Column 0 (INTEGER): sequential int32_t values starting from 0.
    // Column 1 (UINTEGER): uint32_t values equal to i * 10.
    // Column 2 (BIGINT): int64_t values equal to i * 100.
    // The function sets the cardinality of the chunk to count and returns it.
    // This utility is primarily used for generating consistent and type-diverse data for testing the ChunkCollection class.
protected:

    vector<PhysicalType> tLeft{PhysicalType::SMALLINT, PhysicalType::UINTEGER, PhysicalType::BIGINT};

    void addChunkToHT(std::unique_ptr<PRLHashTable>& ht, DataChunk &chunk,
            idx_t capacity = STANDARD_VECTOR_SIZE, bool resize = false) {

        if (!ht)
            ht = std::make_unique<PRLHashTable>(*clientContext.bufferManager_,chunk.getTypes(), capacity, resize);

        Vector hash(LogicalTypeId::HASH,chunk.getSize() );
        chunk.hash(hash);
        ht->addChunk(hash, chunk);
    }


    DataChunk scanHT(std::unique_ptr<PRLHashTable>& ht, idx_t offset) {
        DataChunk group;
        group.initialize(ht->getTypes());

        ht->scan(offset, group);

        return group;
    }

};

TEST_F(ProbeRLHTTest, HTSimpleTest) {
    vector<vector<Value>> data;
    addData(data, vector<int>{0,0,0,0});
    addData(data, vector<int>{0,10,20,30});
    vector<LogicalType> types = {LogicalTypeId::INTEGER, LogicalTypeId::UINTEGER};
    DataChunk chunk = generateDataChunk(types, data);


    std::unique_ptr<PRLHashTable> ht;
    addChunkToHT(ht, chunk, 32);

    DataChunk result = scanHT(ht,0);
    compareChunks(result, chunk);
}


TEST_F(ProbeRLHTTest, HTTestWithDuplicates) {
    vector<vector<Value>> data;
    addData(data, vector<int>{0,0,0,0});
    addData(data, vector<int>{0,10,10,10});
    vector<LogicalType> types = {LogicalTypeId::INTEGER, LogicalTypeId::UINTEGER};
    DataChunk chunk = generateDataChunk(types, data);


    std::unique_ptr<PRLHashTable> ht;
    addChunkToHT(ht, chunk, 32);

    DataChunk result = scanHT(ht,0);
    vector<vector<Value>> expectedData;
    addData(expectedData, vector<int>{0,0});
    addData(expectedData, vector<int>{0,10});
    DataChunk expectedChunk = generateDataChunk(types, expectedData);
    compareChunks(result, expectedChunk);
}

TEST_F(ProbeRLHTTest, HTTestWithDuplicatesMultiAdd) {
    std::unique_ptr<PRLHashTable> ht;
    idx_t iterations = 3;
    idx_t size = STANDARD_VECTOR_SIZE;
    vector<LogicalType> types = {LogicalTypeId::UTINYINT, LogicalTypeId::UBIGINT, LogicalTypeId::BIGINT};
    DataChunk allData;
    allData.initialize(types);
    for (idx_t i = 1; i < iterations; i++) {
        vector<vector<Value>> data;
        addData(data, std::vector<int>(size, 0));
        int start = (int)size * (int)i, end = start + (int)size;
        addData(data, geenrateSequence(start,end, 1 ));
        start = start*-10;end = start +(int)size;
        addData(data, geenrateSequence(start,end, 1 ));
        DataChunk chunk = generateDataChunk(types, data);
        addChunkToHT(ht, chunk, size, true);
        allData.append(chunk, true);
    }

    EXPECT_EQ(ht->getSize(), allData.getSize());
}


TEST_F(ProbeRLHTTest, HTCombineTest) {
    std::unique_ptr<PRLHashTable> ht1;
    std::unique_ptr<PRLHashTable> ht2;
    idx_t size = 256;
    vector<LogicalType> types = {LogicalTypeId::UTINYINT, LogicalTypeId::UBIGINT};
    // generate same chunks
    vector<vector<Value>> data;
    addData(data, std::vector<int>(size, 0));
    int start = (int)0, end = start + (int)size;
    addData(data, geenrateSequence(start,end, 1 ));
    DataChunk chunk = generateDataChunk(types, data);
    addChunkToHT(ht1, chunk, size, true);
    addChunkToHT(ht2, chunk, size, true);
    ht1->combine(*ht2);

    // combine 2 ht with same data
    EXPECT_EQ(chunk.getSize(), ht1->getSize());
    DataChunk result = scanHT(ht1, 0);
    compareChunks(result, chunk);
}

TEST_F(ProbeRLHTTest, HTpartitionTest) {
    std::unique_ptr<PRLHashTable> ht;
    idx_t size = STANDARD_VECTOR_SIZE;
    vector<LogicalType> types = {LogicalTypeId::UTINYINT, LogicalTypeId::UBIGINT};
    // generate same chunks
    vector<vector<Value>> data;
    addData(data, std::vector<int>(size, 0));
    int start = (int)0, end = start + (int)size;
    addData(data, geenrateSequence(start,end, 1 ));
    DataChunk chunk = generateDataChunk(types, data);
    addChunkToHT(ht, chunk, size, true);

    vector<distinct_ht_ptr_t> partitions;
    partitions.resize(8);
    ht->partition(partitions, 64-3);
}

