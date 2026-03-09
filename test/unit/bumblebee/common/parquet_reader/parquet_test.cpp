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

#include "../../ParquetTest.hpp"
#include "bumblebee/ClientContext.hpp"
#include "bumblebee/common/TypeDefs.hpp"
#include "bumblebee/common/parquet/ParquetReader.hpp"
#include "bumblebee/planner/filter/ConstantFilter.hpp"
#include "bumblebee/planner/filter/TableFilter.hpp"


using namespace bumblebee;

class ParquetScanTest : public ParquetTest {
protected:
    ClientContext context;

    void SetUp() override{
    }

    string getFilePath(const string& filename) override {
        std::filesystem::path TEST_FILE_PATH = __FILE__;
        std::filesystem::path dataFilePath = TEST_FILE_PATH.parent_path() / "data" / filename;
        return dataFilePath.c_str();
    }

    string getExpectedFilePath(const string& filename) override{
        std::filesystem::path TEST_FILE_PATH = __FILE__;
        std::filesystem::path expectedFilePath = TEST_FILE_PATH.parent_path() / "expected" / filename;
        return expectedFilePath.c_str();
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
                columns.push_back(StringUtils::normalizeColumnName(allColumnsNames[i]));
                sliceCols.push_back(i);
            }
        }else {
            std::unordered_set<string> columnsSet(columns.begin(), columns.end());
            for (idx_t i = 0; i < allColumnsNames.size(); ++i) {
                if (columnsSet.contains(StringUtils::normalizeColumnName(allColumnsNames[i]))) {
                    column_ids.push_back(i);
                    sliceCols.push_back(i);
                }else
                    column_ids.push_back(COLUMN_IDENTIFIER_ROW_ID);
            }
        }
        EXPECT_EQ(sliceCols.size(), columns.size());

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

    void testParquetReaderCount(string file, idx_t count, ParquetOptions options = {}, vector<string> columns = {}, vector<idx_t> groups_to_read = {}, TableFilterSet* filters = nullptr) {
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

        // --- Scan + compare chunk-by-chunk ---
        idx_t offset = 0;

        reader.scan(state, chunk);
        while (chunk.getSize()) {
            const idx_t chunk_sz = chunk.getSize();
            offset += chunk_sz;
            chunk.reset();
            reader.scan(state, chunk);
        }

        EXPECT_EQ(offset, count);
    }

};


TEST_F(ParquetScanTest, SimpleParquetScanTest) {
    string file = "t1.parquet";
    testParquetReader(file);
}

TEST_F(ParquetScanTest, SimpleParquetScanOneColTest) {
    string file = "t1.parquet";
    testParquetReader(file, {}, {"I"});
    testParquetReader(file, {}, {"J"});
}

TEST_F(ParquetScanTest, LineItemParquetScanTest) {
    string file = "lineitem-arrow.parquet";
    testParquetReader(file);
}

TEST_F(ParquetScanTest, LineItemParquetSelectScanTest) {
    string file = "lineitem-arrow.parquet";
    testParquetReader(file, {}, {"L_PARTKEY", "L_SUPPKEY", "L_DISCOUNT"});
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

TEST_F(ParquetScanTest, DecimalFilterAllOneColParquetScanTest1) {
    string file = "int32_decimal.parquet";
    auto types = getColumnTypes(file);
    auto tableFilters = std::make_unique<TableFilterSet>();
    auto filter = std::make_unique<ConstantFilter>(Binop::EQUAL,  getConstantValue(1,0, types));
    tableFilters->pushFilter(0, std::move(filter));
    testParquetReader(file, {}, {}, {}, tableFilters.get());
}

TEST_F(ParquetScanTest, DecimalFilterAllOneColParquetScanTest2) {
    string file = "int32_decimal.parquet";
    auto types = getColumnTypes(file);
    auto tableFilters = std::make_unique<TableFilterSet>();
    auto filter = std::make_unique<ConstantFilter>(Binop::EQUAL,  getConstantValue(25,0, types));
    tableFilters->pushFilter(0, std::move(filter));
    // expected no data
    testParquetReaderCount(file, 0, {}, {}, {}, tableFilters.get());
}

TEST_F(ParquetScanTest, TPCHLineItemFiltersParquetScanTest1) {
    string file = "tpch_lineitem.parquet";
    auto types = getColumnTypes(file);
    auto tableFilters = std::make_unique<TableFilterSet>();
    auto filter = std::make_unique<ConstantFilter>(Binop::EQUAL,  getConstantValue(1,0, types));
    tableFilters->pushFilter(0, std::move(filter));
    // expected 122880 (first row group contains first column equal to 1)
    testParquetReaderCount(file, 122880, {}, {}, {}, tableFilters.get());
}

TEST_F(ParquetScanTest, TPCHLineItemFiltersParquetScanTest2) {
    string file = "tpch_lineitem.parquet";
    auto types = getColumnTypes(file);
    auto tableFilters = std::make_unique<TableFilterSet>();
    auto filter = std::make_unique<ConstantFilter>(Binop::LESS,  getConstantValue(1,0, types));
    tableFilters->pushFilter(0, std::move(filter));
    // expected no data
    testParquetReaderCount(file, 0, {}, {}, {}, tableFilters.get());
}

