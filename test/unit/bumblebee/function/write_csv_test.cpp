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

    struct WriteParams {
        std::string sep = ",";
        std::string mode = "overwrite";
        std::optional<std::string> partitions;
        bool single_file = false;
        std::optional<std::string> columnsCSV; // "Username,ID,First,Last"

        std::unordered_map<std::string, Value> toMap() const {
            std::unordered_map<std::string, Value> m;
            m["sep"]  = sep;
            m["mode"] = mode;
            if (partitions)  m["partitions"] = *partitions;
            if (single_file) m["single_file"] = 1;
            if (columnsCSV)  m["columns"]     = *columnsCSV;
            return m;
        }
    };

    struct TestCase {
        std::string inputCsv;
        std::string outPath;               // folder or .csv
        WriteParams params1;
        bool multithread = false;
        std::optional<idx_t> expectChunks; // e.g. 10
        bool doSecondRun = false;
        WriteParams params2{};
        std::vector<std::string> expectedHeaderNames; // optional override
        std::vector<std::string> sourceColumnNames;   // names passed to bind
    };


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

    void compareCSVResultAndClean(const string& expectedFilename, const string& resultDirectory, bool clean = false, const string& customHeader = "", idx_t times = 1) {
        string expectedFilepath = getInputCsvFilePath(expectedFilename);
        string resultPath =  getOutputCsvFilePath(resultDirectory);

        string header;
        string expectedHeader;

        // 1) Read expected lines
        vector<string> expectedLines = readLinesNormalized(expectedFilepath);
        expectedHeader = expectedLines[0];
        expectedLines.erase(expectedLines.begin());
        if (!customHeader.empty()) {
            expectedHeader = customHeader;
        }

        vector<string> files;
        if (context.fileSystem_->directoryExists(resultPath))
            files = context.fileSystem_->glob(resultPath + "/**/*.csv");
        else
            files.push_back(resultPath);

        vector<string> resultLines;
        idx_t i=0;
        for (const auto& f : files) {
            vector<string> lines = readLinesNormalized(f);
            if (header.empty())
                header = lines[0];
            else
                EXPECT_EQ(header, lines[0]);
            lines.erase(lines.begin());
            resultLines.insert(resultLines.end(), lines.begin(), lines.end());
        }

        EXPECT_EQ(header, expectedHeader);

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
            if (have != need * times) {
                missing.push_back(val);
            }
        }

        // Anything in result that wasn't expected (or was too many)?
        for (const auto& [val, have] : resultCounts) {
            long long need = 0;
            if (auto it = expectedCounts.find(val); it != expectedCounts.end()) need = it->second;
            if (have != need * times) {
                extra.push_back(val);
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
        vector<string>& names, function_data_ptr_t& bind_data, function_ptr_t& func) {
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
        return chunks;
    }

    static std::string joinHeader(const std::vector<std::string>& names, const std::string& sep) {
        std::ostringstream oss;
        for (size_t i=0;i<names.size();++i) { if (i) oss << sep; oss << names[i]; }
        return oss.str();
    }

    struct Job { std::vector<data_chunk_ptr_t> chunks; function_data_ptr_t bind; function_ptr_t func; };

    Job makeJob(const std::string& inputCsv,
                const std::string& outFolderOrFile,
                const WriteParams& params,
                const std::vector<std::string>& names) {
        auto chunks = readFromCsvFile(inputCsv);

        vector<Value> input;
        input.emplace_back(getOutputCsvFilePath(outFolderOrFile));
        vector<ConstantType> inputTypes = { STRING };
        vector<Expression> filters;
        auto functionPtr = WriteCsvFunc::getFunction();
        auto& pred = static_cast<PredFunction&>(*functionPtr);
        vector<ConstantType> returnTypes = chunks[0]->getTypes();
        auto map = params.toMap();
        auto bind = pred.bindFunction_(context, input, inputTypes, map,
                                       returnTypes, const_cast<std::vector<std::string>&>(names), filters);
        return { std::move(chunks), std::move(bind), std::move(functionPtr) };
    }

    static void runChunksSingleThread(ClientContext& ctx,
                                      PredFunction& pred,
                                      function_data_ptr_t& bind,
                                      const std::vector<data_chunk_ptr_t>& chunks)
    {
        auto fopd = pred.initFunction_(ctx, bind.get());
        for (auto& ch : chunks) {
            pred.function_(ctx, bind.get(), fopd.get(), nullptr, *ch);
        }
        pred.combine_function_(ctx, bind.get(), fopd.get());
        pred.finalize_function_(ctx, bind.get());
    }

    static void runChunksMultiThread(ClientContext& ctx,
                                     PredFunction& pred,
                                     function_data_ptr_t& bind,
                                     const std::vector<data_chunk_ptr_t>& chunks,
                                     idx_t groupSize = 2)
    {
        vector<std::thread> threads;
        for (idx_t i = 0; i < chunks.size(); i += groupSize) {
            threads.emplace_back([i, &pred, &bind, &chunks, &ctx, groupSize]() {
                for (idx_t j = 0; j < groupSize; ++j) {
                    auto fopd = pred.initFunction_(ctx, bind.get());
                    pred.function_(ctx, bind.get(), fopd.get(), nullptr, *chunks[i + j]);
                    pred.combine_function_(ctx, bind.get(), fopd.get());
                }
            });
        }
        for (auto& t : threads) t.join();
        pred.finalize_function_(ctx, bind.get());
    }

    void runOnce(const string& inputCsv, const string& out,
                 const WriteParams& params, const vector<string>& names,
                 bool multithread, std::optional<idx_t> expectChunkCount) {
        auto job = makeJob(inputCsv, out, params, names);
        auto& pred = (PredFunction&)(*job.func);
        if (expectChunkCount) BB_ASSERT(job.chunks.size() == *expectChunkCount);
        if (multithread) runChunksMultiThread(context, pred, job.bind, job.chunks);
        else             runChunksSingleThread(context, pred, job.bind, job.chunks);
    }

    void runScenario(const TestCase& tc) {
        SCOPED_TRACE("Output: " + tc.outPath);
        runOnce(tc.inputCsv, tc.outPath, tc.params1, tc.sourceColumnNames, tc.multithread, tc.expectChunks);
        if (tc.doSecondRun)
            runOnce(tc.inputCsv, tc.outPath, tc.params2, tc.sourceColumnNames, tc.multithread, tc.expectChunks);

        std::string customHeader;
        if (!tc.expectedHeaderNames.empty())
            customHeader = joinHeader(tc.expectedHeaderNames, tc.params1.sep);
        auto times = (tc.doSecondRun)?2:1;
        compareCSVResultAndClean(tc.inputCsv, tc.outPath, /*clean=*/false, customHeader, times);
    }

};


TEST_F(WriteCSVSCanTest, SimpleWriteCSVTest) {

    runScenario({
        "username.csv",
        "Output-SimpleWriteCSVTest",
        WriteParams{ .sep=";", .mode="overwrite" },
         false,
        std::nullopt,
        false, {},
         {},
         {"Username"," Identifier","First name","Last name"}
    });

}


TEST_F(WriteCSVSCanTest, SimpleWriteCSVPartitionTest) {

    runScenario({
        "username.csv",
        "Output-SimpleWriteCSVPartitionTest",
        WriteParams{ .sep=";", .mode="overwrite", .partitions = "Username" },
         false,
        std::nullopt,
        false, {},
         {},
         {"Username"," Identifier","First name","Last name"}
    });

}


TEST_F(WriteCSVSCanTest, WriteCSVSingleFileTest) {

    runScenario({
        "customers-10000.csv",
        "Output-WriteCSVSingleFileTest.csv",
        WriteParams{ .sep=",", .mode="overwrite", .single_file = 1 },
         false,
        std::nullopt,
        false, {},
         {},
{"Index","Customer Id","First Name","Last Name","Company","City","Country","Phone 1","Phone 2","Email","Subscription Date","Website"}
    });

}



TEST_F(WriteCSVSCanTest, WriteCSVSingleAppendFileTest) {

    runScenario({
        "customers-10000.csv",
        "Output-WriteCSVSingleAppendFileTest.csv",
        WriteParams{ .sep=",", .mode="overwrite", .single_file = 1 },
         false,
        std::nullopt,
        true, WriteParams{ .sep=",", .mode="append", .single_file = 1 },
         {},
{"Index","Customer Id","First Name","Last Name","Company","City","Country","Phone 1","Phone 2","Email","Subscription Date","Website"}
    });

}

TEST_F(WriteCSVSCanTest, WriteCSVChangeHeaderTest) {

    runScenario({
       "username.csv",
       "Output-WriteCSVChangeHeaderTest",
       WriteParams{ .sep=";", .mode="overwrite", .columnsCSV = "Username,ID,First,Last" },
        false,
       std::nullopt,
       false, {},
        {"Username", "ID","First","Last"},
        {"Username"," Identifier","First name","Last name"}
   });
}



TEST_F(WriteCSVSCanTest, WriteCSVEscapeTest) {

    runScenario({
    "username_escape.csv",
    "Output-WriteCSVEscapeTest",
    WriteParams{ .sep=";", .mode="overwrite" },
     false,
    std::nullopt,
    false, {},
     {},
     {"Username"," Identifier","First name","Last name"}
});

}


TEST_F(WriteCSVSCanTest, WriteCSVOverwriteMultiThreadTest) {


    runScenario({
        "customers-10000.csv",
        "Output-WriteCSVOverwriteMultiThreadTest",
        WriteParams{ .sep=",", .mode="overwrite",  },
         true,
        10,
        false, {},
         {},
{"Index","Customer Id","First Name","Last Name","Company","City","Country","Phone 1","Phone 2","Email","Subscription Date","Website"}
    });
}



TEST_F(WriteCSVSCanTest, WriteCSVOverwriteMultiThreadSingleFileTest) {
    runScenario({
        "customers-10000.csv",
        "Output-WriteCSVOverwriteMultiThreadTest.csv",
        WriteParams{ .sep=",", .mode="overwrite", .single_file = 1  },
         true,
        10,
        false, {},
         {},
{"Index","Customer Id","First Name","Last Name","Company","City","Country","Phone 1","Phone 2","Email","Subscription Date","Website"}
    });

}




TEST_F(WriteCSVSCanTest, WriteCSVAppendMultiThreadTest) {
    runScenario({
        "customers-10000.csv",
        "Output-WriteCSVOverwriteMultiThreadTest",
        WriteParams{ .sep=",", .mode="overwrite",  },
         true,
        10,
        true, WriteParams{ .sep=",", .mode="append",  },
         {},
{"Index","Customer Id","First Name","Last Name","Company","City","Country","Phone 1","Phone 2","Email","Subscription Date","Website"}
    });

}



TEST_F(WriteCSVSCanTest, WriteCSVOverwritePartitionsMultiThreadTest) {
    runScenario({
        "customers-10000.csv",
        "Output-WriteCSVOverwritePartitionsMultiThreadTest",
        WriteParams{ .sep=",", .mode="overwrite", .partitions = "Country,City"  },
         true,
        10,
        false, {   },
         {},
{"Index","Customer Id","First Name","Last Name","Company","City","Country","Phone 1","Phone 2","Email","Subscription Date","Website"}
    });

}

TEST_F(WriteCSVSCanTest, WriteCSVAppendPartitionsMultiThreadTest) {
    runScenario({
        "customers-10000.csv",
        "Output-WriteCSVAppendPartitionsMultiThreadTest",
        WriteParams{ .sep=",", .mode="overwrite", .partitions = "Company,Country"  },
         true,
        10,
        true, WriteParams{ .sep=",", .mode="append", .partitions = "Company,Country"  },
         {},
{"Index","Customer Id","First Name","Last Name","Company","City","Country","Phone 1","Phone 2","Email","Subscription Date","Website"}
    });

}


TEST_F(WriteCSVSCanTest, WriteCSVAppendPartitionsTest) {
    runScenario({
       "username.csv",
       "Output-WriteCSVAppendPartitionsTest",
       WriteParams{ .sep=";", .mode="overwrite", .partitions = "Username" },
        false,
       std::nullopt,
       true, WriteParams{ .sep=";", .mode="append", .partitions = "Username" },
        {},
        {"Username"," Identifier","First name","Last name"}
   });
}



