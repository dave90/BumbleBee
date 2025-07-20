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
#include "bumblebee/common/vector_operations/BinaryExecution.h"
#include  "bumblebee/common/operator/ComparisonOperators.h"

namespace bumblebee {

template <class OP>
idx_t templatedSelectOperationEqualType(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel) {
    BB_ASSERT(left.getType() == right.getType());
    switch (left.getType()) {
        case ConstantType::TINYINT:
            return BinaryExecution::select<int8_t,int8_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::SMALLINT:
            return BinaryExecution::select<int16_t,int16_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::INTEGER:
            return BinaryExecution::select<int32_t,int32_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::BIGINT:
            return BinaryExecution::select<int64_t,int64_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::UTINYINT:
            return BinaryExecution::select<uint8_t,uint8_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::USMALLINT:
            return BinaryExecution::select<uint16_t,uint16_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::UINTEGER:
            return BinaryExecution::select<uint32_t,uint32_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::UBIGINT:
            return BinaryExecution::select<uint64_t,uint64_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::FLOAT:
            return BinaryExecution::select<float,float,OP>(left, right, sel, count, trueSel);
        case ConstantType::DOUBLE:
            return BinaryExecution::select<double,double,OP>(left, right, sel, count, trueSel);
        case ConstantType::STRING:	{
            return BinaryExecution::select<string_t,string_t,OP>(left, right, sel, count, trueSel);
        }
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type '%s' for select operation!");
    }
    return 0;
}

template <class LEFT_TYPE, class RIGHT_TYPE,  class OP>
idx_t templatedSelectOperationLeftRight(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel) {
    return BinaryExecution::select<LEFT_TYPE,RIGHT_TYPE, ComparisonCommonCast<LEFT_TYPE, RIGHT_TYPE, OP>>(left, right, sel, count, trueSel);
}

template <class LEFT_TYPE, class OP>
idx_t templatedSelectOperationLeft(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel) {
    switch (right.getType()) {
        case ConstantType::TINYINT:
            return templatedSelectOperationLeftRight<LEFT_TYPE,int8_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::SMALLINT:
            return templatedSelectOperationLeftRight<LEFT_TYPE,int16_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::INTEGER:
            return templatedSelectOperationLeftRight<LEFT_TYPE,int32_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::BIGINT:
            return templatedSelectOperationLeftRight<LEFT_TYPE,int64_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::UTINYINT:
            return templatedSelectOperationLeftRight<LEFT_TYPE,uint8_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::USMALLINT:
            return templatedSelectOperationLeftRight<LEFT_TYPE,uint16_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::UINTEGER:
            return templatedSelectOperationLeftRight<LEFT_TYPE,uint32_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::UBIGINT:
            return templatedSelectOperationLeftRight<LEFT_TYPE,uint64_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::FLOAT:
            return templatedSelectOperationLeftRight<LEFT_TYPE,float,OP>(left, right, sel, count, trueSel);
        case ConstantType::DOUBLE:
            return templatedSelectOperationLeftRight<LEFT_TYPE,double,OP>(left, right, sel, count, trueSel);
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type '%s' for select operation!");
    }
    return 0;
}
template <class OP>
idx_t templatedSelectOperation(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel) {
    // cannot compare string with different types
    BB_ASSERT(left.getType() != ConstantType::STRING && right.getType() != ConstantType::STRING);
    // left type != right type
    switch (left.getType()) {
        case ConstantType::TINYINT:
            return templatedSelectOperationLeft<int8_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::SMALLINT:
            return templatedSelectOperationLeft<int16_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::INTEGER:
            return templatedSelectOperationLeft<int32_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::BIGINT:
            return templatedSelectOperationLeft<int64_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::UTINYINT:
            return templatedSelectOperationLeft<uint8_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::USMALLINT:
            return templatedSelectOperationLeft<uint16_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::UINTEGER:
            return templatedSelectOperationLeft<uint32_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::UBIGINT:
            return templatedSelectOperationLeft<uint64_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::FLOAT:
            return templatedSelectOperationLeft<float,OP>(left, right, sel, count, trueSel);
        case ConstantType::DOUBLE:
            return templatedSelectOperationLeft<double,OP>(left, right, sel, count, trueSel);
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type '%s' for select operation!");
    }
    return 0;
}


idx_t VectorOperations::equals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel){
    if (left.getType() == right.getType())
        return templatedSelectOperationEqualType<Equals>(left, right, sel, count, trueSel);
    return templatedSelectOperation<Equals>(left, right, sel, count, trueSel);
}
idx_t VectorOperations::notEquals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel){
    if (left.getType() == right.getType())
        return templatedSelectOperationEqualType<NotEquals>(left, right, sel, count, trueSel);
    return templatedSelectOperation<NotEquals>(left, right, sel, count, trueSel);
}
idx_t VectorOperations::greaterThan(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel){
    if (left.getType() == right.getType())
        return templatedSelectOperationEqualType<GreaterThan>(left, right, sel, count, trueSel);
    return templatedSelectOperation<GreaterThan>(left, right, sel, count, trueSel);

}
idx_t VectorOperations::greaterThanEquals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel){
    if (left.getType() == right.getType())
        return templatedSelectOperationEqualType<GreaterThanEquals>(left, right, sel, count, trueSel);
    return templatedSelectOperation<GreaterThanEquals>(left, right, sel, count, trueSel);

}
idx_t VectorOperations::lessThan(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel){
    if (left.getType() == right.getType())
        return templatedSelectOperationEqualType<LessThan>(left, right, sel, count, trueSel);
    return templatedSelectOperation<LessThan>(left, right, sel, count, trueSel);

}
idx_t VectorOperations::lessThanEquals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel){
    if (left.getType() == right.getType())
        return templatedSelectOperationEqualType<LessThanEquals>(left, right, sel, count, trueSel);
    return templatedSelectOperation<LessThanEquals>(left, right, sel, count, trueSel);
}


}