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
#include <fstream>

#include "bumblebee/common/TypeDefs.hpp"
#include "bumblebee/function/predicate/ReadCsv.hpp"
#include "bumblebee/function/predicate/WriteCsv.hpp"


using namespace bumblebee;

class WriteCSVSCanTest : public ::testing::Test {
protected:
    ClientContext context;
    mutex mutex;

    void SetUp() override{
    }

    string getInputCsvFilePath(const string& filename) {
        std::filesystem::path TEST_FILE_PATH = __FILE__;
        std::filesystem::path dataFilePath = TEST_FILE_PATH.parent_path() / "data" / "input" /filename;
        return dataFilePath.c_str();
    }

    string getOutputCsvFilePath(const string& filename) {
        std::filesystem::path TEST_FILE_PATH = __FILE__;
        std::filesystem::path dataFilePath = TEST_FILE_PATH.parent_path() / "data" / "output" / filename;
        return dataFilePath.c_str();
    }

    vector<data_chunk_ptr_t> readFromCsvFile(const string& filename) {
        vector<data_chunk_ptr_t> result;
        string filepath = getInputCsvFilePath(filename);
        BufferedCSVReaderOptions options;
        options.filePath_ = filepath;
        options.autoDetect_ = true;
        options.compression_ = "none";
        options.hasHeader_ = true;
        options.header_ = true;
        BufferedCSVReader reader(context, options);
        data_chunk_ptr_t chunk ;
        idx_t size = 0;
        do {
            chunk = data_chunk_ptr_t(new DataChunk);
            chunk->initialize(reader.types_);
            chunk->setCardinality(0);
            reader.parseCSV(*chunk);
            size = chunk->getSize();
            if (size > 0) result.push_back(std::move(chunk));
        }while (size != 0);

        return result;
    }

    static vector<string> readLinesNormalized(const string& path) {
        vector<string> out;
        std::ifstream in(path);
        if (!in.is_open()) {
            std::ostringstream oss;
            oss << "Failed to open file: " << path;
            throw std::runtime_error(oss.str());
        }
        string line;
        while (std::getline(in, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back(); // normalize CRLF
            if (line.empty()) continue; // ignore blank lines
            out.push_back(line);
        }
        return out;
    }

    std::unordered_map<string, long long> toMultisetCounts(const vector<string>& lines) {
        std::unordered_map<string, long long> counts;
        for (const auto& s : lines) ++counts[s];
        return counts;
    }

    void compareCSVResultAndClean(const string& expectedFilename, const string& resultDirectory, bool clean = false, const string& customHeader = "") {
        string expectedFilepath = getInputCsvFilePath(expectedFilename);
        string resultPath =  getOutputCsvFilePath(resultDirectory);


        // 1) Read expected lines
        vector<string> expectedLines = readLinesNormalized(expectedFilepath);
        if (!customHeader.empty()) {
            // replace header
            expectedLines[0] = customHeader;
        }

        vector<string> files = context.fileSystem_->glob(resultPath + "/**/*.csv");
        vector<string> resultLines;
        for (const auto& f : files) {
            vector<string> lines = readLinesNormalized(f);
            resultLines.insert(resultLines.end(), lines.begin(), lines.end());
        }

         // 3) Compare as multisets (order-insensitive, count-sensitive)
        auto expectedCounts = toMultisetCounts(expectedLines);
        auto resultCounts   = toMultisetCounts(resultLines);

        // Build diffs (missing and extra)
        vector<string> missing;
        vector<string> extra;

        // Anything expected but not sufficiently present in result
        for (const auto& [val, need] : expectedCounts) {
            long long have = 0;
            if (auto it = resultCounts.find(val); it != resultCounts.end()) have = it->second;
            if (have < need) {
                for (long long i = 0; i < (need - have); ++i) missing.push_back(val);
            }
        }

        // Anything in result that wasn't expected (or was too many)?
        for (const auto& [val, have] : resultCounts) {
            long long need = 0;
            if (auto it = expectedCounts.find(val); it != expectedCounts.end()) need = it->second;
            if (have > need) {
                for (long long i = 0; i < (have - need); ++i) extra.push_back(val);
            }
        }

        if (!missing.empty() || !extra.empty()) {
            std::ostringstream oss;
            oss << "CSV comparison failed.\n";
            if (!missing.empty()) {
                oss << "Missing lines (" << missing.size() << "):\n";
                // Show up to a few examples to keep the message readable
                size_t cap = std::min<size_t>(missing.size(), 10);
                for (size_t i = 0; i < cap; ++i) oss << "  - " << missing[i] << "\n";
                if (missing.size() > cap) oss << "  ... and " << (missing.size() - cap) << " more\n";
            }
            if (!extra.empty()) {
                oss << "Unexpected extra lines (" << extra.size() << "):\n";
                size_t cap = std::min<size_t>(extra.size(), 10);
                for (size_t i = 0; i < cap; ++i) oss << "  + " << extra[i] << "\n";
                if (extra.size() > cap) oss << "  ... and " << (extra.size() - cap) << " more\n";
            }
            std::cout << oss.str();
        }

        EXPECT_EQ(missing.size(), 0);
        EXPECT_EQ(extra.size(), 0);

        if (!clean) return;
        // 4)  Clean up result CSVs if desired
        for (const auto& f : files) {
            context.fileSystem_->removeFile(f);
        }
    }

    vector<data_chunk_ptr_t> loadAndGetFunction(const string& file,
        const string& folder,
        std::unordered_map<string, Value>& params,
        vector<string>& names, function_data_ptr_t& bind_data, function_op_data_ptr_t& data, function_ptr_t& func) {
        auto chunks = readFromCsvFile(file);

        vector<Value> input;
        input.emplace_back(getOutputCsvFilePath(folder));

        vector<ConstantType> inputTypes = {STRING};
        vector<Expression> filters;
        auto functionPtr = WriteCsvFunc::getFunction();
        func = functionPtr;
        PredFunction& predFunction = (PredFunction&) *functionPtr;
        vector<ConstantType> returnTypes = chunks[0]->getTypes();
        bind_data = predFunction.bindFunction_(context, input, inputTypes, params, returnTypes, names, filters);
        data = predFunction.initFunction_(context, bind_data.get());
        return chunks;
    }
};


TEST_F(WriteCSVSCanTest, SimpleWriteCSVScanTest) {

    string file = "username.csv";
    string folder = "Output-SimpleWriteCSVScanTest";
    std::unordered_map<string, Value> params;
    params.insert({"sep", ";"});
    params.insert({"mode", "overwrite"});
    vector<string> names = {"Username"," Identifier","First name","Last name"};

    function_data_ptr_t bind;
    function_op_data_ptr_t fopd;
    function_ptr_t func;
    auto chunks = loadAndGetFunction(file, folder, params, names, bind, fopd, func);
    PredFunction& predFunction = (PredFunction&) *func;
    vector<ConstantType> returnTypes = chunks[0]->getTypes();

    for (auto& chunk: chunks)
        predFunction.function_(context, bind.get(), fopd.get(), nullptr, *chunk);

    predFunction.combine_function_(context, bind.get(), fopd.get());
    predFunction.finalize_function_(context, bind.get());

    compareCSVResultAndClean(file, folder);
}



TEST_F(WriteCSVSCanTest, WriteCSVChangeHeaderScanTest) {
    string file = "username.csv";
    string folder = "Output-WriteCSVChangeHeaderScanTest";
    std::unordered_map<string, Value> params;
    params.insert({"sep", ";"});
    params.insert({"mode", "overwrite"});
    vector<string> names = {"Username","ID","First","Last"};

    function_data_ptr_t bind;
    function_op_data_ptr_t fopd;
    function_ptr_t func;
    auto chunks = loadAndGetFunction(file, folder, params, names, bind, fopd, func);
    PredFunction& predFunction = (PredFunction&) *func;
    vector<ConstantType> returnTypes = chunks[0]->getTypes();

    for (auto& chunk: chunks)
        predFunction.function_(context, bind.get(), fopd.get(), nullptr, *chunk);

    predFunction.combine_function_(context, bind.get(), fopd.get());
    predFunction.finalize_function_(context, bind.get());

    string customHeader;
    for (idx_t i=0;i< names.size();i++) {
        if (i > 0) customHeader += params["sep"].toString();
        customHeader += names[i];
    }
    compareCSVResultAndClean(file, folder, false, customHeader);
}