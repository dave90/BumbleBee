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

void VectorOperations::generateSequence(Vector &result, idx_t count, int64_t start, int64_t increment) {

    switch (result.getType()) {
        case ConstantType::TINYINT:
            TemplatedGenerateSequence<int8_t>(result,count, start, increment);
            break;
        case ConstantType::SMALLINT:
            TemplatedGenerateSequence<int16_t>(result,count, start, increment);
            break;
        case ConstantType::INTEGER:
            TemplatedGenerateSequence<int32_t>(result,count, start, increment);
            break;
        case ConstantType::BIGINT:
            TemplatedGenerateSequence<int64_t>(result,count, start, increment);
            break;
        case ConstantType::UTINYINT:
            TemplatedGenerateSequence<uint8_t>(result,count, start, increment);
            break;
        case ConstantType::USMALLINT:
            TemplatedGenerateSequence<uint16_t>(result,count, start, increment);
            break;
        case ConstantType::UINTEGER:
            TemplatedGenerateSequence<uint32_t>(result,count, start, increment);
            break;
        case ConstantType::UBIGINT:
            TemplatedGenerateSequence<uint64_t>(result,count, start, increment);
            break;
        case ConstantType::FLOAT:
            TemplatedGenerateSequence<float>(result,count, start, increment);
            break;
        case ConstantType::DOUBLE:
            TemplatedGenerateSequence<double>(result,count, start, increment);
            break;
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type '%s' for copy!");
    }
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
        result_data[idx] = value + increment * idx;
    }
}

void VectorOperations::generateSequence(Vector &result, idx_t count, const SelectionVector &sel, int64_t start, int64_t increment) {
    switch (result.getType()) {
        case ConstantType::TINYINT:
            TemplatedGenerateSequence<int8_t>(result,count, sel, start, increment);
            break;
        case ConstantType::SMALLINT:
            TemplatedGenerateSequence<int16_t>(result,count, sel, start, increment);
            break;
        case ConstantType::INTEGER:
            TemplatedGenerateSequence<int32_t>(result,count, sel, start, increment);
            break;
        case ConstantType::BIGINT:
            TemplatedGenerateSequence<int64_t>(result,count, sel, start, increment);
            break;
        case ConstantType::UTINYINT:
            TemplatedGenerateSequence<uint8_t>(result,count, sel, start, increment);
            break;
        case ConstantType::USMALLINT:
            TemplatedGenerateSequence<uint16_t>(result,count, sel, start, increment);
            break;
        case ConstantType::UINTEGER:
            TemplatedGenerateSequence<uint32_t>(result,count, sel, start, increment);
            break;
        case ConstantType::UBIGINT:
            TemplatedGenerateSequence<uint64_t>(result,count, sel, start, increment);
            break;
        case ConstantType::FLOAT:
            TemplatedGenerateSequence<float>(result,count, sel, start, increment);
            break;
        case ConstantType::DOUBLE:
            TemplatedGenerateSequence<double>(result,count, sel, start, increment);
            break;
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type '%s' for copy!");
    }
}

}
