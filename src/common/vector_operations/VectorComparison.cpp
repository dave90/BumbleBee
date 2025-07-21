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
idx_t templatedSelectOperationSwitchEqualType(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel) {
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
            ErrorHandler::errorNotImplemented("Unimplemented type for select operation!");
    }
    return 0;
}


template<class LEFT_TYPE, class RIGHT_TYPE, class COMMON_TYPE, class OP>
    struct ComparisonCommonCast {
    static inline bool operation(LEFT_TYPE left, RIGHT_TYPE right) {
        return OP::operation(static_cast<COMMON_TYPE>(left), static_cast<COMMON_TYPE>(right));
    }
};

template <class LEFT_TYPE, class RIGHT_TYPE,  class OP>
idx_t templatedSelectOperationSwitchCommon(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel) {
    auto commonType = getCommonType(left.getType(), right.getType());

    // for int and uint collapse to int 64 bit
    // for decimal collapse to double
    switch (commonType) {
        case ConstantType::TINYINT:
        case ConstantType::SMALLINT:
        case ConstantType::INTEGER:
        case ConstantType::UTINYINT:
        case ConstantType::USMALLINT:
        case ConstantType::UINTEGER:
        case ConstantType::UBIGINT:
        case ConstantType::BIGINT:
            return BinaryExecution::select<LEFT_TYPE,RIGHT_TYPE, ComparisonCommonCast<LEFT_TYPE, RIGHT_TYPE, int64_t, OP>>(left, right, sel, count, trueSel);
        case ConstantType::FLOAT:
        case ConstantType::DOUBLE:
            return BinaryExecution::select<LEFT_TYPE,RIGHT_TYPE, ComparisonCommonCast<LEFT_TYPE, RIGHT_TYPE, double, OP>>(left, right, sel, count, trueSel);
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for select operation!");
    }
    return 0;
}

template <class LEFT_TYPE, class OP>
idx_t templatedSelectOperationSwitchRight(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel) {
    switch (right.getType()) {
        case ConstantType::TINYINT:
            return templatedSelectOperationSwitchCommon<LEFT_TYPE,int8_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::SMALLINT:
            return templatedSelectOperationSwitchCommon<LEFT_TYPE,int16_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::INTEGER:
            return templatedSelectOperationSwitchCommon<LEFT_TYPE,int32_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::BIGINT:
            return templatedSelectOperationSwitchCommon<LEFT_TYPE,int64_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::UTINYINT:
            return templatedSelectOperationSwitchCommon<LEFT_TYPE,uint8_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::USMALLINT:
            return templatedSelectOperationSwitchCommon<LEFT_TYPE,uint16_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::UINTEGER:
            return templatedSelectOperationSwitchCommon<LEFT_TYPE,uint32_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::UBIGINT:
            return templatedSelectOperationSwitchCommon<LEFT_TYPE,uint64_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::FLOAT:
            return templatedSelectOperationSwitchCommon<LEFT_TYPE,float,OP>(left, right, sel, count, trueSel);
        case ConstantType::DOUBLE:
            return templatedSelectOperationSwitchCommon<LEFT_TYPE,double,OP>(left, right, sel, count, trueSel);
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for select operation!");
    }
    return 0;
}
template <class OP>
idx_t templatedSelectOperationSwitchLeft(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel) {
    // cannot compare string with different types
    BB_ASSERT(left.getType() != ConstantType::STRING && right.getType() != ConstantType::STRING);
    // left type != right type
    switch (left.getType()) {
        case ConstantType::TINYINT:
            return templatedSelectOperationSwitchRight<int8_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::SMALLINT:
            return templatedSelectOperationSwitchRight<int16_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::INTEGER:
            return templatedSelectOperationSwitchRight<int32_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::BIGINT:
            return templatedSelectOperationSwitchRight<int64_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::UTINYINT:
            return templatedSelectOperationSwitchRight<uint8_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::USMALLINT:
            return templatedSelectOperationSwitchRight<uint16_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::UINTEGER:
            return templatedSelectOperationSwitchRight<uint32_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::UBIGINT:
            return templatedSelectOperationSwitchRight<uint64_t,OP>(left, right, sel, count, trueSel);
        case ConstantType::FLOAT:
            return templatedSelectOperationSwitchRight<float,OP>(left, right, sel, count, trueSel);
        case ConstantType::DOUBLE:
            return templatedSelectOperationSwitchRight<double,OP>(left, right, sel, count, trueSel);
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for select operation!");
    }
    return 0;
}



idx_t VectorOperations::equals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel){
    if (left.getType() == right.getType())
        return templatedSelectOperationSwitchEqualType<Equals>(left, right, sel, count, trueSel);
    return templatedSelectOperationSwitchLeft<Equals>(left, right, sel, count, trueSel);
}
idx_t VectorOperations::notEquals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel){
    if (left.getType() == right.getType())
        return templatedSelectOperationSwitchEqualType<NotEquals>(left, right, sel, count, trueSel);
    return templatedSelectOperationSwitchLeft<NotEquals>(left, right, sel, count, trueSel);
}
idx_t VectorOperations::greaterThan(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel){
    if (left.getType() == right.getType())
        return templatedSelectOperationSwitchEqualType<GreaterThan>(left, right, sel, count, trueSel);
    return templatedSelectOperationSwitchLeft<GreaterThan>(left, right, sel, count, trueSel);

}
idx_t VectorOperations::greaterThanEquals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel){
    if (left.getType() == right.getType())
        return templatedSelectOperationSwitchEqualType<GreaterThanEquals>(left, right, sel, count, trueSel);
    return templatedSelectOperationSwitchLeft<GreaterThanEquals>(left, right, sel, count, trueSel);

}
idx_t VectorOperations::lessThan(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel){
    if (left.getType() == right.getType())
        return templatedSelectOperationSwitchEqualType<LessThan>(left, right, sel, count, trueSel);
    return templatedSelectOperationSwitchLeft<LessThan>(left, right, sel, count, trueSel);

}
idx_t VectorOperations::lessThanEquals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel){
    if (left.getType() == right.getType())
        return templatedSelectOperationSwitchEqualType<LessThanEquals>(left, right, sel, count, trueSel);
    return templatedSelectOperationSwitchLeft<LessThanEquals>(left, right, sel, count, trueSel);
}


}