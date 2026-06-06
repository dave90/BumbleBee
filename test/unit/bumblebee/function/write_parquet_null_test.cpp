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

#include "bumblebee/ClientContext.hpp"
#include "bumblebee/common/TypeDefs.hpp"
#include "bumblebee/common/types/DataChunk.hpp"
#include "bumblebee/common/types/Value.hpp"
#include "bumblebee/function/predicate/ReadParquet.hpp"
#include "bumblebee/function/predicate/WriteParquet.hpp"
#include "bumblebee/planner/filter/TableFilter.hpp"

using namespace bumblebee;

// Round-trips NULL-bearing chunks through write->read parquet, exercising both
// the writer (real definition levels + skipped payload) and the reader
// (definition levels landed into the ValidityMask).
class WriteParquetNullTest : public ::testing::Test {
protected:
    ClientContext context;

    string getOutputPath(const string& name) {
        std::filesystem::path base = __FILE__;
        return (base.parent_path() / "data" / "output" / name).string();
    }

    string getInputParquetPath(const string& filename) {
        std::filesystem::path base = __FILE__;
        return (base.parent_path() / "data" / "input" / "parquet" / filename).string();
    }

    void cleanSingle(const string& path) {
        if (context.fileSystem_->fileExists(path))
            context.fileSystem_->removeFile(path);
        string base = path;
        if (base.ends_with(".parquet"))
            base.erase(base.size() - string(".parquet").size());
        string tmp = base + ".tmp";
        if (context.fileSystem_->fileExists(tmp))
            context.fileSystem_->removeFile(tmp);
    }

    // Write one or more in-memory chunks to a single parquet file at outPath.
    void writeChunks(const string& outPath, const vector<DataChunk*>& chunks,
                     const vector<LogicalType>& types, const vector<string>& names) {
        vector<Value> input;
        input.emplace_back(outPath);
        vector<LogicalType> inputTypes = {LogicalTypeId::STRING};
        TableFilterSet filters;
        auto functionPtr = WriteParquetFunc().getFunction({});
        auto& pred = (PredFunction&)*functionPtr;
        std::unordered_map<string, Value> params;
        params["codec"] = string("UNCOMPRESSED");
        params["single_file"] = uint8_t(1);
        std::unordered_map<string, idx_t> bindVarName;
        for (idx_t i = 0; i < names.size(); ++i) bindVarName[names[i]] = i;
        auto returnTypes = types;
        auto namesCopy = names;
        auto bind = pred.bindFunction_(context, input, inputTypes, params,
                                       bindVarName, returnTypes, namesCopy, filters);
        auto fopd = pred.initFunction_(context, bind.get());
        for (auto* ch : chunks)
            pred.function_(context, bind.get(), fopd.get(), nullptr, *ch);
        pred.combine_function_(context, bind.get(), fopd.get());
        pred.finalize_function_(context, bind.get());
    }

    void writeChunk(const string& outPath, DataChunk& chunk,
                    const vector<LogicalType>& types, const vector<string>& names) {
        writeChunks(outPath, {&chunk}, types, names);
    }

    // Read all rows of a single parquet file into one merged chunk vector.
    vector<data_chunk_ptr_t> readFile(const string& path, const vector<string>& names,
                                      vector<LogicalType>& outTypes) {
        vector<Value> input;
        input.emplace_back(path);
        vector<LogicalType> inputTypes = {LogicalTypeId::STRING};
        TableFilterSet filters;
        auto functionPtr = ReadParquetFunc().getFunction({});
        auto& pred = (PredFunction&)*functionPtr;
        std::unordered_map<string, Value> params;
        vector<LogicalType> returnTypes;
        std::unordered_map<string, idx_t> bindVarName;
        auto namesCopy = names;
        auto bind = pred.bindFunction_(context, input, inputTypes, params,
                                       bindVarName, returnTypes, namesCopy, filters);
        outTypes = returnTypes;
        vector<data_chunk_ptr_t> chunks;
        idx_t numThreads = pred.maxThreadFunction_(context, bind.get());
        for (idx_t t = 0; t < numThreads; ++t) {
            auto fopd = pred.initFunction_(context, bind.get());
            DataChunk chunk;
            chunk.initialize(returnTypes);
            chunk.setCardinality(0);
            pred.function_(context, bind.get(), fopd.get(), nullptr, chunk);
            while (chunk.getSize() > 0) {
                chunks.push_back(chunk.clone());
                chunk.setCardinality(0);
                pred.function_(context, bind.get(), fopd.get(), nullptr, chunk);
            }
        }
        return chunks;
    }
};

// INTEGER / STRING / DOUBLE columns with NULLs scattered across rows and
// columns survive a write->read round-trip: NULL cells read back NULL, the
// rest read back their exact value.
TEST_F(WriteParquetNullTest, MixedNullsRoundTrip) {
    auto outPath = getOutputPath("Out-Parquet-Null-Mixed.parquet");
    cleanSingle(outPath);

    const idx_t N = 7;
    vector<LogicalType> types = {LogicalTypeId::INTEGER, LogicalTypeId::STRING, LogicalTypeId::DOUBLE};
    vector<string> names = {"A", "B", "C"};
    DataChunk chunk;
    chunk.initialize(types);
    chunk.setCardinality(N);
    for (idx_t i = 0; i < N; i++) {
        chunk.data_[0].setValue(i, Value((int32_t)(i * 10)));
        chunk.data_[1].setValue(i, Value("s" + std::to_string(i)));
        chunk.data_[2].setValue(i, Value((double)i + 0.5));
    }
    // NULLs: col A rows {1,4}, col B rows {0,4}, col C rows {4,6}. Row 4 all-null.
    chunk.data_[0].setValue(1, Value::null());
    chunk.data_[0].setValue(4, Value::null());
    chunk.data_[1].setValue(0, Value::null());
    chunk.data_[1].setValue(4, Value::null());
    chunk.data_[2].setValue(4, Value::null());
    chunk.data_[2].setValue(6, Value::null());

    writeChunk(outPath, chunk, types, names);

    vector<LogicalType> rtypes;
    auto out = readFile(outPath, names, rtypes);

    idx_t total = 0;
    for (auto& c : out) total += c->getSize();
    ASSERT_EQ(total, N);

    // Flatten into a single logical view for assertions.
    ASSERT_EQ(out.size(), 1u);
    auto& c = *out[0];
    auto expectNull = [&](idx_t col, idx_t row) {
        EXPECT_TRUE(c.getValue(col, row).isNull()) << "col " << col << " row " << row;
    };
    auto expectInt = [&](idx_t row, int32_t v) {
        auto val = c.getValue(0, row);
        EXPECT_FALSE(val.isNull());
        EXPECT_EQ(val.cast(PhysicalType::INTEGER).getValueUnsafe<int32_t>(), v) << "row " << row;
    };

    expectNull(0, 1); expectNull(0, 4);
    expectNull(1, 0); expectNull(1, 4);
    expectNull(2, 4); expectNull(2, 6);

    expectInt(0, 0);
    expectInt(2, 20);
    EXPECT_FALSE(c.getValue(1, 1).isNull());
    EXPECT_FALSE(c.getValue(2, 0).isNull());
}

// >5000 rows so it spans the STANDARD_VECTOR_SIZE chunk boundary; every 17th
// row is NULL in two columns. Verifies definition-level packing across chunks.
TEST_F(WriteParquetNullTest, MultiBatchNullsRoundTrip) {
    auto outPath = getOutputPath("Out-Parquet-Null-Multi.parquet");
    cleanSingle(outPath);

    const idx_t N = 5300;
    vector<LogicalType> types = {LogicalTypeId::BIGINT, LogicalTypeId::STRING};
    vector<string> names = {"K", "V"};

    // Split into chunks of at most STANDARD_VECTOR_SIZE rows (a DataChunk caps
    // there); the writer accumulates them into one parquet file.
    vector<std::unique_ptr<DataChunk>> owned;
    vector<DataChunk*> chunkPtrs;
    idx_t produced = 0;
    while (produced < N) {
        idx_t batch = std::min<idx_t>(STANDARD_VECTOR_SIZE, N - produced);
        auto c = std::make_unique<DataChunk>();
        c->initialize(types);
        c->setCardinality(batch);
        for (idx_t r = 0; r < batch; r++) {
            idx_t i = produced + r;
            c->data_[0].setValue(r, Value((int64_t)i));
            c->data_[1].setValue(r, Value("v" + std::to_string(i)));
            if (i % 17 == 0) {
                c->data_[0].setValue(r, Value::null());
                c->data_[1].setValue(r, Value::null());
            }
        }
        chunkPtrs.push_back(c.get());
        owned.push_back(std::move(c));
        produced += batch;
    }

    writeChunks(outPath, chunkPtrs, types, names);

    vector<LogicalType> rtypes;
    auto out = readFile(outPath, names, rtypes);

    // Reassemble logical rows in order across chunks.
    idx_t row = 0;
    for (auto& c : out) {
        for (idx_t r = 0; r < c->getSize(); r++, row++) {
            bool shouldNull = (row % 17 == 0);
            EXPECT_EQ(c->getValue(0, r).isNull(), shouldNull) << "K row " << row;
            EXPECT_EQ(c->getValue(1, r).isNull(), shouldNull) << "V row " << row;
            if (!shouldNull) {
                EXPECT_EQ(c->getValue(0, r).cast(PhysicalType::BIGINT).getValueUnsafe<int64_t>(),
                          (int64_t)row) << "K row " << row;
            }
        }
    }
    EXPECT_EQ(row, N);
}

// Backward-compat: an existing all-valid parquet file reads back with no
// spurious NULLs (the reader's mask reset + def-level path must not introduce
// nulls when maxDefine is met on every row).
TEST_F(WriteParquetNullTest, ExistingAllValidFileHasNoNulls) {
    vector<LogicalType> rtypes;
    auto out = readFile(getInputParquetPath("data_zstd.parquet"),
                        {"COL1", "COL2", "COL3"}, rtypes);
    ASSERT_FALSE(out.empty());
    idx_t total = 0;
    for (auto& c : out) {
        for (idx_t r = 0; r < c->getSize(); r++) {
            for (idx_t col = 0; col < c->columnCount(); col++) {
                EXPECT_FALSE(c->getValue(col, r).isNull())
                    << "unexpected NULL at col " << col << " row " << r;
            }
        }
        total += c->getSize();
    }
    EXPECT_GT(total, 0u);
}