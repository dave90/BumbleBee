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

    void topNChunksTest(idx_t randomChunks, const vector<ConstantType>& types,const vector<ColModifier>& modifiers, idx_t limit) {
        DataChunk chunkone;
        chunkone.initialize(types);
        TopNHeap topn(types, modifiers, limit);

        for (idx_t i = 0; i < randomChunks; ++i) {
            DataChunk chunk = generateRandomDataChunk(types);
            topn.sink(chunk);
            chunkone.append(chunk, true);
        }
        DataChunk result;
        result.initializeEmpty(types);
        topn.getData(result);

        vector<vector<Value>> tableData;
        std::unordered_set<id_t> sortCols(topn.getSortCols().begin(), topn.getSortCols().end());

        for (idx_t row=0; row < chunkone.getSize();++row) {
            vector<Value> rowData;
            for (idx_t col=0; col < chunkone.columnCount();++col) {
                if (!sortCols.contains(col)) continue;
                auto val = chunkone.getValue(col, row);
                rowData.push_back(std::move(val));
            }
            tableData.push_back(std::move(rowData));
        }
        DataChunk expectedResult;
        expectedResult.initialize(types);
        vector<idx_t> idxs(tableData.size());
        std::iota(idxs.begin(), idxs.end(), 0);
        RowComparator cmp{topn.getModifiers(), tableData};
        std::sort(idxs.begin(), idxs.end(), cmp);
        for (idx_t i=0;i<minValue(limit,chunkone.getSize());++i) {
            for (idx_t col=0; col < chunkone.columnCount();++col) {
                auto val = chunkone.getValue(col, idxs[i]);
                expectedResult.setValue(col, i, val);
            }
            expectedResult.setCardinality(i+1);
        }
        compareChunks(expectedResult, result, topn.getSortCols());
    }

};

TEST_F(TopNHeapTest, CreateTop10neColNumeric) {

    topNChunksTest(1, {UINTEGER, INTEGER, FLOAT, DOUBLE}, {
        {.col_ = 1, .modifier_ = OrderType::ASCENDING}
    }, 10);
}

TEST_F(TopNHeapTest, CreateTop10TwoColNumeric) {

    topNChunksTest(1, {UINTEGER, INTEGER, FLOAT, DOUBLE}, {
        {.col_ = 1, .modifier_ = OrderType::ASCENDING},
        {.col_ = 3, .modifier_ = OrderType::DESCENDING}
    }, 10);
}

TEST_F(TopNHeapTest, TopNMultipleChunksMixedTypes) {
    topNChunksTest(2, {UINTEGER, STRING, DOUBLE}, {
        {.col_ = 0, .modifier_ = OrderType::DESCENDING},
    }, 3);
}

TEST_F(TopNHeapTest, TopNStringAndNumericAllDescending) {
    topNChunksTest(3, {STRING, INTEGER, FLOAT}, {
        {.col_ = 0, .modifier_ = OrderType::DESCENDING},
        {.col_ = 1, .modifier_ = OrderType::DESCENDING},
        {.col_ = 2, .modifier_ = OrderType::DESCENDING}
    }, 5);
}

TEST_F(TopNHeapTest, TopNLimitGreaterThanRows) {
    topNChunksTest(2, {INTEGER, STRING}, {
        {.col_ = 1, .modifier_ = OrderType::ASCENDING}
    }, 100); // limit > total rows
}

TEST_F(TopNHeapTest, TopNAllAscendingMixedTypes) {
    topNChunksTest(4, {STRING, DOUBLE, UINTEGER}, {
        {.col_ = 0, .modifier_ = OrderType::ASCENDING},
        {.col_ = 1, .modifier_ = OrderType::ASCENDING},
        {.col_ = 2, .modifier_ = OrderType::ASCENDING}
    }, 8);
}

TEST_F(TopNHeapTest, TopNComplexModifiers) {
    topNChunksTest(6, {FLOAT, STRING, INTEGER, DOUBLE}, {
        {.col_ = 2, .modifier_ = OrderType::DESCENDING},
        {.col_ = 1, .modifier_ = OrderType::ASCENDING},
        {.col_ = 3, .modifier_ = OrderType::DESCENDING}
    }, 3);
}

TEST_F(TopNHeapTest, TopNAllStringColumns) {
    topNChunksTest(4, {STRING, STRING, STRING}, {
        {.col_ = 0, .modifier_ = OrderType::ASCENDING},
        {.col_ = 1, .modifier_ = OrderType::DESCENDING},
        {.col_ = 2, .modifier_ = OrderType::ASCENDING}
    }, 6);
}