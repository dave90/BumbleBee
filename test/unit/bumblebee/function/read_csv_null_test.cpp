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

#include <filesystem>
#include <gtest/gtest.h>

#include "bumblebee/common/TypeDefs.hpp"
#include "bumblebee/common/BufferedCSVReader.hpp"
#include "bumblebee/function/predicate/ReadCsv.hpp"

using namespace bumblebee;

// An empty CSV field (the default null marker) reads back as NULL, in both the
// STRING (reference) path and the cast (numeric) path. A non-empty field never
// reads as NULL and keeps its exact value.
class ReadCSVNullTest : public ::testing::Test {
protected:
    ClientContext context;

    string getCsvFilePath(const string& filename) {
        std::filesystem::path base = __FILE__;
        return (base.parent_path() / "data" / "input" / "csv" / filename).string();
    }
};

TEST_F(ReadCSVNullTest, EmptyFieldsBecomeNull) {
    // 4 rows, cols A:INTEGER B:STRING C:DOUBLE; empties at A[1], B[2], C[3].
    DataChunk chunk;
    {
        vector<Value> input;
        input.emplace_back(getCsvFilePath("null_basic.csv"));
        vector<LogicalType> inputTypes = {LogicalTypeId::STRING};
        TableFilterSet filters;
        auto functionPtr = ReadCsvFunc().getFunction({});
        auto& pred = (PredFunction&)*functionPtr;
        std::unordered_map<string, Value> params;
        params.emplace("auto_detect", uint8_t(0));
        params.emplace("header", uint8_t(0));
        params.emplace("columns", string("A:INTEGER;B:STRING;C:DOUBLE"));
        vector<LogicalType> returnTypes;
        std::unordered_map<string, idx_t> bindVarName;
        vector<string> names = {"A", "B", "C"};
        auto bind = pred.bindFunction_(context, input, inputTypes, params,
                                       bindVarName, returnTypes, names, filters);
        pred.maxThreadFunction_(context, bind.get());  // populates the file task list
        auto fopd = pred.initFunction_(context, bind.get());
        chunk.initialize(returnTypes);
        chunk.setCardinality(0);
        pred.function_(context, bind.get(), fopd.get(), nullptr, chunk);
    }

    ASSERT_EQ(chunk.getSize(), 4u);

    // NULLs land exactly where the fields were empty.
    EXPECT_TRUE(chunk.getValue(0, 1).isNull());  // A row 1 (cast path)
    EXPECT_TRUE(chunk.getValue(1, 2).isNull());  // B row 2 (string path)
    EXPECT_TRUE(chunk.getValue(2, 3).isNull());  // C row 3 (cast path)

    // Everything else is non-null with the right value.
    EXPECT_FALSE(chunk.getValue(0, 0).isNull());
    EXPECT_EQ(chunk.getValue(0, 0).cast(PhysicalType::INTEGER).getValueUnsafe<int32_t>(), 1);
    EXPECT_FALSE(chunk.getValue(0, 2).isNull());
    EXPECT_EQ(chunk.getValue(0, 2).cast(PhysicalType::INTEGER).getValueUnsafe<int32_t>(), 3);
    EXPECT_FALSE(chunk.getValue(1, 0).isNull());
    EXPECT_FALSE(chunk.getValue(2, 0).isNull());
}

// With auto_detect, a NULL (empty) cell must not force its column to STRING: the
// sniffer ignores NULL cells, so a numeric column with missing values still
// infers a numeric type (and keeps the NULLs).
TEST_F(ReadCSVNullTest, AutoDetectIgnoresNullCellsForTyping) {
    DataChunk chunk;
    {
        vector<Value> input;
        input.emplace_back(getCsvFilePath("null_typed.csv"));
        vector<LogicalType> inputTypes = {LogicalTypeId::STRING};
        TableFilterSet filters;
        auto functionPtr = ReadCsvFunc().getFunction({});
        auto& pred = (PredFunction&)*functionPtr;
        std::unordered_map<string, Value> params;
        params.emplace("auto_detect", uint8_t(1));
        params.emplace("header", uint8_t(1));
        vector<LogicalType> returnTypes;
        std::unordered_map<string, idx_t> bindVarName;
        vector<string> names = {"*"};
        auto bind = pred.bindFunction_(context, input, inputTypes, params,
                                       bindVarName, returnTypes, names, filters);
        pred.maxThreadFunction_(context, bind.get());
        auto fopd = pred.initFunction_(context, bind.get());
        chunk.initialize(returnTypes);
        chunk.setCardinality(0);
        pred.function_(context, bind.get(), fopd.get(), nullptr, chunk);
    }

    ASSERT_EQ(chunk.getSize(), 4u);
    // Column a (int with a NULL) and c (double with a NULL) infer numeric, not STRING.
    EXPECT_NE(chunk.data_[0].getType(), PhysicalType::STRING);
    EXPECT_NE(chunk.data_[2].getType(), PhysicalType::STRING);
    // NULLs are preserved: a[1] and c[3] empty.
    EXPECT_TRUE(chunk.getValue(0, 1).isNull());
    EXPECT_TRUE(chunk.getValue(2, 3).isNull());
    EXPECT_FALSE(chunk.getValue(0, 0).isNull());
}