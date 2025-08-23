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

#include "bumblebee/function/AggregateFunction.h"

namespace bumblebee {
void AggregateFunction::initStates(data_ptr_t states, SelectionVector& sel, AggregateFunction &func, idx_t count) {
    auto size = func.stateSize_();
    for (idx_t i = 0; i < count; i++) {
        idx_t index = sel.getIndex(i);
        uint8_t* state = states + index * size;
        func.initialize_(state);
    }
}

void AggregateFunction::combineStates(data_ptr_t states, data_ptr_t targetStates,SelectionVector &sel, SelectionVector &targetSel,
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
    void templatedUpdateState(Vector& input, data_ptr_t states, SelectionVector &sel, AggregateFunction &func,
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

void AggregateFunction::updateState(Vector& input, data_ptr_t states, SelectionVector &sel, AggregateFunction &func,
    idx_t count) {

    switch (input.getType()) {
        case ConstantType::TINYINT:
            return templatedUpdateState<int8_t>(input, states, sel, func, count);
        case ConstantType::SMALLINT:
            return templatedUpdateState<int16_t>(input, states, sel, func, count);
        case ConstantType::INTEGER:
            return templatedUpdateState<int32_t>(input, states, sel, func, count);
        case ConstantType::BIGINT:
            return templatedUpdateState<int64_t>(input, states, sel, func, count);
        case ConstantType::UTINYINT:
            return templatedUpdateState<uint8_t>(input, states, sel, func, count);
        case ConstantType::USMALLINT:
            return templatedUpdateState<uint16_t>(input, states, sel, func, count);
        case ConstantType::UINTEGER:
            return templatedUpdateState<uint32_t>(input, states, sel, func, count);
        case ConstantType::UBIGINT:
            return templatedUpdateState<uint64_t>(input, states, sel, func, count);
        case ConstantType::FLOAT:
            return templatedUpdateState<float>(input, states, sel, func, count);
        case ConstantType::DOUBLE:
            return templatedUpdateState<double>(input, states, sel, func, count);
        case ConstantType::STRING:	{
            return templatedUpdateState<string_t>(input, states, sel, func, count);
        }
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for select operation!");
    }
}

template <class T>
void templatedFinalizeState(Vector &result, data_ptr_t states, SelectionVector &sel, AggregateFunction &func,
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


void AggregateFunction::finalizeState(Vector &result, data_ptr_t states, SelectionVector &sel, AggregateFunction &func,
    idx_t count) {

    switch (result.getType()) {
        case ConstantType::TINYINT:
            return templatedFinalizeState<int8_t>(result, states, sel, func, count);
        case ConstantType::SMALLINT:
            return templatedFinalizeState<int16_t>(result, states, sel, func, count);
        case ConstantType::INTEGER:
            return templatedFinalizeState<int32_t>(result, states, sel, func, count);
        case ConstantType::BIGINT:
            return templatedFinalizeState<int64_t>(result, states, sel, func, count);
        case ConstantType::UTINYINT:
            return templatedFinalizeState<uint8_t>(result, states, sel, func, count);
        case ConstantType::USMALLINT:
            return templatedFinalizeState<uint16_t>(result, states, sel, func, count);
        case ConstantType::UINTEGER:
            return templatedFinalizeState<uint32_t>(result, states, sel, func, count);
        case ConstantType::UBIGINT:
            return templatedFinalizeState<uint64_t>(result, states, sel, func, count);
        case ConstantType::FLOAT:
            return templatedFinalizeState<float>(result, states, sel, func, count);
        case ConstantType::DOUBLE:
            return templatedFinalizeState<double>(result, states, sel, func, count);
        case ConstantType::STRING:	{
            return templatedFinalizeState<string_t>(result, states, sel, func, count);
        }
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for select operation!");
    }
}

}
