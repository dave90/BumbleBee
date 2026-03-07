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

#include "../../BumbleBaseTest.hpp"
#include "bumblebee/ClientContext.hpp"
#include "bumblebee/common/vector_operations/CreateSortKey.hpp"

using namespace bumblebee;


class SortKeysTest : public BumbleBaseTest {
protected:


    void createSortKeysAndCheckResult(vector<vector<Value>> &data, DataChunk& input, vector<OrderModifiers>& modifiers ) {

        Vector result(PhysicalType::STRING);
        CreateSortKey::createSortKey(input, modifiers, result);

        // store the sort keys in a vector
        vector<string> keyString;
        auto dataPtr = FlatVector::getData<string_t>(result);
        for (idx_t i=0; i<input.getSize(); ++i) {
            auto s = dataPtr[i].getString();
            keyString.push_back(s);
        }
        // convert the data from column store to row store
        vector<vector<Value>> tableData;
        for (idx_t row = 0 ; row < data[0].size(); ++row) {
            vector<Value> rowData;
            for (idx_t col = 0; col < data.size(); ++col) {
                rowData.push_back(std::move(data[col][row]));
            }
            tableData.push_back(std::move(rowData));
        }

        // sort the value data and store the index -> sorted index
        vector<idx_t> idxs(tableData.size());
        std::iota(idxs.begin(), idxs.end(), 0);
        RowComparator cmp{modifiers, tableData};
        std::sort(idxs.begin(), idxs.end(), cmp);
        // now put in a map index -> sort index
        std::unordered_map<idx_t, idx_t> orderedPosition;
        for (idx_t i = 0; i < idxs.size(); ++i) {
            orderedPosition[idxs[i]] = i;
        }

        // store the index for each sort key
        std::unordered_map<string, idx_t> indexBeforeSort;
        for (idx_t i = 0; i < keyString.size(); i++) {
            if (!indexBeforeSort.contains(keyString[i]))
                indexBeforeSort[keyString[i]] = i;
        }
        // sort the sort keys and compare the sort index with the previous sort index
        std::sort(keyString.begin(), keyString.end());
        std::unordered_map<string, idx_t> actualOrder;
        for (idx_t i = 0; i < keyString.size(); i++) {
            auto orig_idx = indexBeforeSort[keyString[i]];
            if (i != orderedPosition[orig_idx]) {
                if (keyString[i] == keyString[orderedPosition[orig_idx]])
                    continue; // continue as they are same strings
                std::cout << "Key index "<<orig_idx<<" is now in position "<<i<<" but expected in "<<orderedPosition[orig_idx]<<std::endl;
                for (idx_t j = 0; j < keyString.size(); j++) {
                    std::cout << j << " : ";
                    for (auto c:keyString[j]) {
                        std::cout << std::to_string((uint8_t)c) << " - ";
                    }
                    std::cout << std::endl;
                }
            }
            EXPECT_EQ(i, orderedPosition[orig_idx]);
        }

    }
};


TEST_F(SortKeysTest, CreateSortKeysOneColAscTest) {
    vector<vector<Value>> data;
    addData(data, vector<int>{0,10,20,30});
    vector<LogicalType> types = {LogicalTypeId::UINTEGER};
    DataChunk chunk = generateDataChunk(types, data);

    vector<OrderModifiers> modifiers = {OrderType::ASCENDING};
    createSortKeysAndCheckResult(data, chunk, modifiers);
}

TEST_F(SortKeysTest, CreateSortKeysOneColAscIntTest) {
    vector<vector<Value>> data;
    addData(data, vector<int>{0,10,20,30});
    vector<LogicalType> types = {LogicalTypeId::BIGINT};
    DataChunk chunk = generateDataChunk(types, data);

    vector<OrderModifiers> modifiers = {OrderType::ASCENDING};
    createSortKeysAndCheckResult(data, chunk, modifiers);
}

TEST_F(SortKeysTest, CreateSortKeysOneColDescIntTest) {
    vector<vector<Value>> data;
    addData(data, vector<int>{0,10,20,30});
    vector<LogicalType> types = {LogicalTypeId::SMALLINT};
    DataChunk chunk = generateDataChunk(types, data);

    vector<OrderModifiers> modifiers = {OrderType::DESCENDING};
    createSortKeysAndCheckResult(data, chunk, modifiers);
}

TEST_F(SortKeysTest, CreateSortKeysTwoColsDoubleDescIntAscTest) {
    vector<vector<Value>> data;
    addData(data, vector<double>{1.5, -2.0, 3.1415, 0.0});
    addData(data, vector<int>{10, 5, 20, 15});
    vector<LogicalType> types = {LogicalTypeId::DOUBLE, LogicalTypeId::INTEGER};
    DataChunk chunk = generateDataChunk(types, data);

    vector<OrderModifiers> modifiers = {OrderType::DESCENDING, OrderType::ASCENDING};
    createSortKeysAndCheckResult(data, chunk, modifiers);
}

TEST_F(SortKeysTest, CreateSortKeysThreeColsFloatUintStrMixedTest) {
    vector<vector<Value>> data;
    addData(data, vector<float>{1.0f, 2.0f, 1.0f, 3.5f, 2.0f, 1.0f});
    addData(data, vector<unsigned int>{2, 1, 2, 0, 1, 2});
    addData(data, vector<string>{"b", "a", "a", "c", "b", "a"});
    vector<LogicalType> types = {LogicalTypeId::FLOAT, LogicalTypeId::UINTEGER, LogicalTypeId::STRING};
    DataChunk chunk = generateDataChunk(types, data);

    vector<OrderModifiers> modifiers = {OrderType::ASCENDING, OrderType::DESCENDING, OrderType::ASCENDING};
    createSortKeysAndCheckResult(data, chunk, modifiers);
}

TEST_F(SortKeysTest, CreateSortKeysFiveColsMixedTypesTest) {
    vector<vector<Value>> data;
    addData(data, vector<int64_t>{10, 10, 5, 10, 5}); // BIGINT
    addData(data, vector<short>{1, 0, 2, 1, 2}); // SMALLINT
    addData(data, vector<double>{0.1, -1.0, 0.1, 2.0, 0.1});
    addData(data, vector<string>{"x", "x", "a", "y", "a"});
    addData(data, vector<unsigned int>{1, 2, 1, 0, 1}); // UINTEGER
    vector<LogicalType> types = {LogicalTypeId::BIGINT, LogicalTypeId::SMALLINT,
        LogicalTypeId::DOUBLE, LogicalTypeId::STRING, LogicalTypeId::UINTEGER};
    DataChunk chunk = generateDataChunk(types, data);

    vector<OrderModifiers> modifiers = {OrderType::ASCENDING, OrderType::DESCENDING, OrderType::ASCENDING, OrderType::DESCENDING, OrderType::ASCENDING};
    createSortKeysAndCheckResult(data, chunk, modifiers);
}

TEST_F(SortKeysTest, CreateSortKeysTwoColsStringDescUintAscTest) {
    vector<vector<Value>> data;
    addData(data, vector<string>{"apple", "banana", "apple", "zebra", "banana"});
    addData(data, vector<unsigned int>{5, 3, 5, 1, 4});
    vector<LogicalType> types = {LogicalTypeId::STRING, LogicalTypeId::UINTEGER};
    DataChunk chunk = generateDataChunk(types, data);

    vector<OrderModifiers> modifiers = {OrderType::DESCENDING, OrderType::ASCENDING};
    createSortKeysAndCheckResult(data, chunk, modifiers);
}

TEST_F(SortKeysTest, CreateSortKeysThreeColsMixedOrderTypesTest) {
    vector<vector<Value>> data;
    addData(data, vector<float>{0.0f, -0.0f, 1.5f, 1.5f, 2.0f, -1.0f, 1.5f});
    addData(data, vector<double>{0.0, 0.0, 1.5, 1.4, 2.0, -1.0, 1.5});
    addData(data, vector<int>{0, 1, 2, 1, 3, 0, 2});
    vector<LogicalType> types = {LogicalTypeId::FLOAT, LogicalTypeId::DOUBLE, LogicalTypeId::INTEGER};
    DataChunk chunk = generateDataChunk(types, data);

    vector<OrderModifiers> modifiers = {OrderType::DESCENDING, OrderType::ASCENDING, OrderType::ASCENDING};
    createSortKeysAndCheckResult(data, chunk, modifiers);
}

TEST_F(SortKeysTest, CreateSortKeysLongStringAscIntDescTest) {
    vector<vector<Value>> data;
    addData(data, vector<string>{
        "this_is_a_long_string_1",
        "another_very_long_string_2",
        "zebra_long_string_3",
        "middle_length_string_4",
        "0_middle_length_string_4"
    });
    addData(data, vector<int>{42, 7, 100, 55,55});
    vector<LogicalType> types = {PhysicalType::STRING, PhysicalType::INTEGER};
    DataChunk chunk = generateDataChunk(types, data);

    vector<OrderModifiers> modifiers = {OrderType::ASCENDING, OrderType::DESCENDING};
    createSortKeysAndCheckResult(data, chunk, modifiers);
}

TEST_F(SortKeysTest, CreateSortKeysLongStringDescUintAscTest) {
    vector<vector<Value>> data;
    addData(data, vector<string>{
        "long_string_alpha_abcdefghij",
        "long_string_beta_klmnopqrst",
        "long_string_gamma_uvwxyzabc",
        "1_long_string_delta_defghijkl"
    });
    addData(data, vector<unsigned int>{10, 5, 20, 15});
    vector<LogicalType> types = {LogicalTypeId::STRING, LogicalTypeId::UINTEGER};
    DataChunk chunk = generateDataChunk(types, data);

    vector<OrderModifiers> modifiers = {OrderType::DESCENDING, OrderType::ASCENDING};
    createSortKeysAndCheckResult(data, chunk, modifiers);
}


TEST_F(SortKeysTest, CreateSortKeysLongUInteger) {
    vector<vector<Value>> data;
    addData(data, vector<uint32_t>{
        4293053207,4293668640,4294747978,
        4279807690,4293053207,4286980034
    });
    vector<LogicalType> types = {LogicalTypeId::UINTEGER};
    DataChunk chunk = generateDataChunk(types, data);

    vector<OrderModifiers> modifiers = {OrderType::DESCENDING};
    createSortKeysAndCheckResult(data, chunk, modifiers);
}
