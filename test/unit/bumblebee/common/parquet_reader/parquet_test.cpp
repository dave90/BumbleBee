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
#include <gtest/gtest.h>

#include "bumblebee/ClientContext.hpp"
#include "bumblebee/common/TypeDefs.hpp"
#include "bumblebee/common/parquet/ParquetReader.hpp"


using namespace bumblebee;

class ParquetScanTest : public ::testing::Test {
protected:
    ClientContext context;

    void SetUp() override{
    }

    string getFilePath(const string& filename) {
        std::filesystem::path TEST_FILE_PATH = __FILE__;
        std::filesystem::path dataFilePath = TEST_FILE_PATH.parent_path() / "data" / filename;
        return dataFilePath.c_str();
    }

    string getExpectedFilePath(const string& filename) {
        std::filesystem::path TEST_FILE_PATH = __FILE__;
        std::filesystem::path expectedFilePath = TEST_FILE_PATH.parent_path() / "expected" / filename;
        return expectedFilePath.c_str();
    }

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
            header_index.emplace(header[i], i);
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

    void testParquetReader(string file, ParquetOptions options = {}, vector<string> columns = {}, vector<idx_t> groups_to_read = {}, TableFilterSet* filters = nullptr) {
        string filepath = getFilePath(file);
        std::cout << filepath << std::endl;
        ParquetReader reader(context, filepath, options);
        auto allColumnsNames = reader.names_;
        vector<idx_t> column_ids;
        vector<idx_t> sliceCols;
        if (columns.empty()) {
            for (idx_t i = 0; i < allColumnsNames.size(); ++i) {
                column_ids.push_back(i);
                columns.push_back(allColumnsNames[i]);
                sliceCols.push_back(i);
            }
        }else {
            std::unordered_set<string> columnsSet(columns.begin(), columns.end());
            for (idx_t i = 0; i < allColumnsNames.size(); ++i) {
                if (columnsSet.contains(allColumnsNames[i])) {
                    column_ids.push_back(i);
                    sliceCols.push_back(i);
                }else
                    column_ids.push_back(COLUMN_IDENTIFIER_ROW_ID);
            }
        }

        auto rows = reader.metadata_->metadata_->num_rows;
        auto row_groups = reader.getFileMetadata()->row_groups.size();
        if (groups_to_read.empty()) {
            for (idx_t i = 0; i < row_groups; ++i) {
                groups_to_read.push_back(i);
            }
        }

        ParquetReaderScanState state;
        reader.initializeScan(state, column_ids, groups_to_read, filters);
        auto types = reader.returnTypes_;
        DataChunk chunk;
        chunk.initialize(types);

        // --- Load expected CSV (same filename, but in expected/ and .csv extension) ---
        const string expected_csv_name = replaceExtensionToCsv(file);
        const string expected_csv_path = getExpectedFilePath(expected_csv_name);
        auto expected_all = loadExpectedCsvColumnMajor(expected_csv_path, columns);

        // Sanity: expected row count should match parquet row count (after optional header skip)
        ASSERT_EQ(static_cast<idx_t>(expected_all.empty() ? 0 : expected_all[0].size()),
                  static_cast<idx_t>(rows))
            << "Expected CSV row count does not match parquet row count";

        // --- Scan + compare chunk-by-chunk ---
        idx_t offset = 0;

        reader.scan(state, chunk);
        while (chunk.getSize()) {
            const idx_t chunk_sz = chunk.getSize();

            ASSERT_LE(offset + chunk_sz, static_cast<idx_t>(expected_all[0].size()))
                << "Chunk exceeds expected CSV size";

            auto expected_slice = sliceExpected(expected_all, offset, chunk_sz);
            DataChunk expectedChunk;
            expectedChunk.initAndReference(chunk, sliceCols);

            compareResult(expected_slice, expectedChunk);

            offset += chunk_sz;

            chunk.setCardinality(0);
            reader.scan(state, chunk);
        }

        EXPECT_EQ(offset, static_cast<idx_t>(rows))
            << "Total scanned rows do not match parquet metadata rows";
    }

};


TEST_F(ParquetScanTest, SimpleParquetScanTest) {
    string file = "t1.parquet";
    testParquetReader(file);
}

TEST_F(ParquetScanTest, SimpleParquetScanOneColTest) {
    string file = "t1.parquet";
    testParquetReader(file, {}, {"i"});
    testParquetReader(file, {}, {"j"});
}

TEST_F(ParquetScanTest, LineItemParquetScanTest) {
    string file = "lineitem-arrow.parquet";
    testParquetReader(file);
}

TEST_F(ParquetScanTest, GZipParquetScanTest) {
    string file = "data_gzip.parquet";
    testParquetReader(file);
}

TEST_F(ParquetScanTest, SnappyParquetScanTest) {
    string file = "data_snappy.parquet";
    testParquetReader(file);
}

TEST_F(ParquetScanTest, ZSTDParquetScanTest) {
    string file = "data_zstd.parquet";
    testParquetReader(file);
}


TEST_F(ParquetScanTest, Int32DecimalParquetScanTest) {
    string file = "int32_decimal.parquet";
    testParquetReader(file);
}

TEST_F(ParquetScanTest, TimestampParquetScanTest) {
    string file = "timestamp.parquet";
    testParquetReader(file);
}

TEST_F(ParquetScanTest, TimetampMSParquetScanTest) {
    string file = "timestamp-ms.parquet";
    testParquetReader(file);
}


TEST_F(ParquetScanTest, MiniHitsParquetScanTest) {
    string file = "mini_hits.parquet";
    testParquetReader(file);
}

TEST_F(ParquetScanTest, TpchLineItemParquetScanTest) {
    string file = "tpch_lineitem.parquet";
    testParquetReader(file);
}

TEST_F(ParquetScanTest, NullParquetScanTest) {
    string file = "null.parquet";
    testParquetReader(file);
}