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
#pragma once
#include "bumblebee/common/parquet/ParquetReader.hpp"
#include "bumblebee/function/Function.hpp"
#include "bumblebee/function/PredFunction.hpp"
#include "bumblebee/planner/filter/TableFilter.hpp"

namespace bumblebee{

// A read task processes a single parquet file only (tasks never span multiple files).
// For best performance, files should contain at least MORSEL_SIZE rows and row groups
// should be <= MORSEL_SIZE to maximize parallelism.
// Each ReadParquetDataChunk represents a contiguous range of row groups within one file.
// A chunk is processed by exactly one task/thread (chunks are never split across tasks).
struct ReadParquetDataChunk {
    ReadParquetDataChunk():ReadParquetDataChunk(0,0,0){}
    ReadParquetDataChunk(idx_t file, idx_t start, idx_t end)
        : file_(file),
          groupStart_(start),
          groupEnd_(end) {
    }

    // file index to parse
    idx_t file_{0};
    // groupStart_ inclusive
    idx_t groupStart_{0};
    // groupEnd_ inclusive
    idx_t groupEnd_{0};
};

struct ReadParquetData : public FunctionData {
    explicit ReadParquetData(ClientContext &context): context_(context) {}

    parquet_reader_ptr_t reader_;

    // The file path of the parquet file to read
    vector<string> files_;

    ClientContext &context_;

    ParquetOptions options_;

    // The columns to select
    vector<idx_t> cols_;

    table_filter_set_ptr_t filters_;

    // file to be processed
    atomic<idx_t> nextFileToProcess_{0};
    vector<ReadParquetDataChunk> filesToProcess_;

    string extension_ = ".parquet";

    //Return the max thread to read the csv
    idx_t getMaxThread();
    // return the chunk to read for a thread
    ReadParquetDataChunk getNextChunksToRead();
};

struct ReadParquetOperatorData : public FunctionOperatorData {
    // The Parquet reader state
    ParquetReaderScanState readerState_;

    // cached chunk to read the data
    DataChunk readChunk_;

    ReadParquetDataChunk filesToProcess_;

    bool finished_{false};
    bool initialized_{false};
};


class ReadParquetFunc : public FunctionGenerator {
public:
    string getName() override;

    function_ptr_t createFunction(const vector<LogicalType> &type) override;

    // register the functions
    static void registerFunction(FunctionRegister& funcRegister);
};


}
