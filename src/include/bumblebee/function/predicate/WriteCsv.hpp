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
#include "bumblebee/common/serializer/BufferedSerializer.hpp"
#include "bumblebee/function/Function.hpp"
#include "bumblebee/function/PredFunction.hpp"

namespace bumblebee{


struct WriteCSVData : public FunctionData {
    explicit WriteCSVData(FileSystem &fs)
        : fs(fs) {
    }

    // The folder path of the CSV file to write
    string folder_;

    // Whether or not a delimiter was defined by the user
    bool hasDelimiter_ = false;
    // Delimiter to separate columns within each line
    string delimiter_ = ",";
    // Whether or not a quote sign was defined by the user
    bool hasQuote_ = false;
    // Quote used for columns that contain reserved characters, e.g., delimiter
    string quote_ = "\"";
    // Whether or not an escape character was defined by the user
    bool hasEscape_ = false;
    // Escape character to escape quote character
    string escape_;
    // Whether or not a header information was given by the user
    bool hasHeader_ = false;
    // Whether or not the file has a header line
    bool header_ = true;
    // Name of the columns
    vector<string> colNames_;
    // Partitions columns
    vector<idx_t> partitions_;
    // If we need to clear before writing
    bool overwrite_ = false;
    // global mutex
    mutex mutex_;
    // For each thread return a csv file to write
    std::unordered_map<string, string> threadFiles_;
    // mutex for each files
    std::unordered_map<string, std::unique_ptr<std::mutex>> lockFiles_;
    // file handle
    std::unordered_map<string, file_handler_ptr_t> filesHandler_;

    FileSystem &fs;

    string extension_ = ".csv";
    string tmpExtension_ = ".tmp";
    idx_t initial_buff_size = 16384;


    string newline_ = "\n";

    // return file to write for each thread
    string getFileToWrite();

    void writeDataToFile(string& filename, const_data_ptr_t data, idx_t size);

    string getHeader();
};


struct WriteCSVOperatorData : public FunctionOperatorData {
    // The thread local file path of the CSV file to write
    string file_;
    // The thread-local buffer to write data into
    BufferedSerializer serializer_;
    // A chunk with VARCHAR columns to cast intermediates into
    DataChunk chunk_;


    // The size of the CSV file (in bytes) that we buffer before we flush it to disk
    idx_t flushSize_ = 4096 * 8;
};


class WriteCsvFunc {
public:
    // get the function from the type
    static function_ptr_t getFunction();

    // register the functions
    static void registerFunction(FunctionRegister& funcRegister);
};



}
