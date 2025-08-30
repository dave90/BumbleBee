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

#include "bumblebee/common/operator/CastOperators.h"
#include "bumblebee/common/vector_operations/UnaryExecution.h"
#include "bumblebee/common/vector_operations/VectorOperations.h"


namespace bumblebee {


template <class SRC, class DST, class OP>
void vectorCastLoop(Vector &source, Vector &result, idx_t count) {
    UnaryExecution::execute<SRC, DST, OP>(source, result, count);
}


template <class SRC>
void numericCastSwitch(Vector &source, Vector &result, idx_t count) {

    switch (result.getType()) {
        case ConstantType::TINYINT:
            vectorCastLoop<SRC, int8_t, Cast>(source, result, count);
            break;
        case ConstantType::SMALLINT:
            vectorCastLoop<SRC, int16_t, Cast>(source, result, count);
            break;
        case ConstantType::INTEGER:
            vectorCastLoop<SRC, int32_t, Cast>(source, result, count);
            break;
        case ConstantType::UTINYINT:
            vectorCastLoop<SRC, uint8_t, Cast>(source, result, count);
            break;
        case ConstantType::USMALLINT:
            vectorCastLoop<SRC, uint16_t, Cast>(source, result, count);
            break;
        case ConstantType::UINTEGER:
            vectorCastLoop<SRC, uint32_t, Cast>(source, result, count);
            break;
        case ConstantType::UBIGINT:
            vectorCastLoop<SRC, uint64_t, Cast>(source, result, count);
            break;
        case ConstantType::BIGINT:
            vectorCastLoop<SRC, int64_t, Cast>(source, result, count);
            break;
        case ConstantType::FLOAT:
            vectorCastLoop<SRC, float, Cast>(source, result, count);
            break;
        case ConstantType::DOUBLE:
            vectorCastLoop<SRC, double, Cast>(source, result, count);
            break;
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for execute operation!");
    }
}


void VectorOperations::cast(Vector &source, Vector &result, idx_t count) {
    BB_ASSERT(source.getType() != result.getType());

    switch (source.getType()) {
        case ConstantType::TINYINT:
            numericCastSwitch<int8_t>(source, result, count);
            break;
        case ConstantType::SMALLINT:
            numericCastSwitch<int16_t>(source, result, count);
            break;
        case ConstantType::INTEGER:
            numericCastSwitch<int32_t>(source, result, count);
            break;
        case ConstantType::UTINYINT:
            numericCastSwitch<uint8_t>(source, result, count);
            break;
        case ConstantType::USMALLINT:
            numericCastSwitch<uint16_t>(source, result, count);
            break;
        case ConstantType::UINTEGER:
            numericCastSwitch<uint32_t>(source, result, count);
            break;
        case ConstantType::UBIGINT:
            numericCastSwitch<uint64_t>(source, result, count);
            break;
        case ConstantType::BIGINT:
            numericCastSwitch<int64_t>(source, result, count);
            break;
        case ConstantType::FLOAT:
            numericCastSwitch<float>(source, result, count);
            break;
        case ConstantType::DOUBLE:
            numericCastSwitch<double>(source, result, count);
            break;
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for cast operation!");
    }
}


}
