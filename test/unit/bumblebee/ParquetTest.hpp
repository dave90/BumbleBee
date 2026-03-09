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
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

#include "bumblebee/ClientContext.hpp"
#include "bumblebee/common/parquet/ParquetReader.hpp"

namespace bumblebee{


class ParquetTest  : public ::testing::Test {
protected:
    ClientContext context;

    void SetUp() override{
    }
    virtual string getFilePath(const string& filename) = 0;

    virtual string getExpectedFilePath(const string& filename) = 0;

    static string replaceExtensionToCsv(const string& parquet_filename) {
        std::filesystem::path p(parquet_filename);
        p.replace_extension(".csv");
        return p.string();
    }

    // Minimal CSV row parser (handles quoted fields and escaped quotes "")
    static vector<string> parseCsvRow(const string& line, char delimiter = ',') {
        vector<string> out;
        string cur;
        bool in_quotes = false;

        for (size_t i = 0; i < line.size(); i++) {
            char c = line[i];

            if (in_quotes) {
                if (c == '"') {
                    // Escaped quote
                    if (i + 1 < line.size() && line[i + 1] == '"') {
                        cur.push_back('"');
                        i++;
                    } else {
                        in_quotes = false;
                    }
                } else {
                    cur.push_back(c);
                }
            } else {
                if (c == '"') {
                    in_quotes = true;
                } else if (c == delimiter) {
                    out.push_back(cur);
                    cur.clear();
                } else {
                    cur.push_back(c);
                }
            }
        }
        out.push_back(cur);
        return out;
    }


    // Loads CSV into column-major storage: expected[col][row]
    static vector<vector<string>> loadExpectedCsvColumnMajor(
        const string& csv_path,
        const vector<string>& columnsName,
        bool skip_header_if_matches = true,
        char delimiter = ','
    ) {
        std::ifstream in(csv_path);
        EXPECT_TRUE(in.is_open()) << "Could not open expected CSV: " << csv_path;

        vector<vector<string>> cols;
        cols.resize(columnsName.size());


        string line;
        bool first_row = true;

        std::getline(in, line);
        auto header = parseCsvRow(line, delimiter);
        std::unordered_map<string, size_t> header_index;
        for (size_t i = 0; i < header.size(); i++) {
            header_index.emplace(StringUtils::normalizeColumnName(header[i]), i);
        }
        vector<size_t> col_indices;
        col_indices.reserve(columnsName.size());

        for (const auto& col_name : columnsName) {
            auto it = header_index.find(col_name);
            EXPECT_TRUE(it != header_index.end());
            col_indices.push_back(it->second);
        }

        while (std::getline(in, line)) {
            // Skip empty lines
            if (line.empty()) {
                continue;
            }

            auto fields = parseCsvRow(line, delimiter);

            // If CSV has fewer fields select the columns
            for (size_t c = 0; c < col_indices.size(); c++) {
                size_t idx = col_indices[c];
                string value;
                if (idx < fields.size())
                    value = fields[idx];   // normal case
                else
                    value = "";            // missing field → empty

                cols[c].push_back(std::move(value));
            }

        }

        return cols;
    }

    // Slice column-major expected data for a given [offset, offset+count)
    static vector<vector<string>> sliceExpected(
        const vector<vector<string>>& expected_all,
        idx_t offset,
        idx_t count
    ) {
        vector<vector<string>> sliced;
        sliced.resize(expected_all.size());
        for (size_t c = 0; c < expected_all.size(); c++) {
            auto begin = expected_all[c].begin() + static_cast<std::ptrdiff_t>(offset);
            auto end = begin + static_cast<std::ptrdiff_t>(count);
            sliced[c].assign(begin, end);
        }
        return sliced;
    }

    void compareResult(vector<vector<string>>& expected, DataChunk& chunk) {
        EXPECT_EQ(chunk.columnCount(), expected.size());
        // Cast as strings
        vector<LogicalType> stringTypes{chunk.columnCount(), LogicalTypeId::STRING};
        DataChunk stringChunk;
        stringChunk.initAndReference(chunk);
        stringChunk.cast(stringTypes);

        for (idx_t col = 0;col < chunk.columnCount();col++) {
            EXPECT_EQ(expected[col].size(), stringChunk.getSize());
            for (idx_t row = 0;row < stringChunk.getSize();row++) {
                Value val = stringChunk.getValue(col, row);
                string str = val.toString();
                if (val.isDoubleQuotedString())
                    str = str.substr(1, str.length() - 2);
                EXPECT_EQ(expected[col][row], str);
            }
        }
    }

    vector<LogicalType> getColumnTypes(string file) {
        string filepath = getFilePath(file);
        ParquetOptions options;
        ParquetReader reader(context, filepath, options);
        return reader.returnTypes_;
    }

    Value getConstantValue(int value, idx_t col,const vector<LogicalType>& types) {
        if (types[col].type() == LogicalTypeId::DECIMAL) {
            int s = types[col].getDecimalData().scale_;
            for (idx_t i = 0; i < s; i++) value *= 10;
        }
        return Value(value).cast(types[col].getPhysicalType());
    }

};


}
