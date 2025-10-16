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
#include "bumblebee/function/Function.hpp"
#include "bumblebee/function/PredFunction.hpp"

namespace bumblebee{

struct ReadCSVDataChunk {
    // file index to parse
    idx_t file_{0};
    idx_t start_{0};
    idx_t end_{0};
};

struct ReadCSVData : public FunctionData {
    // The file path of the CSV file to read
    vector<string> files_;
    // The CSV reader options
    BufferedCSVReaderOptions options_;
    // The columns to select
    vector<idx_t> cols_;

    // The expected types to read of ALL the columns
    vector<ConstantType> types_;
    // The initial reader (if any): this is used when automatic detection is used during binding.
    buffered_csv_reader_ptr_t initialReader_;

    // Max threads to parse the csv
    idx_t maxThreads_{0};
    // partitions of data to read for each thread
    vector<vector<ReadCSVDataChunk>> chunks_;
    // Size in bytes for each csv file to parse
    std::unordered_map<string, idx_t> filesSize_;
    // mutext of the read csv
    mutex mutex_;


    //Return the max thread to read the csv
    idx_t getMaxThread();
    // return the chunk to read for a thread
    vector<ReadCSVDataChunk> getNextChunksToRead();
};

struct ReadCSVOperatorData : public FunctionOperatorData {
    // The CSV reader
    buffered_csv_reader_ptr_t csvReader_;
    // The csv to read
    vector<ReadCSVDataChunk> chunks_;
    // cached chunk to read the data and avoid reinitialization of chunks
    DataChunk readChunk_;

    idx_t chunkIndex_{0};
};


class ReadCsvFunc {
public:
    // get the function from the type
    static function_ptr_t getFunction();

    // register the functions
    static void registerFunction(FunctionRegister& funcRegister);
};


}
