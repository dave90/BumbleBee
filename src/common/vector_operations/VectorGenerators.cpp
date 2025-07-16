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

#include "bumblebee/common/vector_operations/VectorOperations.h"

namespace bumblebee{

template <class T>
void TemplatedGenerateSequence(Vector &result,idx_t count, int64_t start, int64_t increment) {
    BB_ASSERT(result.getType() != ConstantType::STRING);
    BB_ASSERT(count > 0);
    //TODO check min and max numeric overflow
    result.setVectorType(VectorType::FLAT_VECTOR);
    auto result_data = FlatVector::getData<T>(result);
    auto value = (T)start;
    result_data[0] = value;
    for (idx_t i = 1; i < count; i++) {
        value += increment;
        result_data[i] = value;
    }
}

template <class T>
void TemplatedGenerateSequence(Vector &result,idx_t count, int64_t start, int64_t offset, int64_t stride, int64_t end) {
    BB_ASSERT(result.getType() != ConstantType::STRING);
    BB_ASSERT(count > 0);
    BB_ASSERT(start < end);
    //TODO check min and max numeric overflow
    result.setVectorType(VectorType::FLAT_VECTOR);
    auto result_data = FlatVector::getData<T>(result);
    int64_t size = end - start + 1;
    for (idx_t i = 0; i < count; i++) {
        result_data[i] = (T)start + (T)( (i+offset) / stride % size );
    }
}


template <class T, bool HASH_END>
void TemplatedGeneralGenerateSequence(Vector &result,idx_t count, int64_t start,int64_t offset, int64_t stride, int64_t increment, int64_t end) {
    if (HASH_END)
        TemplatedGenerateSequence<T>(result, count, start, offset,  stride, end);
    else
        TemplatedGenerateSequence<T>(result, count, start, increment);

}

template <bool HAS_END>
void TemplatedGeneralSwitchGenerateSequence(Vector &result,idx_t count, int64_t start, int64_t offset, int64_t stride,  int64_t increment, int64_t end) {
    switch (result.getType()) {
        case ConstantType::TINYINT:
            TemplatedGeneralGenerateSequence<int8_t, HAS_END>(result,count, start, offset, stride, increment, end);
            break;
        case ConstantType::SMALLINT:
            TemplatedGeneralGenerateSequence<int16_t, HAS_END>(result,count, start, offset, stride, increment, end);
            break;
        case ConstantType::INTEGER:
            TemplatedGeneralGenerateSequence<int32_t, HAS_END>(result,count, start, offset, stride, increment, end);
            break;
        case ConstantType::BIGINT:
            TemplatedGeneralGenerateSequence<int64_t, HAS_END>(result,count, start, offset, stride, increment, end);
            break;
        case ConstantType::UTINYINT:
            TemplatedGeneralGenerateSequence<uint8_t, HAS_END>(result,count, start, offset, stride, increment, end);
            break;
        case ConstantType::USMALLINT:
            TemplatedGeneralGenerateSequence<uint16_t, HAS_END>(result,count, start, offset, stride, increment, end);
            break;
        case ConstantType::UINTEGER:
            TemplatedGeneralGenerateSequence<uint32_t, HAS_END>(result,count, start, offset, stride, increment, end);
            break;
        case ConstantType::UBIGINT:
            TemplatedGeneralGenerateSequence<uint64_t, HAS_END>(result,count, start, offset, stride, increment, end);
            break;
        case ConstantType::FLOAT:
            TemplatedGeneralGenerateSequence<float,HAS_END>(result,count, start, offset, stride, increment, end);
            break;
        case ConstantType::DOUBLE:
            TemplatedGeneralGenerateSequence<double,HAS_END>(result,count, start, offset, stride, increment, end);
            break;
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type '%s' for copy!");
    }
}


void VectorOperations::generateSequence(Vector &result, idx_t count, int64_t start, int64_t increment) {
    TemplatedGeneralSwitchGenerateSequence<false>(result, count, start,0, 0, increment, 0);
}

void VectorOperations::generateSequence(Vector &result, idx_t count, int64_t start, int64_t offset, int64_t stride,int64_t end ) {
    TemplatedGeneralSwitchGenerateSequence<true>(result, count, start, offset, stride, 0, end);
}



template <class T>
void TemplatedGenerateSequence(Vector &result, idx_t count, const SelectionVector &sel, int64_t start,
                                   int64_t increment) {
    BB_ASSERT(result.getType() != ConstantType::STRING);
    BB_ASSERT(count > 0);
    //TODO check min and max numeric overflow

    result.setVectorType(VectorType::FLAT_VECTOR);
    auto result_data = FlatVector::getData<T>(result);
    auto value = (T)start;
    for (idx_t i = 0; i < count; i++) {
        auto idx = sel.getIndex(i);
        result_data[idx] = start + idx * increment;
    }
}

template <class T>
void TemplatedGenerateSequence(Vector &result, idx_t count, const SelectionVector &sel, int64_t start, int64_t offset,
                                       int64_t stride, int64_t end ) {
    BB_ASSERT(result.getType() != ConstantType::STRING);
    BB_ASSERT(count > 0);
    //TODO check min and max numeric overflow

    result.setVectorType(VectorType::FLAT_VECTOR);
    auto result_data = FlatVector::getData<T>(result);
    int64_t size = end - start + 1;
    for (idx_t i = 0; i < count; i++) {
        auto idx = sel.getIndex(i);
        result_data[idx] = (T)start + (T)(( offset + idx) / stride % size);
    }
}

template <class T, bool HAS_END>
    void TemplatedGeneralGenerateSelectionSequence(Vector &result, idx_t count, const SelectionVector &sel, int64_t start, int64_t offset,
                                           int64_t stride, int64_t increment, int64_t end ) {
    if (HAS_END)
        TemplatedGenerateSequence<T>(result, count,sel, start, offset, stride, end);
    else
        TemplatedGenerateSequence<T>(result, count,sel, start, increment);

}
template <bool HAS_END>
void TemplatedGeneralSwitchGenerateSequence(Vector &result, idx_t count, const SelectionVector &sel, int64_t start, int64_t offset, int64_t stride, int64_t increment, int64_t end ) {
    switch (result.getType()) {
        case ConstantType::TINYINT:
            TemplatedGeneralGenerateSelectionSequence<int8_t, HAS_END>(result, count, sel, start, offset, stride, increment, end);
            break;
        case ConstantType::SMALLINT:
            TemplatedGeneralGenerateSelectionSequence<int16_t, HAS_END>(result,count, sel, start, offset, stride, increment, end);
            break;
        case ConstantType::INTEGER:
            TemplatedGeneralGenerateSelectionSequence<int32_t, HAS_END>(result,count, sel, start, offset, stride, increment, end);
            break;
        case ConstantType::BIGINT:
            TemplatedGeneralGenerateSelectionSequence<int64_t, HAS_END>(result,count, sel, start, offset, stride, increment, end);
            break;
        case ConstantType::UTINYINT:
            TemplatedGeneralGenerateSelectionSequence<uint8_t, HAS_END>(result,count, sel, start, offset, stride, increment, end);
            break;
        case ConstantType::USMALLINT:
            TemplatedGeneralGenerateSelectionSequence<uint16_t, HAS_END>(result,count, sel, start, offset, stride, increment, end);
            break;
        case ConstantType::UINTEGER:
            TemplatedGeneralGenerateSelectionSequence<uint32_t, HAS_END>(result,count, sel, start, offset, stride, increment, end);
            break;
        case ConstantType::UBIGINT:
            TemplatedGeneralGenerateSelectionSequence<uint64_t, HAS_END>(result,count, sel, start, offset, stride, increment, end);
            break;
        case ConstantType::FLOAT:
            TemplatedGeneralGenerateSelectionSequence<float, HAS_END>(result,count, sel, start, offset, stride, increment, end);
            break;
        case ConstantType::DOUBLE:
            TemplatedGeneralGenerateSelectionSequence<double, HAS_END>(result,count, sel, start, offset, stride, increment, end);
            break;
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type '%s' for copy!");
    }
}


void VectorOperations::generateSequence(Vector &result, idx_t count, const SelectionVector &sel, int64_t start, int64_t increment) {
    TemplatedGeneralSwitchGenerateSequence<false>(result, count, sel, start, 0, 0, increment, 0);
}

void VectorOperations::generateSequence(Vector &result, idx_t count, const SelectionVector &sel, int64_t start, int64_t offset, int64_t stride, int64_t end) {
    TemplatedGeneralSwitchGenerateSequence<true>(result, count, sel, start, offset, stride, 0, end);
}

}
