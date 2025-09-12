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

#include <gtest/gtest.h>

#include "bumblebee/ClientContext.hpp"
#include "bumblebee/common/row_operations/RowOperations.hpp"
#include "bumblebee/common/types/RowDataCollection.hpp"
#include "bumblebee/common/types/Vector.hpp"
#include "bumblebee/common/vector_operations/VectorOperations.hpp"

using namespace bumblebee;

class RowOperationsTest : public ::testing::Test {

public:
    RowOperationsTest():row_collection(*cc.bufferManager_, (idx_t)Storage::BLOCK_SIZE, 1, true) {}

protected:
    ClientContext cc;
    RowDataCollection row_collection;
    vector<buffer_handle_ptr_t> payload_hds;
    vector<data_ptr_t> payload_hds_ptrs;
    idx_t payload_page_offset;


    Vector generateVector(ConstantType type, vector<Value>& values ) {
        Vector v1(type,values.size());
        for (idx_t i = 0; i < values.size(); i++) {
            v1.setValue(i, values[i].cast(v1.getType()));
        }
        return v1;
    }

    DataChunk generateDataChunk(vector<ConstantType>& types, vector<vector<Value>>& data) {
        BB_ASSERT(types.size() == data.size());
        DataChunk chunk;
        chunk.initializeEmpty(types);
        idx_t idx = 0;
        for (auto& data_col : data) {
            Vector vec = generateVector(types[idx], data_col);
            chunk.data_[idx++].reference(vec);
        }
        chunk.setCapacity(data[0].size());
        chunk.setCardinality(data[0].size());
        return chunk;
    }

    template <class T>
    void addData(vector<vector<Value>>& table, vector<T> data) {
        vector<Value> col_data;
        for (auto& d : data) {
            col_data.push_back(d);
        }
        table.push_back(std::move(col_data));
    }

    void newBlock() {
        auto pin = cc.bufferManager_->allocate(Storage::BLOCK_SIZE);
        payload_hds.push_back(std::move(pin));
        payload_hds_ptrs.push_back(payload_hds.back()->ptr());
        payload_page_offset = 0;
    }

    Vector createAddresses(idx_t size, RowLayout& layout) {
        auto tuple_size = layout.getRowWidth();
        auto tuples_per_block = Storage::BLOCK_SIZE / tuple_size;
        Vector addresses(UBIGINT, size);
        auto addresses_ptr = FlatVector::getData<data_ptr_t>(addresses);
        for (idx_t i = 0; i < size; i++) {
            if (payload_page_offset == tuples_per_block || payload_hds.empty()) {
                newBlock();
            }

            auto entry_payload_ptr = payload_hds_ptrs.back() + (payload_page_offset * tuple_size);
            addresses_ptr[i] = entry_payload_ptr;
            ++payload_page_offset;
        }
        return addresses;
    }

};


TEST_F(RowOperationsTest, TestRowOpOneCols) {
    vector<vector<Value>> data;
    addData(data, vector<int>{0,10,20,30});
    vector<ConstantType> types = {INTEGER};

    DataChunk chunk = generateDataChunk(types, data);
    RowLayout layout;
    layout.initialize(types);
    Vector addresses = createAddresses(chunk.getSize(), layout);

    // scatter the data
    SelectionVector sel = FlatVector::INCREMENTAL_SELECTION_VECTOR;
    RowOperations::scatter(chunk, chunk.orrify().get(), layout, addresses, row_collection, sel, chunk.getSize());

    // compare the row vector with the original vector
    idx_t no_match_count = 0;
    SelectionVector matchSel(chunk.getSize());
    for (idx_t i = 0; i < chunk.getSize(); i++) matchSel.setIndex(i,i);
    idx_t count = RowOperations::equal(chunk, chunk.orrify().get(), layout, addresses, matchSel, chunk.getSize(),nullptr, no_match_count);
    EXPECT_EQ(count, chunk.getSize());

    // compare with address vector ( all values should not match)
    SelectionVector noMatchSel(chunk.getSize());
    DataChunk addresesChunk;
    addresesChunk.initializeEmpty({UBIGINT});
    addresesChunk.data_[0].reference(addresses);
    for (idx_t i = 0; i < chunk.getSize(); i++) matchSel.setIndex(i,i);
    count = RowOperations::equal(addresesChunk, addresesChunk.orrify().get(), layout, addresses, matchSel, addresesChunk.getSize(),&noMatchSel, no_match_count);
    EXPECT_EQ(no_match_count, addresesChunk.getSize());
    EXPECT_EQ(count, 0);


    Vector col(types[0]);
    RowOperations::gather(addresses, sel, col, sel, chunk.getSize(), layout.getOffsets()[0]);

    // compare the original vector to the gather vector
    count = VectorOperations::equals(chunk.data_[0], col, nullptr, chunk.getSize(),nullptr);
    EXPECT_EQ(count, chunk.getSize());
}



TEST_F(RowOperationsTest, TestRowOpMultiCols) {
    vector<ConstantType> types = {UTINYINT, USMALLINT, INTEGER};
    vector<vector<Value>> data;
    int N = 50000;
    for (idx_t i = 0; i < types.size(); i++) {
        vector<int> generated_data;
        for (idx_t j = 0; j < N; j++) {
            generated_data.push_back(j*i*10);
        }
        addData(data, generated_data);
    }

    DataChunk chunk = generateDataChunk(types, data);
    RowLayout layout;
    layout.initialize(types);
    Vector addresses = createAddresses(chunk.getSize(), layout);

    // scatter the data
    SelectionVector sel = FlatVector::INCREMENTAL_SELECTION_VECTOR;
    RowOperations::scatter(chunk, chunk.orrify().get(), layout, addresses, row_collection, sel, chunk.getSize());

    // compare the row vector with the original vector
    SelectionVector matchSel(chunk.getSize());
    for (idx_t i = 0; i < chunk.getSize(); i++) matchSel.setIndex(i,i);
    SelectionVector noMatchSel(chunk.getSize());
    idx_t no_match_count = 0;
    idx_t count = RowOperations::equal(chunk, chunk.orrify().get(), layout, addresses, matchSel, chunk.getSize(),&noMatchSel, no_match_count);
    EXPECT_EQ(no_match_count, 0);
    EXPECT_EQ(count, chunk.getSize());


    // gather and compare columns
    for (idx_t i = 0; i < chunk.columnCount(); i++) {
        Vector col(types[i], chunk.getSize());
        RowOperations::gather(addresses, sel, col, sel, chunk.getSize(), layout.getOffsets()[i]);

        // compare the original vector to the gather vector
        count = VectorOperations::equals(chunk.data_[i], col, nullptr, chunk.getSize(),nullptr);
        EXPECT_EQ(count, chunk.getSize());
    }
}


TEST_F(RowOperationsTest, TestRowOpString) {
    vector<ConstantType> types = {STRING};
    vector<vector<Value>> data;
    int N = 1000;
    for (idx_t i = 0; i < types.size(); i++) {
        vector<string> generated_data;
        for (idx_t j = 0; j < N; j++) {
            generated_data.push_back("MIAO_"+std::to_string(j));
        }
        addData(data, generated_data);
    }

    DataChunk chunk = generateDataChunk(types, data);
    RowLayout layout;
    layout.initialize(types);
    Vector addresses = createAddresses(chunk.getSize(), layout);

    // scatter the data
    SelectionVector sel = FlatVector::INCREMENTAL_SELECTION_VECTOR;
    RowOperations::scatter(chunk, chunk.orrify().get(), layout, addresses, row_collection, sel, chunk.getSize());

    // compare the row vector with the original vector
    SelectionVector matchSel(chunk.getSize());
    for (idx_t i = 0; i < chunk.getSize(); i++) matchSel.setIndex(i,i);
    SelectionVector noMatchSel(chunk.getSize());
    idx_t no_match_count = 0;
    idx_t count = RowOperations::equal(chunk, chunk.orrify().get(), layout, addresses, matchSel, chunk.getSize(),&noMatchSel, no_match_count);
    EXPECT_EQ(no_match_count, 0);
    EXPECT_EQ(count, chunk.getSize());


    // gather and compare columns
    for (idx_t i = 0; i < chunk.columnCount(); i++) {
        Vector col(types[i], chunk.getSize());
        RowOperations::gather(addresses, sel, col, sel, chunk.getSize(), layout.getOffsets()[i]);

        // compare the original vector to the gather vector
        count = VectorOperations::equals(chunk.data_[i], col, nullptr, chunk.getSize(),nullptr);
        EXPECT_EQ(count, chunk.getSize());
    }
}