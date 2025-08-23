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

#include "bumblebee/function/AggregateFunction.h"
#include "bumblebee/function/aggregate/Sum.h"
#include "vector"

template <class T>
using unique_ptr = std::unique_ptr<T>;

namespace bumblebee {

class AggFuncTest : public ::testing::Test {
public:
    unique_ptr<data_t[]> generateStates(idx_t size) {
        unique_ptr<data_t[]> states = unique_ptr<data_t[]>(new data_t[size]);
        memset(states.get(), 0, size);
        return states;
    }

    Vector generateVector(idx_t size, ConstantType type) {
        Vector result(type);
        for (idx_t i = 0; i < size; i++)
            result.setValue(i, Value((uint64_t)i).cast(type) );

        return result;
    }
};


TEST_F(AggFuncTest, InitSumStateTest ) {

    vector args = {SMALLINT};
    auto aggFunction = (AggregateFunction&) *SumFunc::getFunction(args[0]).get();

    auto stateSize = aggFunction.stateSize_();
    auto n = 10;
    auto size = n*stateSize;

    unique_ptr<data_t[]> states = generateStates(size);
    SelectionVector sel = FlatVector::INCREMENTAL_SELECTION_VECTOR;
    AggregateFunction::initStates(states.get(), sel, aggFunction, n);
    // check all states are 0
    for (idx_t i = 0; i < n; i++) {
        auto state = (SumState<int64_t>*) (states.get() + i*stateSize);
        EXPECT_EQ(state->value, 0);
    }
}


TEST_F(AggFuncTest, UpdateSumStateTest ) {

    vector args = {SMALLINT};
    auto aggFunction = (AggregateFunction&) *SumFunc::getFunction(args[0]).get();

    // init the state
    auto stateSize = aggFunction.stateSize_();
    auto n = 10;
    auto size = n*stateSize;
    unique_ptr<data_t[]> states = generateStates(size);

    SelectionVector sel = FlatVector::INCREMENTAL_SELECTION_VECTOR;
    AggregateFunction::initStates(states.get(), sel, aggFunction, n);

    // update the states with Input vector
    Vector input = generateVector(n, args[0]);
    AggregateFunction::updateState(input, states.get(), sel, aggFunction, n);

    // check the states
    for (idx_t i = 0; i < n; i++) {
        auto state = (SumState<int64_t>*) (states.get() + i*stateSize);
        EXPECT_EQ(state->value, i);
    }
}



TEST_F(AggFuncTest, FinalizeSumStateTest ) {

    vector args = {USMALLINT};
    auto result = UBIGINT;
    auto aggFunction = (AggregateFunction&) *SumFunc::getFunction(args[0]).get();

    auto stateSize = aggFunction.stateSize_();
    auto n = 10;
    auto size = n*stateSize;
    unique_ptr<data_t[]> states = generateStates(size);

    SelectionVector sel = FlatVector::INCREMENTAL_SELECTION_VECTOR;
    AggregateFunction::initStates(states.get(), sel, aggFunction, n);

    // update the states with Input vector
    Vector input = generateVector(n, args[0]);
    AggregateFunction::updateState(input, states.get(), sel, aggFunction, n);

    // update the 10 states again
    AggregateFunction::updateState(input, states.get(), sel, aggFunction, 10);

    // Fetch the results
    Vector resultVec(result);
    AggregateFunction::finalizeState(resultVec, states.get(), sel, aggFunction, n);

    // check results
    for (idx_t i = 0; i < n; i++) {
        if (i < 10) // expected double value
            EXPECT_EQ(resultVec.getValue(i).getNumericValue<uint64_t>(), i*2);
        else
            EXPECT_EQ(resultVec.getValue(i).getNumericValue<uint64_t>(), i);
    }
}


TEST_F(AggFuncTest, FinalizeSumMultipleindexStateTest ) {

    vector args = {USMALLINT};
    auto result = UBIGINT;
    auto aggFunction = (AggregateFunction&) *SumFunc::getFunction(args[0]).get();

    auto stateSize = aggFunction.stateSize_();
    auto n = 25;
    auto size = n*stateSize;
    unique_ptr<data_t[]> states = generateStates(size);

    SelectionVector sel = FlatVector::INCREMENTAL_SELECTION_VECTOR;
    AggregateFunction::initStates(states.get(), sel, aggFunction, n);

    // update only first state
    SelectionVector selZero = ConstantVector::ZERO_SELECTION_VECTOR;
    Vector input = generateVector(n, args[0]);
    AggregateFunction::updateState(input, states.get(), selZero, aggFunction, n);

    // fetch result
    Vector resultVec(result);
    AggregateFunction::finalizeState(resultVec, states.get(), sel, aggFunction, n);

    std::cout << resultVec.toString(n) << std::endl;
    // check all states 0 (except first)
    for (idx_t i = 1; i < n; i++) {
        EXPECT_EQ(resultVec.getValue(i).getNumericValue<uint64_t>(), 0);
    }
    // first value should be
    EXPECT_EQ(resultVec.getValue(0).getNumericValue<uint64_t>(), (n-1)*(n)/2); // sum of first n numbers (is n-1 because first is 0)

}


}