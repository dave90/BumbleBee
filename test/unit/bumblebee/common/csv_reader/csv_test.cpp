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


using namespace bumblebee;

class CSVSCanTest : public ::testing::Test {
protected:
    ClientContext context;

    void SetUp() override{
    }

    string getCsvFilePath(const string& filename) {
        std::filesystem::path TEST_FILE_PATH = __FILE__;
        std::filesystem::path dataFilePath = TEST_FILE_PATH.parent_path() / "data" / filename;
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


TEST_F(CSVSCanTest, SimpleCSVScanTest) {
    string file = "username.csv";
    string filepath = getCsvFilePath(file);
    std::cout << filepath << std::endl;
    BufferedCSVReaderOptions options;
    options.filePath_ = filepath;
    options.autoDetect_ = true;
    options.compression_ = "none";
    options.hasHeader_ = true;
    options.header_ = true;
    std::cout << options.toString() << std::endl;
    BufferedCSVReader reader(context, options);
    vector<ConstantType> expectedTypes = {STRING, INTEGER, STRING, STRING};
    EXPECT_EQ(reader.types_, expectedTypes);
    EXPECT_GE(reader.avgBytesInChunk_, 170);
    EXPECT_LE(reader.avgBytesInChunk_, 190);
    DataChunk chunk;
    chunk.initialize(reader.types_);
    reader.parseCSV(chunk);
    std::cout << chunk.toString() << std::endl;
    EXPECT_EQ(chunk.getSize(), 5);

    vector<vector<string>> expected = {
        // Column 1: Username
        {"booker12", "grey07", "johnson81", "jenkins46", "smith79"},
        // Column 2: Identifier
        {"9012", "2070", "4081", "9346", "5079"},
        // Column 3: First name
        {"Rachel", "Laura", "Craig", "Mary", "Jamie"},
        // Column 4: Last name
        {"Booker", "Grey", "Johnson", "Jenkins", "Smith"}
    };
    compareResult(expected, chunk);
}


TEST_F(CSVSCanTest, FloatCSVScanTest) {
    string file = "double.csv";
    string filepath = getCsvFilePath(file);
    std::cout << filepath << std::endl;
    BufferedCSVReaderOptions options;
    options.filePath_ = filepath;
    options.autoDetect_ = true;
    options.compression_ = "none";
    options.hasHeader_ = true;
    options.header_ = true;
    std::cout << options.toString() << std::endl;
    BufferedCSVReader reader(context, options);
    vector<ConstantType> expectedTypes = {FLOAT ,STRING, INTEGER, STRING, STRING};
    EXPECT_EQ(reader.types_, expectedTypes);
    DataChunk chunk;
    chunk.initialize(reader.types_);
    reader.parseCSV(chunk);
    std::cout << chunk.toString() << std::endl;
    EXPECT_EQ(chunk.getSize(), 5);

    vector<vector<string>> expected = {
        // Columns 0: double
        {"10.000000", "3.000000", "2.022000", "1.200000", "12.100000",},
        // Column 1: Username
        {"booker12", "grey07", "johnson81", "jenkins46", "smith79"},
        // Column 2: Identifier
        {"9012", "2070", "4081", "9346", "5079"},
        // Column 3: First name
        {"Rachel", "Laura", "Craig", "Mary", "Jamie"},
        // Column 4: Last name
        {"Booker", "Grey", "Johnson", "Jenkins", "Smith"}
    };
    compareResult(expected, chunk);
}


TEST_F(CSVSCanTest, MultiScanCSVTest) {
    string file = "customers-10000.csv";
    string filepath = getCsvFilePath(file);
    std::cout << filepath << std::endl;
    BufferedCSVReaderOptions options;
    options.filePath_ = filepath;
    options.autoDetect_ = true;
    options.compression_ = "none";
    options.hasHeader_ = true;
    options.header_ = true;
    std::cout << options.toString() << std::endl;
    BufferedCSVReader reader(context, options);
    EXPECT_GE(reader.avgBytesInChunk_, 170000); // more or less 180KB
    EXPECT_LE(reader.avgBytesInChunk_, 190000);
    vector<string> colNamesExpected = {
        "INDEX",
       "CUSTOMER_ID",
       "FIRST_NAME",
       "LAST_NAME",
       "COMPANY",
       "CITY",
       "COUNTRY",
       "PHONE_1",
       "PHONE_2",
       "EMAIL",
       "SUBSCRIPTION_DATE",
       "WEBSITE"
    };
    EXPECT_EQ(reader.colNames_, colNamesExpected);
    EXPECT_EQ(reader.types_.size(), 12);
    EXPECT_EQ(reader.types_[0], INTEGER);
    EXPECT_GT(reader.bytesInChunk_, 0);
    DataChunk chunk;
    chunk.initialize(reader.types_);
    idx_t lines = 0;
    do {
        chunk.setCardinality(0);
        reader.parseCSV(chunk);
        lines += chunk.getSize();
    }while (chunk.getSize() != 0);
    EXPECT_EQ(lines, 10000);
}



TEST_F(CSVSCanTest, MultiScanNotAutodetectCSVTest) {
    string file = "customers-10000.csv";
    string filepath = getCsvFilePath(file);
    std::cout << filepath << std::endl;
    BufferedCSVReaderOptions options;
    options.filePath_ = filepath;
    options.autoDetect_ = false;
    options.hasDelimiter_ = true;
    options.delimiter_ = ",";
    options.compression_ = "none";
    options.hasHeader_ = true;
    options.header_ = true;
    options.allVarchar_ = true;
    std::cout << options.toString() << std::endl;
    vector<ConstantType> types(12, STRING);
    BufferedCSVReader reader(context, options, types);
    EXPECT_GT(reader.bytesInChunk_, 0);
    DataChunk chunk;
    chunk.initialize(reader.types_);
    idx_t lines = 0;
    do {
        chunk.setCardinality(0);
        reader.parseCSV(chunk);
        lines += chunk.getSize();
    }while (chunk.getSize() != 0);
    EXPECT_EQ(lines, 10000);
}


TEST_F(CSVSCanTest, MultiScanSkipRowsCSVTest) {
    string file = "people-100000.csv";
    string filepath = getCsvFilePath(file);
    std::cout << filepath << std::endl;
    BufferedCSVReaderOptions options;
    options.filePath_ = filepath;
    options.autoDetect_ = true;
    options.compression_ = "none";
    options.hasHeader_ = true;
    options.header_ = true;
    options.skipRows_ = 5000;
    std::cout << options.toString() << std::endl;
    BufferedCSVReader reader(context, options);
    vector<ConstantType> expectedTypes = {STRING, INTEGER, STRING, STRING};
    EXPECT_EQ(reader.types_.size(), 9);
    EXPECT_EQ(reader.types_[0], INTEGER);
    DataChunk chunk;
    chunk.initialize(reader.types_);
    idx_t lines = 0;
    do {
        chunk.setCardinality(0);
        reader.parseCSV(chunk);
        lines += chunk.getSize();
    }while (chunk.getSize() != 0);
    EXPECT_EQ(lines, 95000);
}



TEST_F(CSVSCanTest, SkipAllRowsCSVTest) {
    string file = "customers-10000.csv";
    string filepath = getCsvFilePath(file);
    std::cout << filepath << std::endl;
    BufferedCSVReaderOptions options;
    options.filePath_ = filepath;
    options.numCols_ = 4;
    options.hasDelimiter_ = true;
    options.delimiter_ = ",";
    options.compression_ = "none";
    options.hasHeader_ = true;
    options.header_ = true;
    options.skipRows_ = 10100;
    options.autoDetect_ = false;
    std::cout << options.toString() << std::endl;
    BufferedCSVReader reader(context, options);
    DataChunk chunk;
    chunk.initialize(reader.types_);
    chunk.setCardinality(0);
    reader.parseCSV(chunk);
    EXPECT_EQ(chunk.getSize(), 0);
}