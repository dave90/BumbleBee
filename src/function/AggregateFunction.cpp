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

#include "bumblebee/function/AggregateFunction.hpp"

#include "bumblebee/parser/statement/Term.hpp"

namespace bumblebee {

/* --------------------------------------------------------------------
 *                  VECTOR LAYOUT
 *--------------------------------------------------------------------
*/

void AggregateFunction::initStates(data_ptr_t states, const SelectionVector& sel, AggregateFunction &func, idx_t count) {
    auto size = func.stateSize_();
    for (idx_t i = 0; i < count; i++) {
        idx_t index = sel.getIndex(i);
        uint8_t* state = states + index * size;
        func.initialize_(state);
    }
}

void AggregateFunction::combineStates(data_ptr_t states, data_ptr_t targetStates,const SelectionVector &sel, const SelectionVector &targetSel,
    AggregateFunction &func, idx_t count) {

    auto size = func.stateSize_();
    for (idx_t i = 0; i < count; i++) {
        idx_t index = sel.getIndex(i);
        idx_t tindex = targetSel.getIndex(i);
        uint8_t* state = states + index * size;
        uint8_t* targetState = targetStates + tindex * size;
        func.combine_(state, targetState);
    }

}

template <class T>
    void templatedUpdateState(Vector& input, data_ptr_t states, const SelectionVector &sel, AggregateFunction &func,
        idx_t count) {

    VectorData input_data;
    input.orrify(count, input_data);
    T* data = (T*)input_data.data_;

    auto state_size = func.stateSize_();
    for (idx_t i = 0; i < count; i++) {
        idx_t state_index = sel.getIndex(i);
        idx_t input_index = input_data.sel_->getIndex(i);

        func.update_( (data_ptr_t)(data + input_index), states + state_size * state_index);
    }
}

void AggregateFunction::updateStates(Vector& input, data_ptr_t states, const SelectionVector &sel, AggregateFunction &func,
    idx_t count) {

    switch (input.getType()) {
        case PhysicalType::TINYINT:
            return templatedUpdateState<int8_t>(input, states, sel, func, count);
        case PhysicalType::SMALLINT:
            return templatedUpdateState<int16_t>(input, states, sel, func, count);
        case PhysicalType::INTEGER:
            return templatedUpdateState<int32_t>(input, states, sel, func, count);
        case PhysicalType::BIGINT:
            return templatedUpdateState<int64_t>(input, states, sel, func, count);
        case PhysicalType::UTINYINT:
            return templatedUpdateState<uint8_t>(input, states, sel, func, count);
        case PhysicalType::USMALLINT:
            return templatedUpdateState<uint16_t>(input, states, sel, func, count);
        case PhysicalType::UINTEGER:
            return templatedUpdateState<uint32_t>(input, states, sel, func, count);
        case PhysicalType::UBIGINT:
            return templatedUpdateState<uint64_t>(input, states, sel, func, count);
        case PhysicalType::FLOAT:
            return templatedUpdateState<float>(input, states, sel, func, count);
        case PhysicalType::DOUBLE:
            return templatedUpdateState<double>(input, states, sel, func, count);
        case PhysicalType::STRING:	{
            return templatedUpdateState<string_t>(input, states, sel, func, count);
        }
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for select operation!");
    }
}

template <class T>
void templatedFinalizeState(Vector &result, data_ptr_t states, const SelectionVector &sel, AggregateFunction &func,
        idx_t count) {
    VectorData result_data;
    result.orrify(count, result_data);
    T* data = (T*)result_data.data_;

    auto state_size = func.stateSize_();
    for (idx_t i = 0; i < count; i++) {
        idx_t state_index = sel.getIndex(i);
        idx_t input_index = result_data.sel_->getIndex(state_index);

        func.finalize_( states + state_size * state_index, (data_ptr_t)(data + input_index));
    }
}


void AggregateFunction::finalizeStates(Vector &result, data_ptr_t states, const SelectionVector &sel, AggregateFunction &func,
    idx_t count) {

    switch (result.getType()) {
        case PhysicalType::TINYINT:
            return templatedFinalizeState<int8_t>(result, states, sel, func, count);
        case PhysicalType::SMALLINT:
            return templatedFinalizeState<int16_t>(result, states, sel, func, count);
        case PhysicalType::INTEGER:
            return templatedFinalizeState<int32_t>(result, states, sel, func, count);
        case PhysicalType::BIGINT:
            return templatedFinalizeState<int64_t>(result, states, sel, func, count);
        case PhysicalType::UTINYINT:
            return templatedFinalizeState<uint8_t>(result, states, sel, func, count);
        case PhysicalType::USMALLINT:
            return templatedFinalizeState<uint16_t>(result, states, sel, func, count);
        case PhysicalType::UINTEGER:
            return templatedFinalizeState<uint32_t>(result, states, sel, func, count);
        case PhysicalType::UBIGINT:
            return templatedFinalizeState<uint64_t>(result, states, sel, func, count);
        case PhysicalType::FLOAT:
            return templatedFinalizeState<float>(result, states, sel, func, count);
        case PhysicalType::DOUBLE:
            return templatedFinalizeState<double>(result, states, sel, func, count);
        case PhysicalType::STRING:	{
            return templatedFinalizeState<string_t>(result, states, sel, func, count);
        }
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for select operation!");
    }
}


/* --------------------------------------------------------------------
 *                  ROW LAYOUT
 *--------------------------------------------------------------------
*/

void AggregateFunction::initStates(RowLayout &layout, Vector &addresses, const SelectionVector &sel, idx_t count) {
    if (count == 0) {
        return;
    }
    BB_ASSERT(addresses.getVectorType() == VectorType::FLAT_VECTOR);
    auto pointers = FlatVector::getData<data_ptr_t>(addresses);
    auto &offsets = layout.getOffsets();
    auto aggr_idx = layout.columnCount();

    for (AggregateFunction* aggr : layout.getAggregates()) {
        for (idx_t i = 0; i < count; ++i) {
            auto row_idx = sel.getIndex(i);
            auto row = pointers[row_idx];
            aggr->initialize_(row + offsets[aggr_idx]);
        }
        ++aggr_idx;
    }
}

template <class INPUT_TYPE>
void templatedUpdateStateFlatLoop(AggregateFunction &aggr, INPUT_TYPE* __restrict idata, data_ptr_t* __restrict sdata, idx_t agg_offset, idx_t count) {
    for (idx_t i = 0; i < count; ++i) {
        auto row = sdata[i];
        aggr.update_((data_ptr_t)(idata +i), row + agg_offset);
    }
}

template <class INPUT_TYPE>
void templatedUpdateStateLoop(AggregateFunction &aggr, INPUT_TYPE* __restrict idata, data_ptr_t* __restrict sdata,const SelectionVector& sidata,const SelectionVector& ssdata, idx_t agg_offset, idx_t count) {
    for (idx_t i = 0; i < count; ++i) {
        auto idx = sidata.getIndex(i);
        auto row_idx = ssdata.getIndex(i);
        auto row = sdata[row_idx];
        aggr.update_((data_ptr_t)(idata +idx), row + agg_offset);
    }
}

template <class INPUT_TYPE>
void templatedUpdateState(RowLayout &layout, AggregateFunction &aggr, Vector &addresses, Vector &input, idx_t count, idx_t agg_idx) {

    auto agg_offset = layout.getOffsets()[ layout.columnCount() + agg_idx];
    if (input.getVectorType() == VectorType::CONSTANT_VECTOR &&
            addresses.getVectorType() == VectorType::CONSTANT_VECTOR) {

        // regular constant: get first state
        auto idata = ConstantVector::getData<INPUT_TYPE>(input);
        auto sdata = ConstantVector::getData<data_ptr_t>(addresses);
        aggr.update_( (data_ptr_t)(idata), sdata[0] + agg_offset);

    } else if (input.getVectorType() == VectorType::FLAT_VECTOR &&
                       addresses.getVectorType() == VectorType::FLAT_VECTOR) {
        auto idata = FlatVector::getData<INPUT_TYPE>(input);
        auto sdata = FlatVector::getData<data_ptr_t>(addresses);
        templatedUpdateStateFlatLoop<INPUT_TYPE>(aggr, idata, sdata, agg_offset, count);
    } else {
       VectorData idata, sdata;
       input.orrify(count, idata);
       addresses.orrify(count, sdata);
       templatedUpdateStateLoop<INPUT_TYPE>(aggr, (INPUT_TYPE *)idata.data_, (data_ptr_t *)sdata.data_,
                                                    *idata.sel_, *sdata.sel_, agg_offset, count);
    }
}

void AggregateFunction::updateStates(RowLayout &layout, Vector &addresses, Vector &input, idx_t count, idx_t agg_idx) {
    if (count == 0)
        return;

    BB_ASSERT(agg_idx < layout.getAggregates().size());
    auto aggr = *layout.getAggregates()[agg_idx];
    switch (input.getType()) {
        case PhysicalType::TINYINT:
            return templatedUpdateState<int8_t>(layout, aggr, addresses, input, count, agg_idx);
        case PhysicalType::SMALLINT:
            return templatedUpdateState<int16_t>(layout, aggr, addresses, input, count, agg_idx);
        case PhysicalType::INTEGER:
            return templatedUpdateState<int32_t>(layout, aggr, addresses, input, count, agg_idx);
        case PhysicalType::BIGINT:
            return templatedUpdateState<int64_t>(layout, aggr, addresses, input, count, agg_idx);
        case PhysicalType::UTINYINT:
            return templatedUpdateState<uint8_t>(layout, aggr, addresses, input, count, agg_idx);
        case PhysicalType::USMALLINT:
            return templatedUpdateState<uint16_t>(layout, aggr, addresses, input, count, agg_idx);
        case PhysicalType::UINTEGER:
            return templatedUpdateState<uint32_t>(layout, aggr, addresses, input, count, agg_idx);
        case PhysicalType::UBIGINT:
            return templatedUpdateState<uint64_t>(layout, aggr, addresses, input, count, agg_idx);
        case PhysicalType::FLOAT:
            return templatedUpdateState<float>(layout, aggr, addresses, input, count, agg_idx);
        case PhysicalType::DOUBLE:
            return templatedUpdateState<double>(layout, aggr, addresses, input, count, agg_idx);
        case PhysicalType::STRING:	{
            return templatedUpdateState<string_t>(layout, aggr, addresses, input, count, agg_idx);
        }
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for select operation!");
    }
}

void AggregateFunction::combineStates(RowLayout &layout, Vector &sources, Vector &targets, const SelectionVector &sel, idx_t count) {
    if (count == 0)
        return;

    BB_ASSERT(sources.getVectorType() == VectorType::FLAT_VECTOR);
    BB_ASSERT(targets.getVectorType() == VectorType::FLAT_VECTOR);
    auto spointers = FlatVector::getData<data_ptr_t>(sources);
    auto tpointers = FlatVector::getData<data_ptr_t>(targets);
    auto &offsets = layout.getOffsets();
    auto aggr_idx = layout.columnCount();

    for (AggregateFunction* aggr : layout.getAggregates()) {
        for (idx_t i = 0; i < count; ++i) {
            auto row_idx = sel.getIndex(i);
            auto srow = spointers[row_idx];
            auto trow = tpointers[row_idx];
            aggr->combine_(srow + offsets[aggr_idx], trow + offsets[aggr_idx]);
        }
        ++aggr_idx;
    }
}



template <class INPUT_TYPE>
void templatedFinalizeStateFlatLoop(AggregateFunction &aggr, INPUT_TYPE* __restrict rdata, data_ptr_t* __restrict sdata, idx_t agg_offset, idx_t count) {
    for (idx_t i = 0; i < count; ++i) {
        auto row = sdata[i];
        aggr.finalize_( row + agg_offset, (data_ptr_t)(rdata +i));
    }
}

template <class INPUT_TYPE>
void templatedFinalizeStateLoop(AggregateFunction &aggr, INPUT_TYPE* __restrict rdata, data_ptr_t* __restrict sdata,const SelectionVector& srdata,const SelectionVector& ssdata, idx_t agg_offset, idx_t count) {
    for (idx_t i = 0; i < count; ++i) {
        auto idx = srdata.getIndex(i);
        auto row_idx = ssdata.getIndex(i);
        auto row = sdata[row_idx];
        aggr.finalize_( row + agg_offset, (data_ptr_t)(rdata +idx));
    }
}

template<class RESULT_TYPE>
void templatedFinalizeState(AggregateFunction& aggr, Vector &addresses, Vector &result, idx_t count, idx_t agg_offset ) {

    if (addresses.getVectorType() == VectorType::CONSTANT_VECTOR) {
        BB_ASSERT(result.getVectorType() == VectorType::CONSTANT_VECTOR);
        // regular constant: get first state
        auto rdata = ConstantVector::getData<RESULT_TYPE>(result);
        auto sdata = ConstantVector::getData<data_ptr_t>(addresses);
        aggr.finalize_(  sdata[0] + agg_offset, (data_ptr_t)(rdata));

    } else if (addresses.getVectorType() == VectorType::FLAT_VECTOR) {
        BB_ASSERT(result.getVectorType() == VectorType::FLAT_VECTOR);
        auto rdata = FlatVector::getData<RESULT_TYPE>(result);
        auto sdata = FlatVector::getData<data_ptr_t>(addresses);
        templatedFinalizeStateFlatLoop<RESULT_TYPE>(aggr, rdata, sdata, agg_offset, count);
    } else {
        VectorData rdata, sdata;
        result.orrify(count, rdata);
        addresses.orrify(count, sdata);
        templatedFinalizeStateLoop<RESULT_TYPE>(aggr, (RESULT_TYPE *)rdata.data_, (data_ptr_t *)sdata.data_,
                                                    *rdata.sel_, *sdata.sel_, agg_offset, count);
   }
}

void templatedFinalizeStateSwitch(AggregateFunction& aggr, Vector &addresses, Vector &result, idx_t count, idx_t aggr_offset ) {
    switch (result.getType()) {
        case PhysicalType::TINYINT:
            return templatedFinalizeState<int8_t>( aggr, addresses, result, count, aggr_offset);
        case PhysicalType::SMALLINT:
            return templatedFinalizeState<int16_t>( aggr, addresses, result, count, aggr_offset);
        case PhysicalType::INTEGER:
            return templatedFinalizeState<int32_t>( aggr, addresses, result, count, aggr_offset);
        case PhysicalType::BIGINT:
            return templatedFinalizeState<int64_t>( aggr, addresses, result, count, aggr_offset);
        case PhysicalType::UTINYINT:
            return templatedFinalizeState<uint8_t>( aggr, addresses, result, count, aggr_offset);
        case PhysicalType::USMALLINT:
            return templatedFinalizeState<uint16_t>( aggr, addresses, result, count, aggr_offset);
        case PhysicalType::UINTEGER:
            return templatedFinalizeState<uint32_t>( aggr, addresses, result, count, aggr_offset);
        case PhysicalType::UBIGINT:
            return templatedFinalizeState<uint64_t>( aggr, addresses, result, count, aggr_offset);
        case PhysicalType::FLOAT:
            return templatedFinalizeState<float>( aggr, addresses, result, count, aggr_offset);
        case PhysicalType::DOUBLE:
            return templatedFinalizeState<double>( aggr, addresses, result, count, aggr_offset);
        case PhysicalType::STRING:	{
            return templatedFinalizeState<string_t>( aggr, addresses, result, count, aggr_offset);
        }
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for select operation!");
    }
}

void AggregateFunction::finalizeStates(RowLayout &layout, Vector &addresses, DataChunk &result, idx_t count) {
    BB_ASSERT(layout.getAggregates().size()  == result.columnCount());
    idx_t agg_idx = 0;
    for (auto& aggr : layout.getAggregates()) {
        auto agg_offset = layout.getOffsets()[ layout.columnCount() + agg_idx];
        templatedFinalizeStateSwitch(*aggr, addresses, result.data_[agg_idx],count, agg_offset);
        ++agg_idx;
    }
}

void AggregateFunction::finalizeStates(RowLayout &layout, Vector &addresses, Vector &result, idx_t agg_index, idx_t count) {
    auto& aggrFunction = layout.getAggregates()[agg_index];
    auto agg_offset = layout.getOffsets()[ layout.columnCount() + agg_index];
    templatedFinalizeStateSwitch(*aggrFunction, addresses, result, count, agg_offset);
}
}
