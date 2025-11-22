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

#include "bumblebee/function/AggregateFunction.hpp"
#include "bumblebee/function/aggregate/Sum.hpp"
#include "vector"
#include "../BumbleBaseTest.hpp"

template <class T>
using unique_ptr = std::unique_ptr<T>;

namespace bumblebee {

class AggFuncTest : public BumbleBaseTest {
public:
    unique_ptr<data_t[]> generateStates(idx_t size) {
        unique_ptr<data_t[]> states = unique_ptr<data_t[]>(new data_t[size]);
        memset(states.get(), 0, size);
        return states;
    }



    unique_ptr<data_t[]> generateStates(RowLayout& layout, Vector& addresses,  idx_t size) {
        unique_ptr<data_t[]> rowdata = unique_ptr<data_t[]>(new data_t[layout.getRowWidth() * size]);
        addresses.initialize(size);
        BB_ASSERT(addresses.getType() == UBIGINT);
        BB_ASSERT(addresses.getVectorType() == VectorType::FLAT_VECTOR);
        auto data = FlatVector::getData<data_ptr_t>(addresses);
        for (idx_t i = 0; i < size; i++)
            data[i] = rowdata.get() + (layout.getRowWidth() * i);

        return rowdata;
    }
};



TEST_F(AggFuncTest, InitRowSumStateTest ) {

    vector args = {SMALLINT};
    auto aggFunction = (AggregateFunction&) *SumFunc::getFunction(args[0]).get();
    idx_t count = 10;

    vector types = {SMALLINT, SMALLINT};
    RowLayout layout;
    layout.initialize(types, {&aggFunction}, true);
    Vector addresses(UBIGINT, count);

    SelectionVector sel = FlatVector::INCREMENTAL_SELECTION_VECTOR;
    auto row_data = generateStates(layout, addresses, count);
    AggregateFunction::initStates(layout, addresses, sel, count);
    auto pointers = FlatVector::getData<data_ptr_t>(addresses);
    // check all states are 0
    auto agg_offset = layout.getOffsets()[layout.columnCount()];
    for (idx_t i = 0; i < count; i++) {
        auto row = pointers[i];
        EXPECT_EQ(row, row_data.get() + (layout.getRowWidth() * i));
        auto state = (SumState<int64_t>*)(row + agg_offset);
        EXPECT_EQ(state->value, 0);
    }
}


TEST_F(AggFuncTest, UpdateRowSumStateTest ) {

    vector args = {SMALLINT};
    auto aggFunction = (AggregateFunction&) *SumFunc::getFunction(args[0]).get();
    idx_t n = 10;

    // init the data
    vector types = {BIGINT, SMALLINT};
    RowLayout layout;
    layout.initialize(types, {&aggFunction}, true);
    Vector addresses(UBIGINT, n);


    SelectionVector sel = FlatVector::INCREMENTAL_SELECTION_VECTOR;
    auto row_data = generateStates(layout, addresses, n);
    AggregateFunction::initStates(layout, addresses, sel, n);
    auto pointers = FlatVector::getData<data_ptr_t>(addresses);

    // update the states with Input vector
    Vector input = generateVector(n, args[0]);
    AggregateFunction::updateStates(layout, addresses, input, n, 0);

    // check all states are 0
    auto agg_offset = layout.getOffsets()[layout.columnCount()];
    for (idx_t i = 0; i < n; i++) {
        auto row = pointers[i];
        EXPECT_EQ(row, row_data.get() + (layout.getRowWidth() * i));
        auto state = (SumState<int64_t>*)(row + agg_offset);
        EXPECT_EQ(state->value, i);
    }
}




TEST_F(AggFuncTest, FinalizeRowSumStateTest ) {
    vector args = {SMALLINT};
    auto aggFunction = (AggregateFunction&) *SumFunc::getFunction(args[0]).get();
    idx_t n = 20;

    // init the data
    vector types = {BIGINT, SMALLINT};
    RowLayout layout;
    layout.initialize(types, {&aggFunction}, true);
    Vector addresses(UBIGINT, n);


    SelectionVector sel = FlatVector::INCREMENTAL_SELECTION_VECTOR;
    auto row_data = generateStates(layout, addresses, n);
    AggregateFunction::initStates(layout, addresses, sel, n);
    auto pointers = FlatVector::getData<data_ptr_t>(addresses);

    // update the states with Input vector
    Vector input = generateVector(n, args[0]);
    AggregateFunction::updateStates(layout, addresses, input, n, 0);
    // update again first 10 elements
    AggregateFunction::updateStates(layout, addresses, input, 10, 0);

    // Fetch the results
    DataChunk result;
    result.initialize({aggFunction.result_});
    AggregateFunction::finalizeStates(layout, addresses, result, n);

    // check results
    auto& resultVec = result.data_[0];
    for (idx_t i = 0; i < n; i++) {
        if (i < 10) // expected double value
            EXPECT_EQ(resultVec.getValue(i).getNumericValue<uint64_t>(), i*2);
        else
            EXPECT_EQ(resultVec.getValue(i).getNumericValue<uint64_t>(), i);
    }
}




}