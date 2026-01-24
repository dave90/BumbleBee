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

class ReadCSVSCanTest : public ::testing::Test {
protected:
    ClientContext context;

    void SetUp() override {
    }

    string getCsvFilePath(const string& filename) {
        std::filesystem::path TEST_FILE_PATH = __FILE__;
        std::filesystem::path dataFilePath = TEST_FILE_PATH.parent_path()  / "data" / "input"/ "csv" / filename;
        return dataFilePath.c_str();
    }

    void compareResult(vector<vector<string>>& expected, DataChunk& chunk) {
        EXPECT_EQ(chunk.columnCount(), expected.size());
        for (idx_t col = 0;col < chunk.columnCount();col++) {
            EXPECT_EQ(expected[col].size(), chunk.getSize());
            for (idx_t row = 0;row < chunk.getSize();row++) {
                Value val = chunk.getValue(col, row);
                string str = val.toString();
                if (val.isDoubleQuotedString())
                    str = str.substr(1, str.length() - 2);
                EXPECT_EQ(expected[col][row], str);
            }
        }
    }
};


TEST_F(ReadCSVSCanTest, SimpleCSVScanTest) {
    vector<Value> input;
    input.emplace_back(getCsvFilePath("username.csv"));
    vector<LogicalType> inputTypes = {LogicalTypeId::STRING};
    TableFilterSet filters;
    auto functionPtr = ReadCsvFunc().getFunction({});
    PredFunction& predFunction = (PredFunction&) *functionPtr;
    std::unordered_map<string, Value> params;
    params.emplace("auto_detect", 1 );
    vector<LogicalType> returnTypes;
    vector<string> names = {"*"};
    auto bind = predFunction.bindFunction_(context, input, inputTypes, params, returnTypes, names, filters);
    EXPECT_EQ(predFunction.maxThreadFunction_(context, bind.get()), 1);
    auto fopd = predFunction.initFunction_(context, bind.get());
    DataChunk chunk;
    chunk.initialize(returnTypes);
    predFunction.function_(context, bind.get(), fopd.get(), nullptr, chunk);
    EXPECT_EQ(chunk.getSize(), 5);
}


TEST_F(ReadCSVSCanTest, MultiCSVScanTest) {
    vector<Value> input;
    input.emplace_back(getCsvFilePath("customers-10000.csv"));
    vector<LogicalType> inputTypes = {LogicalTypeId::STRING};
    TableFilterSet filters;
    auto functionPtr = ReadCsvFunc().getFunction({});
    PredFunction& predFunction = (PredFunction&) *functionPtr;
    std::unordered_map<string, Value> params;
    params.emplace("auto_detect", 1 );
    params.emplace("header", 1 );
    vector<LogicalType> returnTypes;
    vector<string> names = {"*"};
    auto bind = predFunction.bindFunction_(context, input, inputTypes, params, returnTypes, names, filters);
    auto threads = predFunction.maxThreadFunction_(context, bind.get());
    EXPECT_EQ(threads, 1);
    idx_t totalLines = 0;
    for (idx_t i=0;i<threads;i++) {
        auto fopd = predFunction.initFunction_(context, bind.get());
        DataChunk chunk;
        chunk.initialize(returnTypes);
        do {
            chunk.setCardinality(0);
            predFunction.function_(context, bind.get(), fopd.get(), nullptr, chunk);
            totalLines += chunk.getSize();
        }while(chunk.getSize());
    }
    EXPECT_EQ(totalLines, 10000);
}



TEST_F(ReadCSVSCanTest, MultiFilesCSVScanTest) {
    vector<Value> input;
    input.emplace_back(getCsvFilePath("customers/**/*.csv"));
    vector<LogicalType> inputTypes = {LogicalTypeId::STRING};
    TableFilterSet filters;
    auto functionPtr = ReadCsvFunc().getFunction({});
    PredFunction& predFunction = (PredFunction&) *functionPtr;
    std::unordered_map<string, Value> params;
    params.emplace("auto_detect", 1 );
    params.emplace("header", 1 );
    vector<LogicalType> returnTypes;
    vector<string> names = {"*"};
    auto bind = predFunction.bindFunction_(context, input, inputTypes, params, returnTypes, names, filters);
    auto threads = predFunction.maxThreadFunction_(context, bind.get());
    EXPECT_GT(threads, 1);
    idx_t totalLines = 0;
    for (idx_t i=0;i<threads;i++) {
        auto fopd = predFunction.initFunction_(context, bind.get());
        DataChunk chunk;
        chunk.initialize(returnTypes);
        do {
            chunk.setCardinality(0);
            predFunction.function_(context, bind.get(), fopd.get(), nullptr, chunk);
            totalLines += chunk.getSize();
        }while(chunk.getSize());
    }
    idx_t expectedLines = 99 + 4875 +1882 +3007 + 36;
    EXPECT_EQ(totalLines, expectedLines);
}


TEST_F(ReadCSVSCanTest, Username2CSVScanTest) {
    vector<Value> input;
    input.emplace_back(getCsvFilePath("username_2.csv"));
    vector<LogicalType> inputTypes = {LogicalTypeId::STRING};
    TableFilterSet filters;
    auto functionPtr = ReadCsvFunc().getFunction({});
    PredFunction& predFunction = (PredFunction&) *functionPtr;
    std::unordered_map<string, Value> params;
    params.emplace("auto_detect", 1 );
    params.emplace("header", 1 );
    vector<LogicalType> returnTypes;
    vector<string> names = {"COLUMN_1USERNAME","COLUMN_2_IDENTIFIER","COLUMN__LAST_NAME"};
    auto bind = predFunction.bindFunction_(context, input, inputTypes, params, returnTypes, names, filters);
    auto threads = predFunction.maxThreadFunction_(context, bind.get());
    EXPECT_EQ(threads, 1);
    idx_t totalLines = 0;
    for (idx_t i=0;i<threads;i++) {
        auto fopd = predFunction.initFunction_(context, bind.get());
        DataChunk chunk;
        chunk.initialize(returnTypes);
        do {
            chunk.setCardinality(0);
            predFunction.function_(context, bind.get(), fopd.get(), nullptr, chunk);
            totalLines += chunk.getSize();
        }while(chunk.getSize());
    }
    idx_t expectedLines = 5;
    EXPECT_EQ(totalLines, expectedLines);
}



TEST_F(ReadCSVSCanTest, MultiFilesCSVDirScanTest) {
    vector<Value> input;
    input.emplace_back(getCsvFilePath("customers"));
    vector<LogicalType> inputTypes = {LogicalTypeId::STRING};
    TableFilterSet filters;
    auto functionPtr = ReadCsvFunc().getFunction({});
    PredFunction& predFunction = (PredFunction&) *functionPtr;
    std::unordered_map<string, Value> params;
    params.emplace("auto_detect", 1 );
    params.emplace("header", 1 );
    vector<LogicalType> returnTypes;
    vector<string> names = {"*"};
    auto bind = predFunction.bindFunction_(context, input, inputTypes, params, returnTypes, names, filters);
    auto threads = predFunction.maxThreadFunction_(context, bind.get());
    EXPECT_GT(threads, 1);
    idx_t totalLines = 0;
    for (idx_t i=0;i<threads;i++) {
        auto fopd = predFunction.initFunction_(context, bind.get());
        DataChunk chunk;
        chunk.initialize(returnTypes);
        do {
            chunk.setCardinality(0);
            predFunction.function_(context, bind.get(), fopd.get(), nullptr, chunk);
            totalLines += chunk.getSize();
        }while(chunk.getSize());
    }
    idx_t expectedLines = 99 + 4875 +1882 +3007 + 36;
    EXPECT_EQ(totalLines, expectedLines);
}