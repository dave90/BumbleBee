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
#include <fstream>
#include <set>

#include <gtest/gtest.h>

#include "bumblebee/common/TypeDefs.hpp"
#include "bumblebee/common/types/DataChunk.hpp"
#include "bumblebee/common/types/Value.hpp"
#include "bumblebee/function/predicate/WriteCsv.hpp"

using namespace bumblebee;

// A NULL cell is written as the (default empty) null marker, so it round-trips
// as an empty CSV field.
class WriteCSVNullTest : public ::testing::Test {
protected:
    ClientContext context;

    string getOutputDir(const string& name) {
        std::filesystem::path base = __FILE__;
        return (base.parent_path() / "data" / "output" / name).string();
    }

    void cleanDir(const string& dir) {
        if (context.fileSystem_->directoryExists(dir)) {
            for (auto& f : context.fileSystem_->glob(dir + "/**/*.csv"))
                context.fileSystem_->removeFile(f);
            for (auto& f : context.fileSystem_->glob(dir + "/**/*.tmp"))
                context.fileSystem_->removeFile(f);
        }
    }

    static std::set<string> dataLines(const string& path) {
        std::set<string> out;
        std::ifstream in(path);
        string line;
        bool first = true;
        while (std::getline(in, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (first) { first = false; continue; }  // skip header
            if (!line.empty()) out.insert(line);
        }
        return out;
    }
};

TEST_F(WriteCSVNullTest, NullCellsWrittenAsEmptyFields) {
    auto outDir = getOutputDir("Out-Csv-Null");
    cleanDir(outDir);

    vector<LogicalType> types = {LogicalTypeId::INTEGER, LogicalTypeId::STRING};
    vector<string> names = {"A", "B"};
    DataChunk chunk;
    chunk.initialize(types);
    chunk.setCardinality(3);
    chunk.data_[0].setValue(0, Value((int32_t)10));
    chunk.data_[1].setValue(0, Value("x"));
    chunk.data_[0].setValue(1, Value::null());      // A NULL
    chunk.data_[1].setValue(1, Value("y"));
    chunk.data_[0].setValue(2, Value((int32_t)30));
    chunk.data_[1].setValue(2, Value::null());      // B NULL

    {
        vector<Value> input;
        input.emplace_back(outDir);
        vector<LogicalType> inputTypes = {LogicalTypeId::STRING};
        TableFilterSet filters;
        auto functionPtr = WriteCsvFunc().getFunction({});
        auto& pred = (PredFunction&)*functionPtr;
        std::unordered_map<string, Value> params;
        params["sep"] = string(",");
        params["mode"] = string("overwrite");
        auto returnTypes = types;
        std::unordered_map<string, idx_t> bindVarName;
        auto namesCopy = names;
        auto bind = pred.bindFunction_(context, input, inputTypes, params, bindVarName,
                                       returnTypes, namesCopy, filters);
        auto fopd = pred.initFunction_(context, bind.get());
        pred.function_(context, bind.get(), fopd.get(), nullptr, chunk);
        pred.combine_function_(context, bind.get(), fopd.get());
        pred.finalize_function_(context, bind.get());
    }

    auto files = context.fileSystem_->glob(outDir + "/**/*.csv");
    ASSERT_FALSE(files.empty());
    std::set<string> lines;
    for (auto& f : files) {
        auto l = dataLines(f);
        lines.insert(l.begin(), l.end());
    }

    // NULL integer -> leading empty field; NULL string -> trailing empty field.
    EXPECT_EQ(lines.count("10,x"), 1u);
    EXPECT_EQ(lines.count(",y"), 1u);
    EXPECT_EQ(lines.count("30,"), 1u);
}