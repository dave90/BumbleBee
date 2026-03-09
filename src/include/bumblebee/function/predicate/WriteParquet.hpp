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
#include "bumblebee/common/BufferedCSVReader.hpp"
#include "bumblebee/common/parquet/ParquetWriter.hpp"
#include "bumblebee/common/serializer/BufferedSerializer.hpp"
#include "bumblebee/function/Function.hpp"
#include "bumblebee/function/PredFunction.hpp"

namespace bumblebee{


struct WriteParquetData : public FunctionData {
    explicit WriteParquetData(FileSystem &fs): fs(fs) {}

    // The folder path of the parquet file to write
    string folder_;

    // global mutex
    mutex mutex_;
    // For each thread return a parquet file to write
    std::unordered_map<string, string> threadFiles_;
    // mutex for each files
    std::unordered_map<string, std::unique_ptr<std::mutex>> lockFiles_;
    // file handle
    std::unordered_map<string, parquet_writer_ptr_t> filesWriter_;
    // if the output is in one single file
    bool isSingleFile_ = false;
    // single file path without extension
    string singleFile_;
    // parquet codec
    format::CompressionCodec::type codec_;

    vector<LogicalType> colTypes_;
    vector<string> colNames_;
    vector<idx_t> colsIdx_;


    FileSystem &fs;

    bool overwrite_;


    string extension_ = ".parquet";
    string tmpExtension_ = ".tmp";

    // return file to write for each thread
    string getFileToWrite();
};


struct WriteParquetOperatorData : public FunctionOperatorData {
    // The thread local file path of the Parquet file to write
    string file_;
    // The thread-local write to write data into
    ParquetWriter* writer_;

    // A chunk for intermediates chunks
    ChunkCollection chunkCollection_;
};


class WriteParquetFunc : public FunctionGenerator {
public:
    string getName() override;

    function_ptr_t createFunction(const vector<LogicalType> &type) override;

    // register the functions
    static void registerFunction(FunctionRegister& funcRegister);
};



}
