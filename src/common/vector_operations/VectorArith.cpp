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

#include "CLI11.hpp"
#include "bumblebee/common/operator/ArithOperators.hpp"
#include "bumblebee/common/vector_operations/BinaryExecution.hpp"
#include "bumblebee/common/vector_operations/VectorOperations.hpp"

namespace bumblebee {


template <class OP>
void templatedExecuteOperationSwitchEqualType(Vector &left, Vector &right,Vector &result, idx_t count) {
    BB_ASSERT(left.getType() == right.getType() && left.getType() == result.getType());
    switch (left.getType()) {
        case PhysicalType::TINYINT:
            BinaryExecution::execute<int8_t,int8_t,int8_t,OP>(left, right, result, count);
            break;
        case PhysicalType::SMALLINT:
            BinaryExecution::execute<int16_t,int16_t,int16_t,OP>(left, right, result, count);
            break;
        case PhysicalType::INTEGER:
            BinaryExecution::execute<int32_t,int32_t,int32_t,OP>(left, right, result, count);
            break;
        case PhysicalType::BIGINT:
            BinaryExecution::execute<int64_t,int64_t,int64_t,OP>(left, right, result, count);
            break;
        case PhysicalType::UTINYINT:
            BinaryExecution::execute<uint8_t,uint8_t,uint8_t,OP>(left, right, result, count);
            break;
        case PhysicalType::USMALLINT:
            BinaryExecution::execute<uint16_t,uint16_t,uint16_t,OP>(left, right, result, count);
            break;
        case PhysicalType::UINTEGER:
            BinaryExecution::execute<uint32_t,uint32_t,uint32_t,OP>(left, right, result, count);
            break;
        case PhysicalType::UBIGINT:
            BinaryExecution::execute<uint64_t,uint64_t,uint64_t,OP>(left, right, result, count);
            break;
        case PhysicalType::FLOAT:
            BinaryExecution::execute<float,float,float,OP>(left, right, result, count);
            break;
        case PhysicalType::DOUBLE:
            BinaryExecution::execute<double,double,double,OP>(left, right, result, count);
            break;
        case PhysicalType::STRING:	{
            BinaryExecution::execute<string_t,string_t,string_t,OP>(left, right, result, count);
            break;
        }
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for execute operation!");
    }
}

template<class LEFT_TYPE, class RIGHT_TYPE, class RESULT_TYPE, class OP>
struct ArithCommonCast {
    static inline RESULT_TYPE operation(LEFT_TYPE left, RIGHT_TYPE right) {
        return OP::operation(static_cast<RESULT_TYPE>(left), static_cast<RESULT_TYPE>(right));
    }
};

template <class LEFT_TYPE, class RIGHT_TYPE,  class OP>
void templatedExecuteOperationSwitchResult(Vector &left, Vector &right, Vector &result, idx_t count) {
    switch (result.getType()) {
        case PhysicalType::TINYINT:
            BinaryExecution::execute<LEFT_TYPE,RIGHT_TYPE, int8_t, ArithCommonCast<LEFT_TYPE, RIGHT_TYPE, int8_t, OP>>(left, right, result, count);
            break;
        case PhysicalType::SMALLINT:
            BinaryExecution::execute<LEFT_TYPE,RIGHT_TYPE, int16_t, ArithCommonCast<LEFT_TYPE, RIGHT_TYPE, int16_t, OP>>(left, right, result, count);
            break;
        case PhysicalType::INTEGER:
            BinaryExecution::execute<LEFT_TYPE,RIGHT_TYPE, int32_t, ArithCommonCast<LEFT_TYPE, RIGHT_TYPE, int32_t, OP>>(left, right, result, count);
            break;
        case PhysicalType::UTINYINT:
            BinaryExecution::execute<LEFT_TYPE,RIGHT_TYPE, uint8_t, ArithCommonCast<LEFT_TYPE, RIGHT_TYPE, uint8_t, OP>>(left, right, result, count);
            break;
        case PhysicalType::USMALLINT:
            BinaryExecution::execute<LEFT_TYPE,RIGHT_TYPE, uint16_t, ArithCommonCast<LEFT_TYPE, RIGHT_TYPE, uint16_t, OP>>(left, right, result, count);
            break;
        case PhysicalType::UINTEGER:
            BinaryExecution::execute<LEFT_TYPE,RIGHT_TYPE, uint32_t, ArithCommonCast<LEFT_TYPE, RIGHT_TYPE, uint32_t, OP>>(left, right, result, count);
            break;
        case PhysicalType::UBIGINT:
            BinaryExecution::execute<LEFT_TYPE,RIGHT_TYPE, uint64_t, ArithCommonCast<LEFT_TYPE, RIGHT_TYPE, uint64_t, OP>>(left, right, result, count);
            break;
        case PhysicalType::BIGINT:
            BinaryExecution::execute<LEFT_TYPE,RIGHT_TYPE, int64_t, ArithCommonCast<LEFT_TYPE, RIGHT_TYPE, int64_t, OP>>(left, right, result, count);
            break;
        case PhysicalType::FLOAT:
            BinaryExecution::execute<LEFT_TYPE,RIGHT_TYPE, float, ArithCommonCast<LEFT_TYPE, RIGHT_TYPE, float, OP>>(left, right, result, count);
            break;
        case PhysicalType::DOUBLE:
            BinaryExecution::execute<LEFT_TYPE,RIGHT_TYPE, double, ArithCommonCast<LEFT_TYPE, RIGHT_TYPE, double, OP>>(left, right, result, count);
            break;
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for execute operation!");
    }
}

template <class LEFT_TYPE,  class OP>
void templatedExecuteOperationSwitchRight(Vector &left, Vector &right, Vector &result, idx_t count) {
    switch (right.getType()) {
        case PhysicalType::TINYINT:
            templatedExecuteOperationSwitchResult<LEFT_TYPE,int8_t, OP>(left, right, result, count);
            break;
        case PhysicalType::SMALLINT:
            templatedExecuteOperationSwitchResult<LEFT_TYPE,int16_t, OP>(left, right, result, count);
            break;
        case PhysicalType::INTEGER:
            templatedExecuteOperationSwitchResult<LEFT_TYPE,int32_t, OP>(left, right, result, count);
            break;
        case PhysicalType::UTINYINT:
            templatedExecuteOperationSwitchResult<LEFT_TYPE,uint8_t, OP>(left, right, result, count);
            break;
        case PhysicalType::USMALLINT:
            templatedExecuteOperationSwitchResult<LEFT_TYPE,uint16_t, OP>(left, right, result, count);
            break;
        case PhysicalType::UINTEGER:
            templatedExecuteOperationSwitchResult<LEFT_TYPE,uint32_t, OP>(left, right, result, count);
            break;
        case PhysicalType::UBIGINT:
            templatedExecuteOperationSwitchResult<LEFT_TYPE,uint64_t, OP>(left, right, result, count);
            break;
        case PhysicalType::BIGINT:
            templatedExecuteOperationSwitchResult<LEFT_TYPE,int64_t, OP>(left, right, result, count);
            break;
        case PhysicalType::FLOAT:
            templatedExecuteOperationSwitchResult<LEFT_TYPE,float, OP>(left, right, result, count);
            break;
        case PhysicalType::DOUBLE:
            templatedExecuteOperationSwitchResult<LEFT_TYPE,double, OP>(left, right, result, count);
            break;
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for execute operation!");
    }
}

template <class OP>
void templatedExecuteOperationSwitchLeft(Vector &left, Vector &right, Vector &result, idx_t count) {
    switch (left.getType()) {
        case PhysicalType::TINYINT:
            templatedExecuteOperationSwitchRight<int8_t, OP>(left, right, result, count);
            break;
        case PhysicalType::SMALLINT:
            templatedExecuteOperationSwitchRight<int16_t, OP>(left, right, result, count);
            break;
        case PhysicalType::INTEGER:
            templatedExecuteOperationSwitchRight<int32_t, OP>(left, right, result, count);
            break;
        case PhysicalType::UTINYINT:
            templatedExecuteOperationSwitchRight<uint8_t, OP>(left, right, result, count);
            break;
        case PhysicalType::USMALLINT:
            templatedExecuteOperationSwitchRight<uint16_t, OP>(left, right, result, count);
            break;
        case PhysicalType::UINTEGER:
            templatedExecuteOperationSwitchRight<uint32_t, OP>(left, right, result, count);
            break;
        case PhysicalType::UBIGINT:
            templatedExecuteOperationSwitchRight<uint64_t, OP>(left, right, result, count);
            break;
        case PhysicalType::BIGINT:
            templatedExecuteOperationSwitchRight<int64_t, OP>(left, right, result, count);
            break;
        case PhysicalType::FLOAT:
            templatedExecuteOperationSwitchRight<float, OP>(left, right, result, count);
            break;
        case PhysicalType::DOUBLE:
            templatedExecuteOperationSwitchRight<double, OP>(left, right, result, count);
            break;
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for execute operation!");
    }
}

void VectorOperations::sum(Vector &left, Vector &right, Vector &result, idx_t count) {
    if (left.getType() == right.getType() && left.getType() == result.getType())
        templatedExecuteOperationSwitchEqualType<Sum>(left, right, result, count);
    else
        templatedExecuteOperationSwitchLeft<Sum>(left, right, result, count);
}

void VectorOperations::dot(Vector &left, Vector &right, Vector &result, idx_t count) {
    if (left.getType() == right.getType() && left.getType() == result.getType())
        templatedExecuteOperationSwitchEqualType<Dot>(left, right, result, count);
    else
        templatedExecuteOperationSwitchLeft<Dot>(left, right, result, count);

}

void VectorOperations::division(Vector &left, Vector &right, Vector &result, idx_t count) {
    if (left.getType() == right.getType() && left.getType() == result.getType())
        templatedExecuteOperationSwitchEqualType<Division>(left, right, result, count);
    else
        templatedExecuteOperationSwitchLeft<Division>(left, right, result, count);

}

void VectorOperations::difference(Vector &left, Vector &right, Vector &result, idx_t count) {
    if (left.getType() == right.getType() && left.getType() == result.getType())
        templatedExecuteOperationSwitchEqualType<Difference>(left, right, result, count);
    else
        templatedExecuteOperationSwitchLeft<Difference>(left, right, result, count);
}

void VectorOperations::modulo(Vector &left, Vector &right, Vector &result, idx_t count) {
    if (left.getType() == right.getType() && left.getType() == result.getType())
        templatedExecuteOperationSwitchEqualType<Modulo>(left, right, result, count);
    else
        templatedExecuteOperationSwitchLeft<Modulo>(left, right, result, count);
}

void VectorOperations::lAnd(Vector &left, Vector &right, Vector &result, idx_t count) {
    if (left.getType() == right.getType() && left.getType() == result.getType())
        templatedExecuteOperationSwitchEqualType<And>(left, right, result, count);
    else
        templatedExecuteOperationSwitchLeft<And>(left, right, result, count);
}


}