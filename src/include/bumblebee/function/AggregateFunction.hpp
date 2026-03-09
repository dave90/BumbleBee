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
#include "Function.hpp"
#include "bumblebee/common/TypeDefs.hpp"
#include "bumblebee/common/types/DataChunk.hpp"
#include "bumblebee/common/types/RowLayout.hpp"
#include "bumblebee/common/types/Vector.hpp"

namespace bumblebee{

// The type used for sizing hashed aggregate function states
typedef idx_t (*aggregate_size_t)();
// The type used for initializing hashed aggregate function states
typedef void (*aggregate_initialize_t)(data_ptr_t state);
// The type used for updating hashed aggregate functions
typedef void (*aggregate_update_t)(data_ptr_t input, data_ptr_t state);
// The type used for combining hashed aggregate states (optional)
typedef void (*aggregate_combine_t)(data_ptr_t state, data_ptr_t combined);
// The type used for finalizing hashed aggregate function payloads
typedef void (*aggregate_finalize_t)(data_ptr_t state, data_ptr_t result);

using agg_states_ptr = std::unique_ptr<data_t[]> ;

class AggregateFunction : public Function {
public:
    AggregateFunction(const string &name,const vector<LogicalType> &arguments, LogicalType result,
        aggregate_size_t state_size, aggregate_initialize_t initialize, aggregate_update_t update,
        aggregate_combine_t combine, aggregate_finalize_t finalize)
        : Function(name, arguments, result),
          stateSize_(state_size),
          initialize_(initialize),
          update_(update),
          combine_(combine),
          finalize_(finalize) {
    }

    AggregateFunction(const AggregateFunction &other)
        : Function(other),
          stateSize_(other.stateSize_),
          initialize_(other.initialize_),
          update_(other.update_),
          combine_(other.combine_),
          finalize_(other.finalize_) {
    }

    AggregateFunction & operator=(const AggregateFunction &other) {
        if (this == &other)
            return *this;
        Function::operator =(other);
        stateSize_ = other.stateSize_;
        initialize_ = other.initialize_;
        update_ = other.update_;
        combine_ = other.combine_;
        finalize_ = other.finalize_;
        return *this;
    }

    AggregateFunction & operator=(AggregateFunction &&other) noexcept {
        if (this == &other)
            return *this;
        Function::operator =(std::move(other));
        stateSize_ = other.stateSize_;
        initialize_ = other.initialize_;
        update_ = other.update_;
        combine_ = other.combine_;
        finalize_ = other.finalize_;
        return *this;
    }

    friend bool operator==(const AggregateFunction &lhs, const AggregateFunction &rhs) {
        return lhs.stateSize_ == rhs.stateSize_
               && lhs.initialize_ == rhs.initialize_
               && lhs.update_ == rhs.update_
               && lhs.combine_ == rhs.combine_
               && lhs.finalize_ == rhs.finalize_;
    }

    friend bool operator!=(const AggregateFunction &lhs, const AggregateFunction &rhs) {
        return !(lhs == rhs);
    }

    aggregate_size_t stateSize_;
    aggregate_initialize_t initialize_;
    aggregate_update_t update_;
    aggregate_combine_t combine_;
    aggregate_finalize_t finalize_;


public:

    template <template <typename> class STATE, class INPUT_TYPE, class RESULT_TYPE, template <typename,typename> class OP>
    static AggregateFunction unaryAggregate(const string &name,const vector<LogicalType> &arguments, LogicalType result) {
        return AggregateFunction(name, arguments, result,
            stateSize<STATE<RESULT_TYPE>>,
            stateInitialize<STATE<RESULT_TYPE>, OP<INPUT_TYPE,RESULT_TYPE>>,
            stateUpdate<INPUT_TYPE, STATE<RESULT_TYPE>, OP<INPUT_TYPE,RESULT_TYPE>>,
            stateCombine<STATE<RESULT_TYPE>, OP<INPUT_TYPE,RESULT_TYPE>>,
            stateFinalize<RESULT_TYPE, STATE<RESULT_TYPE>, OP<INPUT_TYPE,RESULT_TYPE>>);
    }

protected:
    template <class STATE>
    static idx_t stateSize() {
        return sizeof(STATE);
    }


    template <class STATE, class OP>
    static void stateInitialize(data_ptr_t state) {
        OP::initialize((STATE *)state);
    }

    template <class STATE, class OP>
    static void stateCombine(data_ptr_t state,data_ptr_t target ) {
        OP::combine((STATE *)state, (STATE *)target);
    }

    template <class INPUT_TYPE, class STATE, class OP>
    static void stateUpdate(data_ptr_t input, data_ptr_t state ) {
        OP::operation((INPUT_TYPE *)input, (STATE *)state);
    }

    template <class RESULT_TYPE, class STATE, class OP>
    static void stateFinalize(data_ptr_t state, data_ptr_t result ) {
        OP::finalize((STATE *)state, (RESULT_TYPE *)result);
    }


public:
    // vector operations
    static void initStates(data_ptr_t states, const SelectionVector& sel, AggregateFunction& func, idx_t count);
    static void combineStates(data_ptr_t states, data_ptr_t targetStates, const SelectionVector& sel, const SelectionVector& targetSel, AggregateFunction& func, idx_t count);
    static void updateStates(Vector& input, data_ptr_t states, const SelectionVector& sel, AggregateFunction& func, idx_t count);
    static void finalizeStates(Vector& result, data_ptr_t states, const SelectionVector& sel, AggregateFunction& func, idx_t count);

    // row operation
    static void initStates(RowLayout &layout, Vector &addresses, const SelectionVector &sel, idx_t count);
    static void updateStates(RowLayout &layout, Vector &addresses, Vector &payload, idx_t count, idx_t agg_idx);
    static void combineStates(RowLayout &layout, Vector &sources, Vector &targets, const SelectionVector &sel, idx_t count);
    static void finalizeStates(RowLayout &layout, Vector &addresses, DataChunk &result, idx_t count);
    static void finalizeStates(RowLayout &layout, Vector &addresses, Vector& result, idx_t agg_index, idx_t count);

};

}
