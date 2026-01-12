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
#include <gtest/gtest.h>
#include <algorithm>
#include <numeric>

#include "../BumbleBaseTest.hpp"
#include "bumblebee/ClientContext.hpp"
#include "bumblebee/common/vector_operations/CreateSortKey.hpp"
#include "bumblebee/execution/TopNHeap.hpp"

using namespace bumblebee;


class TopNHeapTest : public BumbleBaseTest {
protected:

    vector<data_chunk_ptr_t> generateDataChunks(idx_t randomChunks, const vector<LogicalType>& types) {
        vector<data_chunk_ptr_t> result;
        for (idx_t i = 0; i < randomChunks; ++i) {
            DataChunk chunk = generateRandomDataChunk(types);
            chunk.setCardinality(10);
            auto ptr = chunk.clone();
            result.push_back(std::move(ptr));
        }
        return result;
    }

    DataChunk sinkTopNHeap(TopNHeap& topn, vector<data_chunk_ptr_t>& chunks, bool finalize = true) {
        for (auto& chunk : chunks) {
            topn.sink(*chunk);
        }

        if (finalize)
            topn.finalize();

        DataChunk result;
        result.initializeEmpty(topn.getPayloadTypes());
        topn.getData(result);
        return result;
    }

    void compareResult(vector<data_chunk_ptr_t>& chunks, DataChunk& result, TopNHeap& topn) {
        DataChunk chunkone;
        chunkone.initialize(chunks[0]->getTypes());
        for (auto& chunk : chunks) {
            chunkone.append(*chunk, true);
        }

        vector<vector<Value>> tableData;
        auto sortCols = topn.getSortCols();
        for (idx_t row=0; row < chunkone.getSize();++row) {
            vector<Value> rowData;
            for (idx_t col_idx=0; col_idx < sortCols.size();++col_idx) {
                auto col = sortCols[col_idx];
                auto val = chunkone.getValue(col, row);
                rowData.push_back(std::move(val));
            }
            tableData.push_back(std::move(rowData));
        }

        DataChunk expectedResult;
        expectedResult.initialize(chunkone.getTypes());
        vector<idx_t> idxs(tableData.size());
        std::iota(idxs.begin(), idxs.end(), 0);
        RowComparator cmp{topn.getModifiers(), tableData};
        std::sort(idxs.begin(), idxs.end(), cmp);
        for (idx_t i=0;i<minValue(topn.getHeapSize(),chunkone.getSize());++i) {
            for (idx_t col=0; col < chunkone.columnCount();++col) {
                auto val = chunkone.getValue(col, idxs[i]);
                expectedResult.setValue(col, i, val);
            }
            expectedResult.setCardinality(i+1);
        }
        compareChunks(expectedResult, result, topn.getSortCols());
    }

    void topNChunksTest(idx_t randomChunks, const vector<LogicalType>& types,const vector<ColModifier>& modifiers, idx_t limit) {
        auto chunks = generateDataChunks(randomChunks, types);

        TopNHeap topn(types, modifiers, limit);
        auto result = sinkTopNHeap(topn, chunks);

        compareResult(chunks, result, topn);
    }

};

TEST_F(TopNHeapTest, CreateTop10neColNumeric) {

    topNChunksTest(1, {PhysicalType::UINTEGER, PhysicalType::INTEGER, PhysicalType::FLOAT, PhysicalType::DOUBLE}, {
        {.col_ = 1, .modifier_ = OrderType::ASCENDING}
    }, 10);
}

TEST_F(TopNHeapTest, CreateTop10TwoColNumeric) {

    topNChunksTest(1, {PhysicalType::UINTEGER, PhysicalType::INTEGER, PhysicalType::FLOAT, PhysicalType::DOUBLE}, {
        {.col_ = 1, .modifier_ = OrderType::ASCENDING},
        {.col_ = 3, .modifier_ = OrderType::DESCENDING}
    }, 10);
}

TEST_F(TopNHeapTest, TopNMultipleChunksMixedTypes) {
    topNChunksTest(2, {PhysicalType::UINTEGER, PhysicalType::STRING, PhysicalType::DOUBLE}, {
        {.col_ = 0, .modifier_ = OrderType::DESCENDING},
    }, 3);
}

TEST_F(TopNHeapTest, TopNStringAndNumericAllDescending) {
    topNChunksTest(3, {PhysicalType::STRING, PhysicalType::INTEGER, PhysicalType::FLOAT}, {
        {.col_ = 0, .modifier_ = OrderType::DESCENDING},
        {.col_ = 1, .modifier_ = OrderType::DESCENDING},
        {.col_ = 2, .modifier_ = OrderType::DESCENDING}
    }, 5);
}

TEST_F(TopNHeapTest, TopNLimitGreaterThanRows) {
    topNChunksTest(2, {PhysicalType::INTEGER, PhysicalType::STRING}, {
        {.col_ = 1, .modifier_ = OrderType::ASCENDING}
    }, 100); // limit > total rows
}

TEST_F(TopNHeapTest, TopNAllAscendingMixedTypes) {
    topNChunksTest(4, {PhysicalType::STRING, PhysicalType::DOUBLE, PhysicalType::UINTEGER}, {
        {.col_ = 0, .modifier_ = OrderType::ASCENDING},
        {.col_ = 1, .modifier_ = OrderType::ASCENDING},
        {.col_ = 2, .modifier_ = OrderType::ASCENDING}
    }, 8);
}

TEST_F(TopNHeapTest, TopNComplexModifiers) {
    topNChunksTest(6, {PhysicalType::FLOAT, PhysicalType::STRING, PhysicalType::INTEGER, PhysicalType::DOUBLE}, {
        {.col_ = 2, .modifier_ = OrderType::DESCENDING},
        {.col_ = 1, .modifier_ = OrderType::ASCENDING},
        {.col_ = 3, .modifier_ = OrderType::DESCENDING}
    }, 3);
}

TEST_F(TopNHeapTest, TopNAllStringColumns) {
    topNChunksTest(4, {PhysicalType::STRING, PhysicalType::STRING, PhysicalType::STRING}, {
        {.col_ = 0, .modifier_ = OrderType::ASCENDING},
        {.col_ = 1, .modifier_ = OrderType::DESCENDING},
        {.col_ = 2, .modifier_ = OrderType::ASCENDING}
    }, 6);
}

TEST_F(TopNHeapTest, TopNComplexModifiersCombineSameChunks) {
    auto randomChunks = 3;
    vector<LogicalType> types = {LogicalTypeId::FLOAT, LogicalTypeId::STRING, LogicalTypeId::INTEGER, LogicalTypeId::DOUBLE};
    vector<ColModifier> modifiers ={
        {.col_ = 2, .modifier_ = OrderType::DESCENDING},
        {.col_ = 1, .modifier_ = OrderType::ASCENDING},
        {.col_ = 3, .modifier_ = OrderType::DESCENDING}
    };
    auto limit = 100;

    auto chunks1 = generateDataChunks(randomChunks, types);
    vector<data_chunk_ptr_t> chunks2;
    for (idx_t i = 0; i < chunks1.size(); i++) {
        chunks2.push_back(chunks1[i]->clone());
    }

    TopNHeap topn1(types, modifiers, limit);
    sinkTopNHeap(topn1, chunks1, false);
    TopNHeap topn2(types, modifiers, limit);
    sinkTopNHeap(topn2, chunks2,false);
    topn2.finalize();
    topn1.combine(topn2);


    DataChunk result;
    result.initializeEmpty(topn1.getPayloadTypes());
    topn1.getData(result);

    vector<data_chunk_ptr_t> chunks(std::move(chunks1));
    for (auto& chunk : chunks2) {
        chunks.push_back(std::move(chunk));
    }
    compareResult(chunks, result, topn1);
}


TEST_F(TopNHeapTest, TopNComplexModifiersCombineDifferentChunks) {
    auto randomChunks = 10;
    vector<LogicalType> types = {LogicalTypeId::STRING, LogicalTypeId::DOUBLE, LogicalTypeId::UINTEGER};
    vector<ColModifier> modifiers ={
        {.col_ = 2, .modifier_ = OrderType::DESCENDING},
        {.col_ = 1, .modifier_ = OrderType::ASCENDING},
    };
    auto limit = 800;

    auto chunks1 = generateDataChunks(randomChunks, types);
    vector<data_chunk_ptr_t> chunks2 = generateDataChunks(randomChunks, types);

    TopNHeap topn1(types, modifiers, limit);
    sinkTopNHeap(topn1, chunks1, false);
    TopNHeap topn2(types, modifiers, limit);
    sinkTopNHeap(topn2, chunks2,false);
    topn2.finalize();
    topn1.combine(topn2);


    DataChunk result;
    result.initializeEmpty(topn1.getPayloadTypes());
    topn1.getData(result);

    vector<data_chunk_ptr_t> chunks(std::move(chunks1));
    for (auto& chunk : chunks2) {
        chunks.push_back(std::move(chunk));
    }
    compareResult(chunks, result, topn1);
}