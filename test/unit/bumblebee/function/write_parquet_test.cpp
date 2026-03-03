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
#include <set>
#include <thread>

#include <gtest/gtest.h>

#include "bumblebee/ClientContext.hpp"
#include "bumblebee/common/TypeDefs.hpp"
#include "bumblebee/function/predicate/ReadParquet.hpp"
#include "bumblebee/function/predicate/WriteParquet.hpp"
#include "bumblebee/planner/filter/TableFilter.hpp"

using namespace bumblebee;

class WriteParquetTest : public ::testing::Test {
protected:
    ClientContext context;

    // -----------------------------------------------------------------------
    // WriteParams
    // -----------------------------------------------------------------------
    struct WriteParams {
        std::string codec      = "UNCOMPRESSED";
        std::string mode       = "overwrite";   // "overwrite" | "append"
        bool        single_file = false;
        std::optional<std::string> columnsCSV; // "A,B,C" → custom column names

        std::unordered_map<std::string, Value> toMap() const {
            std::unordered_map<std::string, Value> m;
            m["codec"] = codec;
            m["mode"]  = mode;
            if (single_file) m["single_file"] = uint8_t(1);
            if (columnsCSV)  m["columns"]     = *columnsCSV;
            return m;
        }
    };

    // -----------------------------------------------------------------------
    // Path helpers
    // -----------------------------------------------------------------------
    string getInputParquetPath(const string& filename) {
        std::filesystem::path base = __FILE__;
        return (base.parent_path() / "data" / "input" / "parquet" / filename).string();
    }

    string getOutputPath(const string& name) {
        std::filesystem::path base = __FILE__;
        return (base.parent_path() / "data" / "output" / name).string();
    }

    // For single_file, writeParquetBind strips ".parquet" from path before
    // appending ".tmp", then finalize renames ".tmp" → ".parquet".
    // Net result: the final parquet file IS at outPath (the original path).
    // For directory mode: final files are folder/**/*.parquet.

    // -----------------------------------------------------------------------
    // ReadResult – typed chunks + schema
    // -----------------------------------------------------------------------
    struct ReadResult {
        vector<data_chunk_ptr_t> chunks;
        vector<LogicalType>      types;
        vector<string>           names;
    };

    // Read a single parquet file (exact path) into typed chunks.
    ReadResult readParquetFile(const string& filePath, vector<string> names) {
        vector<Value> input;
        input.emplace_back(filePath);
        vector<LogicalType>               inputTypes = {LogicalTypeId::STRING};
        TableFilterSet                    filters;
        auto functionPtr = ReadParquetFunc().getFunction({});
        auto& pred = (PredFunction&)*functionPtr;
        std::unordered_map<string, Value>   params;
        vector<LogicalType>               returnTypes;
        std::unordered_map<string, idx_t> bindVarName;
        auto bind = pred.bindFunction_(context, input, inputTypes, params,
                                       bindVarName, returnTypes, names, filters);

        ReadResult result;
        result.types = returnTypes;
        result.names = names;

        idx_t numThreads = pred.maxThreadFunction_(context, bind.get());
        for (idx_t t = 0; t < numThreads; ++t) {
            auto fopd = pred.initFunction_(context, bind.get());
            DataChunk chunk;
            chunk.initialize(returnTypes);
            chunk.setCardinality(0);
            pred.function_(context, bind.get(), fopd.get(), nullptr, chunk);
            while (chunk.getSize() > 0) {
                result.chunks.push_back(chunk.clone());
                chunk.setCardinality(0);
                pred.function_(context, bind.get(), fopd.get(), nullptr, chunk);
            }
        }
        return result;
    }

    // Read the input parquet and collect all chunks across all threads.
    ReadResult readParquetToChunks(const string& inputParquet, vector<string> names) {
        return readParquetFile(getInputParquetPath(inputParquet), std::move(names));
    }

    // -----------------------------------------------------------------------
    // Job – bundles write state
    // -----------------------------------------------------------------------
    struct Job {
        vector<data_chunk_ptr_t> chunks;
        function_data_ptr_t      bind;
        function_ptr_t           func;
        vector<LogicalType>      returnTypes;
    };

    // Build a write job: read input parquet, create WriteParquetFunc binding.
    Job makeJob(const string& inputParquet,
                vector<string> names,
                const string&  outPath,
                const WriteParams& params) {
        auto rd = readParquetToChunks(inputParquet, std::move(names));

        vector<Value> input;
        input.emplace_back(outPath);
        vector<LogicalType> inputTypes = {LogicalTypeId::STRING};
        TableFilterSet      filters;
        auto functionPtr = WriteParquetFunc().getFunction({});
        auto& pred = (PredFunction&)*functionPtr;
        auto   map = params.toMap();

        // WriteParquet bind requires bindVarName populated: name → column index
        std::unordered_map<string, idx_t> bindVarName;
        for (idx_t i = 0; i < rd.names.size(); ++i)
            bindVarName[rd.names[i]] = i;

        auto returnTypes = rd.types;
        auto namesCopy   = rd.names;
        auto bind = pred.bindFunction_(context, input, inputTypes, map,
                                       bindVarName, returnTypes, namesCopy, filters);
        return {std::move(rd.chunks), std::move(bind), std::move(functionPtr),
                std::move(rd.types)};
    }

    // -----------------------------------------------------------------------
    // Cleanup: remove .parquet (and stray .tmp) files from prior runs
    // -----------------------------------------------------------------------
    void cleanOutput(const string& pathOrFolder, bool singleFile) {
        string sep = context.fileSystem_->getFileSeparator();
        if (singleFile) {
            // Final file IS at pathOrFolder (already ends with .parquet).
            if (context.fileSystem_->fileExists(pathOrFolder))
                context.fileSystem_->removeFile(pathOrFolder);
            // Also clean any stray .tmp left by a prior crashed run.
            string singleBase = pathOrFolder;
            if (singleBase.ends_with(".parquet"))
                singleBase.erase(singleBase.size() - string(".parquet").size());
            string tmp = singleBase + ".tmp";
            if (context.fileSystem_->fileExists(tmp))
                context.fileSystem_->removeFile(tmp);
        } else if (context.fileSystem_->directoryExists(pathOrFolder)) {
            for (const auto& ext : {".parquet", ".tmp"}) {
                auto files = context.fileSystem_->glob(
                    pathOrFolder + sep + "**" + sep + "*" + ext);
                for (const auto& f : files)
                    context.fileSystem_->removeFile(f);
            }
        }
    }

    // -----------------------------------------------------------------------
    // Execution helpers
    // -----------------------------------------------------------------------
    static void runChunksSingleThread(ClientContext& ctx,
                                      PredFunction& pred,
                                      function_data_ptr_t& bind,
                                      const vector<data_chunk_ptr_t>& chunks) {
        auto fopd = pred.initFunction_(ctx, bind.get());
        for (const auto& ch : chunks)
            pred.function_(ctx, bind.get(), fopd.get(), nullptr, *ch);
        pred.combine_function_(ctx, bind.get(), fopd.get());
        pred.finalize_function_(ctx, bind.get());
    }

    // Multi-thread: divide chunks into groups (one per thread), each thread
    // handles its group with init → function(per chunk) → combine.
    static void runChunksMultiThread(ClientContext& ctx,
                                     PredFunction& pred,
                                     function_data_ptr_t& bind,
                                     const vector<data_chunk_ptr_t>& chunks,
                                     idx_t groupSize = 1) {
        vector<std::thread> threads;
        for (idx_t i = 0; i < chunks.size(); i += groupSize) {
            threads.emplace_back([i, &pred, &bind, &chunks, &ctx, groupSize]() {
                for (idx_t j = 0; j < groupSize && (i + j) < chunks.size(); ++j) {
                    auto fopd = pred.initFunction_(ctx, bind.get());
                    pred.function_(ctx, bind.get(), fopd.get(), nullptr, *chunks[i + j]);
                    pred.combine_function_(ctx, bind.get(), fopd.get());
                }
            });
        }
        for (auto& t : threads) t.join();
        pred.finalize_function_(ctx, bind.get());
    }

    // -----------------------------------------------------------------------
    // Output reading
    // After finalize: .tmp files are renamed to .parquet
    // -----------------------------------------------------------------------
    ReadResult readOutputChunks(const string& pathOrFolder,
                                vector<string> names,
                                bool singleFile) {
        string sep = context.fileSystem_->getFileSeparator();
        vector<string> files;
        if (singleFile) {
            // Single file: final path IS pathOrFolder (already .parquet)
            EXPECT_TRUE(context.fileSystem_->fileExists(pathOrFolder))
                << "Expected single output file: " << pathOrFolder;
            files.push_back(pathOrFolder);
        } else {
            // Directory: glob for *.parquet
            files = context.fileSystem_->glob(
                pathOrFolder + sep + "**" + sep + "*.parquet");
            EXPECT_FALSE(files.empty())
                << "No .parquet output files found in: " << pathOrFolder;
        }

        ReadResult result;
        for (const auto& file : files) {
            auto fr = readParquetFile(file, names);
            if (result.types.empty()) {
                result.types = fr.types;
                result.names = fr.names;
            }
            for (auto& ch : fr.chunks)
                result.chunks.push_back(std::move(ch));
        }
        return result;
    }

    // -----------------------------------------------------------------------
    // Comparison helpers
    // -----------------------------------------------------------------------

    static std::multiset<string> buildRowMultiset(const vector<data_chunk_ptr_t>& chunks) {
        std::multiset<string> rows;
        for (const auto& ch : chunks) {
            for (idx_t row = 0; row < ch->getSize(); ++row) {
                string rowStr;
                for (idx_t col = 0; col < ch->columnCount(); ++col) {
                    if (col > 0) rowStr += "|";
                    rowStr += ch->getValue(col, row).toString();
                }
                rows.insert(rowStr);
            }
        }
        return rows;
    }

    static idx_t totalRows(const vector<data_chunk_ptr_t>& chunks) {
        idx_t n = 0;
        for (const auto& ch : chunks) n += ch->getSize();
        return n;
    }

    // Verify schema and data of written output against input chunks.
    // multiplier: expected row count = multiplier * inputRows (1 = overwrite, 2 = append)
    // expectedTypes: if null, expects same types as input.
    void compareParquetOutput(const vector<data_chunk_ptr_t>& inputChunks,
                              const vector<LogicalType>&       inputTypes,
                              const string&                    outputPath,
                              vector<string>                   outputNames,
                              bool                             singleFile,
                              idx_t                            multiplier = 1,
                              const vector<LogicalType>*       expectedOutputTypes = nullptr) {
        auto out = readOutputChunks(outputPath, std::move(outputNames), singleFile);

        // --- Schema check ---
        const auto& refTypes = expectedOutputTypes ? *expectedOutputTypes : inputTypes;
        ASSERT_EQ(out.types.size(), refTypes.size()) << "Column count mismatch";
        for (size_t i = 0; i < refTypes.size(); ++i)
            EXPECT_EQ(out.types[i], refTypes[i]) << "Type mismatch at column " << i;

        // --- Row count check ---
        idx_t expectedRows = totalRows(inputChunks) * multiplier;
        EXPECT_EQ(expectedRows, totalRows(out.chunks)) << "Row count mismatch";

        // --- Data check (multiset, order-insensitive) ---
        // Build expected multiset: each input row repeated multiplier times
        auto inputRows = buildRowMultiset(inputChunks);
        std::multiset<string> expectedRows_set;
        for (const auto& r : inputRows)
            for (idx_t k = 0; k < multiplier; ++k)
                expectedRows_set.insert(r);

        auto outputRows = buildRowMultiset(out.chunks);

        if (expectedRows_set != outputRows) {
            size_t cap = 10;
            std::ostringstream oss;
            oss << "Data mismatch.\n";
            size_t shown = 0;
            for (const auto& r : expectedRows_set) {
                if (!outputRows.count(r)) {
                    oss << "  missing: " << r << "\n";
                    if (++shown >= cap) { oss << "  ...\n"; break; }
                }
            }
            shown = 0;
            for (const auto& r : outputRows) {
                if (!expectedRows_set.count(r)) {
                    oss << "  extra:   " << r << "\n";
                    if (++shown >= cap) { oss << "  ...\n"; break; }
                }
            }
            FAIL() << oss.str();
        }
    }
};


// ---------------------------------------------------------------------------
// Tests: codec variants (single-thread, data_zstd.parquet)
// ---------------------------------------------------------------------------

TEST_F(WriteParquetTest, WriteParquetUncompressed) {
    auto outPath = getOutputPath("Out-Parquet-Uncomp");
    cleanOutput(outPath, false);
    auto job = makeJob("data_zstd.parquet", {"COL1","COL2","COL3"}, outPath, WriteParams{});
    auto& pred = (PredFunction&)*job.func;
    runChunksSingleThread(context, pred, job.bind, job.chunks);
    compareParquetOutput(job.chunks, job.returnTypes, outPath, {"COL1","COL2","COL3"}, false);
}

TEST_F(WriteParquetTest, WriteParquetSnappy) {
    auto outPath = getOutputPath("Out-Parquet-Snappy");
    cleanOutput(outPath, false);
    auto job = makeJob("data_zstd.parquet", {"COL1","COL2","COL3"}, outPath,
                       WriteParams{.codec = "SNAPPY"});
    auto& pred = (PredFunction&)*job.func;
    runChunksSingleThread(context, pred, job.bind, job.chunks);
    compareParquetOutput(job.chunks, job.returnTypes, outPath, {"COL1","COL2","COL3"}, false);
}

TEST_F(WriteParquetTest, WriteParquetGzip) {
    auto outPath = getOutputPath("Out-Parquet-Gzip");
    cleanOutput(outPath, false);
    auto job = makeJob("data_zstd.parquet", {"COL1","COL2","COL3"}, outPath,
                       WriteParams{.codec = "GZIP"});
    auto& pred = (PredFunction&)*job.func;
    runChunksSingleThread(context, pred, job.bind, job.chunks);
    compareParquetOutput(job.chunks, job.returnTypes, outPath, {"COL1","COL2","COL3"}, false);
}

TEST_F(WriteParquetTest, WriteParquetZstd) {
    auto outPath = getOutputPath("Out-Parquet-Zstd");
    cleanOutput(outPath, false);
    auto job = makeJob("data_zstd.parquet", {"COL1","COL2","COL3"}, outPath,
                       WriteParams{.codec = "ZSTD"});
    auto& pred = (PredFunction&)*job.func;
    runChunksSingleThread(context, pred, job.bind, job.chunks);
    compareParquetOutput(job.chunks, job.returnTypes, outPath, {"COL1","COL2","COL3"}, false);
}

// ---------------------------------------------------------------------------
// Single file mode (mini_hits.parquet)
// ---------------------------------------------------------------------------

TEST_F(WriteParquetTest, WriteParquetSingleFile) {
    auto outPath = getOutputPath("Out-Parquet-Single.parquet");
    cleanOutput(outPath, true);
    auto job = makeJob("mini_hits.parquet",
                       {"TITLE","EVENTTIME","WATCHID","JAVAENABLE"},
                       outPath, WriteParams{.single_file = true});
    auto& pred = (PredFunction&)*job.func;
    runChunksSingleThread(context, pred, job.bind, job.chunks);
    compareParquetOutput(job.chunks, job.returnTypes, outPath,
                         {"TITLE","EVENTTIME","WATCHID","JAVAENABLE"}, true);
}

// ---------------------------------------------------------------------------
// Custom column names
// ---------------------------------------------------------------------------

TEST_F(WriteParquetTest, WriteParquetCustomColumns) {
    auto outPath = getOutputPath("Out-Parquet-CustomCols");
    cleanOutput(outPath, false);
    // Write COL1,COL2,COL3 but name them A,B,C in the output schema
    auto job = makeJob("data_zstd.parquet", {"COL1","COL2","COL3"}, outPath,
                       WriteParams{.columnsCSV = "A,B,C"});
    auto inputTypes = job.returnTypes;
    auto& pred = (PredFunction&)*job.func;
    runChunksSingleThread(context, pred, job.bind, job.chunks);
    // Read back with custom names; verify schema names=A,B,C and data matches
    compareParquetOutput(job.chunks, inputTypes, outPath, {"A","B","C"}, false,
                         /*multiplier=*/1, &inputTypes);
}

// ---------------------------------------------------------------------------
// Selected columns only (2 of 4 from mini_hits)
// ---------------------------------------------------------------------------

TEST_F(WriteParquetTest, WriteParquetSelectedColumns) {
    auto outPath = getOutputPath("Out-Parquet-SelectedCols");
    cleanOutput(outPath, false);
    auto job = makeJob("mini_hits.parquet", {"TITLE","WATCHID"}, outPath, WriteParams{});
    auto& pred = (PredFunction&)*job.func;
    runChunksSingleThread(context, pred, job.bind, job.chunks);
    compareParquetOutput(job.chunks, job.returnTypes, outPath, {"TITLE","WATCHID"}, false);
}

// ---------------------------------------------------------------------------
// Overwrite mode: second write replaces first (same row count, not doubled)
// ---------------------------------------------------------------------------

TEST_F(WriteParquetTest, WriteParquetOverwrite) {
    auto outPath = getOutputPath("Out-Parquet-Overwrite");
    cleanOutput(outPath, false);

    // First write
    {
        auto job = makeJob("data_zstd.parquet", {"COL1","COL2","COL3"}, outPath,
                           WriteParams{.mode = "overwrite"});
        auto& pred = (PredFunction&)*job.func;
        runChunksSingleThread(context, pred, job.bind, job.chunks);
    }
    // Second write: overwrite should delete the first run's parquets
    {
        auto job = makeJob("data_zstd.parquet", {"COL1","COL2","COL3"}, outPath,
                           WriteParams{.mode = "overwrite"});
        auto& pred = (PredFunction&)*job.func;
        runChunksSingleThread(context, pred, job.bind, job.chunks);
        // Verify: row count == 1x (not 2x)
        compareParquetOutput(job.chunks, job.returnTypes, outPath,
                             {"COL1","COL2","COL3"}, false, /*multiplier=*/1);
    }
}

// ---------------------------------------------------------------------------
// Append mode: second write adds to first (row count doubled)
// ---------------------------------------------------------------------------

TEST_F(WriteParquetTest, WriteParquetAppend) {
    auto outPath = getOutputPath("Out-Parquet-Append");
    cleanOutput(outPath, false);

    // First write
    {
        auto job = makeJob("data_zstd.parquet", {"COL1","COL2","COL3"}, outPath,
                           WriteParams{.mode = "overwrite"});
        auto& pred = (PredFunction&)*job.func;
        runChunksSingleThread(context, pred, job.bind, job.chunks);
    }
    // Second write: append — keep old file, add new file alongside it
    {
        auto job = makeJob("data_zstd.parquet", {"COL1","COL2","COL3"}, outPath,
                           WriteParams{.mode = "append"});
        auto& pred = (PredFunction&)*job.func;
        runChunksSingleThread(context, pred, job.bind, job.chunks);
        // Verify: row count == 2x (first + second write)
        compareParquetOutput(job.chunks, job.returnTypes, outPath,
                             {"COL1","COL2","COL3"}, false, /*multiplier=*/2);
    }
}

// ---------------------------------------------------------------------------
// Overwrite single file: second write replaces first
// ---------------------------------------------------------------------------

TEST_F(WriteParquetTest, WriteParquetOverwriteSingleFile) {
    auto outPath = getOutputPath("Out-Parquet-OverwriteSingle.parquet");
    cleanOutput(outPath, true);

    // First write
    {
        auto job = makeJob("data_zstd.parquet", {"COL1","COL2","COL3"}, outPath,
                           WriteParams{.mode = "overwrite", .single_file = true});
        auto& pred = (PredFunction&)*job.func;
        runChunksSingleThread(context, pred, job.bind, job.chunks);
    }
    // Second write with overwrite
    {
        auto job = makeJob("data_zstd.parquet", {"COL1","COL2","COL3"}, outPath,
                           WriteParams{.mode = "overwrite", .single_file = true});
        auto& pred = (PredFunction&)*job.func;
        runChunksSingleThread(context, pred, job.bind, job.chunks);
        compareParquetOutput(job.chunks, job.returnTypes, outPath,
                             {"COL1","COL2","COL3"}, true, /*multiplier=*/1);
    }
}

// ---------------------------------------------------------------------------
// Multi-thread (tpch_lineitem.parquet – 5 row groups)
// ---------------------------------------------------------------------------

TEST_F(WriteParquetTest, WriteParquetMultiThread) {
    auto outPath = getOutputPath("Out-Parquet-MultiThread");
    cleanOutput(outPath, false);
    auto job = makeJob("tpch_lineitem.parquet",
                       {"L_ORDERKEY","L_RETURNFLAG","L_LINESTATUS"},
                       outPath, WriteParams{});
    auto& pred = (PredFunction&)*job.func;
    runChunksMultiThread(context, pred, job.bind, job.chunks, 1);
    compareParquetOutput(job.chunks, job.returnTypes, outPath,
                         {"L_ORDERKEY","L_RETURNFLAG","L_LINESTATUS"}, false);
}

TEST_F(WriteParquetTest, WriteParquetMultiThreadSingleFile) {
    auto outPath = getOutputPath("Out-Parquet-MultiThreadSingle.parquet");
    cleanOutput(outPath, true);
    auto job = makeJob("tpch_lineitem.parquet",
                       {"L_ORDERKEY","L_RETURNFLAG","L_LINESTATUS"},
                       outPath, WriteParams{.single_file = true});
    auto& pred = (PredFunction&)*job.func;
    runChunksMultiThread(context, pred, job.bind, job.chunks, 1);
    compareParquetOutput(job.chunks, job.returnTypes, outPath,
                         {"L_ORDERKEY","L_RETURNFLAG","L_LINESTATUS"}, true);
}

TEST_F(WriteParquetTest, WriteParquetSnappyMultiThread) {
    auto outPath = getOutputPath("Out-Parquet-SnappyMulti");
    cleanOutput(outPath, false);
    auto job = makeJob("tpch_lineitem.parquet",
                       {"L_ORDERKEY","L_RETURNFLAG","L_LINESTATUS"},
                       outPath, WriteParams{.codec = "SNAPPY"});
    auto& pred = (PredFunction&)*job.func;
    runChunksMultiThread(context, pred, job.bind, job.chunks, 1);
    compareParquetOutput(job.chunks, job.returnTypes, outPath,
                         {"L_ORDERKEY","L_RETURNFLAG","L_LINESTATUS"}, false);
}
