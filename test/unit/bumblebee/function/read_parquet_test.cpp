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

#include "bumblebee/ClientContext.hpp"
#include "../ParquetTest.hpp"
#include "bumblebee/function/predicate/ReadParquet.hpp"
#include "bumblebee/planner/filter/TableFilter.hpp"

using namespace bumblebee;

class ReadParquetTest : public ParquetTest {
protected:
    ClientContext context;

    void SetUp() override{
    }

    string getFilePath(const string& filename) override {
        std::filesystem::path TEST_FILE_PATH = __FILE__;
        std::filesystem::path dataFilePath = TEST_FILE_PATH.parent_path() / "data" / "input" / "parquet" / filename;
        return dataFilePath.c_str();
    }

    string getExpectedFilePath(const string& filename) override{
        std::filesystem::path TEST_FILE_PATH = __FILE__;
        std::filesystem::path expectedFilePath = TEST_FILE_PATH.parent_path() / "expected" / "parquet" / filename;
        return expectedFilePath.c_str();
    }

    bool executeReadThread(const string &file, const vector<string>& expectedNames, PredFunction &predFunction, const vector<LogicalType>& returnTypes, function_data_ptr_t& bind, idx_t& offset) {
        auto fopd = predFunction.initFunction_(context, bind.get());
        DataChunk chunk;
        chunk.initialize(returnTypes);

        auto &bindParquet = (ReadParquetData&) *bind;
        auto &bindOpParquet = (ReadParquetOperatorData&) *fopd;
        // --- Load expected CSV (same filename, but in expected/ and .csv extension) ---
        const string expected_csv_name = replaceExtensionToCsv(file);
        const string expected_csv_path = getExpectedFilePath(expected_csv_name);
        auto expected_all = loadExpectedCsvColumnMajor(expected_csv_path, expectedNames);


        predFunction.function_(context, bind.get(), fopd.get(), nullptr, chunk);
        while (chunk.getSize()) {
            const idx_t chunk_sz = chunk.getSize();
            EXPECT_LE(offset + chunk_sz, static_cast<idx_t>(expected_all[0].size()))
                    << "Chunk exceeds expected CSV size";

            auto expected_slice = sliceExpected(expected_all, offset, chunk_sz);
            compareResult(expected_slice, chunk);

            offset += chunk_sz;

            chunk.setCardinality(0);
            predFunction.function_(context, bind.get(), fopd.get(), nullptr, chunk);
        }
        return false;
    }

    void readParquetTest(const string& file, vector<string> names, idx_t threadExpected, vector<string> expectedNames = {}) {
        vector<Value> input;
        input.emplace_back(getFilePath(file));

        vector<LogicalType> inputTypes = {LogicalTypeId::STRING};
        TableFilterSet filters;
        auto functionPtr = ReadParquetFunc().getFunction({});
        auto& predFunction = (PredFunction&)*functionPtr;
        std::unordered_map<string, Value> params;
        vector<LogicalType> returnTypes;
        std::unordered_map<string, idx_t> bindVarName;
        auto bind = predFunction.bindFunction_(context, input, inputTypes, params, bindVarName, returnTypes, names, filters);
        EXPECT_EQ(predFunction.maxThreadFunction_(context, bind.get()), threadExpected);
        if (expectedNames.empty())
            expectedNames = names;
        EXPECT_EQ(returnTypes.size(), expectedNames.size());

        idx_t offset = 0;
        for (idx_t thread = 0; thread < threadExpected; ++thread) {
            executeReadThread(file, expectedNames, predFunction, returnTypes, bind, offset);
        }

    }

};


TEST_F(ReadParquetTest, ReadParquetDataTest) {
    auto file = "data_zstd.parquet";
    vector<string> selectedNames = {"COL1","COL2","COL3"};
    readParquetTest(file, {"*"}, 1, selectedNames);

    readParquetTest(file, {"COL1","COL3"}, 1);
}


TEST_F(ReadParquetTest, ReadParquetMiniHitsTest) {
    auto file = "mini_hits.parquet";
    vector<string> names = {"TITLE", "EVENTTIME", "WATCHID","JAVAENABLE"};
    readParquetTest(file, names, 1);
}


TEST_F(ReadParquetTest, ReadParquetTpchLinetimeTest) {
    auto file = "tpch_lineitem.parquet";
    {
        vector<string> names = {"L_ORDERKEY","L_RETURNFLAG", "L_SHIPDATE"};
        readParquetTest(file, names, 5);
    }
    {
        vector<string> names = {"L_COMMENT", "L_SHIPMODE", "L_RECEIPTDATE", "L_SHIPDATE",
            "L_RETURNFLAG", "L_LINESTATUS", "L_COMMITDATE", "L_DISCOUNT",
            "L_SUPPKEY", "L_PARTKEY", "L_EXTENDEDPRICE", "L_QUANTITY", "L_TAX",
            "L_SHIPINSTRUCT", "L_LINENUMBER", "L_ORDERKEY"};
        readParquetTest(file, names, 5);
    }
}


TEST_F(ReadParquetTest, ReadParquetGlobTest) {
    auto file = "glob";
    {
        vector<string> names = {"*"};
        readParquetTest(file, names, 2);
    }
}
