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
#include "bumblebee/execution/PRLHashTable.hpp"

using namespace bumblebee;



class ProbeRLHTTest : public ::testing::Test {
    // Creates and returns a DataChunk initialized with a predefined set of column types: INTEGER, UINTEGER, and BIGINT.
    // For each of the count rows, it populates the columns with incrementing values:
    // Column 0 (INTEGER): sequential int32_t values starting from 0.
    // Column 1 (UINTEGER): uint32_t values equal to i * 10.
    // Column 2 (BIGINT): int64_t values equal to i * 100.
    // The function sets the cardinality of the chunk to count and returns it.
    // This utility is primarily used for generating consistent and type-diverse data for testing the ChunkCollection class.
protected:

    vector<ConstantType> tLeft{ConstantType::SMALLINT, ConstantType::UINTEGER, ConstantType::BIGINT};
    ClientContext clientContext;


    Vector generateVector(ConstantType type, vector<Value>& values ) {
        Vector v1(type,values.size());
        for (idx_t i = 0; i < values.size(); i++) {
            v1.setValue(i, values[i].cast(v1.getType()));
        }
        return v1;
    }

    DataChunk generateDataChunk(vector<ConstantType>& types, vector<vector<Value>>& data) {
        BB_ASSERT(types.size() == data.size());
        DataChunk chunk;
        chunk.initializeEmpty(types);
        idx_t idx = 0;
        for (auto& data_col : data) {
            Vector vec = generateVector(types[idx], data_col);
            chunk.data_[idx++].reference(vec);
        }
        chunk.setCapacity(data[0].size());
        chunk.setCardinality(data[0].size());
        return chunk;
    }

    template <class T>
    void addData(vector<vector<Value>>& table, vector<T> data) {
        vector<Value> col_data;
        for (auto& d : data) {
            col_data.push_back(d);
        }
        table.push_back(std::move(col_data));
    }

    vector<int> geenrateSequence(int start, int end, int step=1) {
        vector<int> result;
        for (int i = start; i <= end; i += step) {
            result.push_back(i);
        }
        return result;
    }

    void addChunkToHT(std::unique_ptr<PRLHashTable>& ht, DataChunk &chunk,
            idx_t capacity = STANDARD_VECTOR_SIZE, bool resize = false) {

        if (!ht)
            ht = std::make_unique<PRLHashTable>(*clientContext.bufferManager_,chunk.getTypes(), capacity, resize);

        Vector hash(UBIGINT,chunk.getSize() );
        chunk.hash(hash);
        ht->addChunk(hash, chunk);
    }


    DataChunk scanHT(std::unique_ptr<PRLHashTable>& ht, idx_t offset) {
        DataChunk group;
        group.initialize(ht->getTypes());

        ht->scan(offset, group);

        return group;
    }

    void compareChunks(DataChunk &chunk1, DataChunk &chunk2) {
        EXPECT_EQ(chunk1.columnCount(), chunk2.columnCount());
        EXPECT_EQ(chunk1.getSize(), chunk2.getSize());
        std::unordered_set<string> chunk1Str, chunk2Str;
        // compare the chunks
        for (idx_t i = 0; i < chunk1.getSize(); i++) {
            string row1, row2;
            for (idx_t j = 0; j < chunk1.columnCount(); j++) {
                row1 += chunk1.getValue(j, i).toString() + " ; ";
                row2 += chunk2.getValue(j, i).toString()+ " ; ";;
            }
            chunk1Str.insert(row1);
            chunk2Str.insert(row2);
        }
        EXPECT_EQ(chunk1Str.size(), chunk2Str.size());
        EXPECT_EQ(chunk1Str, chunk2Str);
    }

};

TEST_F(ProbeRLHTTest, HTSimpleTest) {
    vector<vector<Value>> data;
    addData(data, vector<int>{0,0,0,0});
    addData(data, vector<int>{0,10,20,30});
    vector<ConstantType> types = {INTEGER, UINTEGER};
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
    vector<ConstantType> types = {INTEGER, UINTEGER};
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
    vector<ConstantType> types = {UTINYINT, UBIGINT, BIGINT};
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
    vector<ConstantType> types = {UTINYINT, UBIGINT};
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


