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

#include "bumblebee/common/vector_operations/VectorOperations.hpp"
#include "bumblebee/common/vector_operations/BinaryExecution.hpp"
#include  "bumblebee/common/operator/ComparisonOperators.hpp"

namespace bumblebee {

template <class OP>
idx_t templatedSelectOperationSwitchEqualType(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel, SelectionVector *falseSel, idx_t& falseCount) {
    BB_ASSERT(left.getType() == right.getType());
    switch (left.getType()) {
        case PhysicalType::TINYINT:
            return BinaryExecution::select<int8_t,int8_t,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        case PhysicalType::SMALLINT:
            return BinaryExecution::select<int16_t,int16_t,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        case PhysicalType::INTEGER:
            return BinaryExecution::select<int32_t,int32_t,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        case PhysicalType::BIGINT:
            return BinaryExecution::select<int64_t,int64_t,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        case PhysicalType::UTINYINT:
            return BinaryExecution::select<uint8_t,uint8_t,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        case PhysicalType::USMALLINT:
            return BinaryExecution::select<uint16_t,uint16_t,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        case PhysicalType::UINTEGER:
            return BinaryExecution::select<uint32_t,uint32_t,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        case PhysicalType::UBIGINT:
            return BinaryExecution::select<uint64_t,uint64_t,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        case PhysicalType::FLOAT:
            return BinaryExecution::select<float,float,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        case PhysicalType::DOUBLE:
            return BinaryExecution::select<double,double,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        case PhysicalType::STRING:	{
            return BinaryExecution::select<string_t,string_t,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
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
idx_t templatedSelectOperationSwitchCommon(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel, SelectionVector *falseSel, idx_t& falseCount) {
    auto commonType = getCommonType(left.getType(), right.getType());

    // for int and uint collapse to int 64 bit
    // for decimal collapse to double
    switch (commonType.getPhysicalType()) {
        case PhysicalType::TINYINT:
        case PhysicalType::SMALLINT:
        case PhysicalType::INTEGER:
        case PhysicalType::UTINYINT:
        case PhysicalType::USMALLINT:
        case PhysicalType::UINTEGER:
        case PhysicalType::UBIGINT:
        case PhysicalType::BIGINT:
            return BinaryExecution::select<LEFT_TYPE,RIGHT_TYPE, ComparisonCommonCast<LEFT_TYPE, RIGHT_TYPE, int64_t, OP>>(left, right, sel, count, trueSel, falseSel, falseCount);
        case PhysicalType::FLOAT:
        case PhysicalType::DOUBLE:
            return BinaryExecution::select<LEFT_TYPE,RIGHT_TYPE, ComparisonCommonCast<LEFT_TYPE, RIGHT_TYPE, double, OP>>(left, right, sel, count, trueSel, falseSel, falseCount);
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for select operation!");
    }
    return 0;
}

template <class LEFT_TYPE, class OP>
idx_t templatedSelectOperationSwitchRight(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel, SelectionVector *falseSel, idx_t& falseCount) {
    switch (right.getType()) {
        case PhysicalType::TINYINT:
            return templatedSelectOperationSwitchCommon<LEFT_TYPE,int8_t,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        case PhysicalType::SMALLINT:
            return templatedSelectOperationSwitchCommon<LEFT_TYPE,int16_t,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        case PhysicalType::INTEGER:
            return templatedSelectOperationSwitchCommon<LEFT_TYPE,int32_t,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        case PhysicalType::BIGINT:
            return templatedSelectOperationSwitchCommon<LEFT_TYPE,int64_t,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        case PhysicalType::UTINYINT:
            return templatedSelectOperationSwitchCommon<LEFT_TYPE,uint8_t,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        case PhysicalType::USMALLINT:
            return templatedSelectOperationSwitchCommon<LEFT_TYPE,uint16_t,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        case PhysicalType::UINTEGER:
            return templatedSelectOperationSwitchCommon<LEFT_TYPE,uint32_t,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        case PhysicalType::UBIGINT:
            return templatedSelectOperationSwitchCommon<LEFT_TYPE,uint64_t,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        case PhysicalType::FLOAT:
            return templatedSelectOperationSwitchCommon<LEFT_TYPE,float,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        case PhysicalType::DOUBLE:
            return templatedSelectOperationSwitchCommon<LEFT_TYPE,double,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for select operation!");
    }
    return 0;
}
template <class OP>
idx_t templatedSelectOperationSwitchLeft(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel, SelectionVector *falseSel, idx_t& falseCount) {
    // cannot compare string with different types
    BB_ASSERT(left.getType() != PhysicalType::STRING && right.getType() != PhysicalType::STRING);
    // left type != right type
    switch (left.getType()) {
        case PhysicalType::TINYINT:
            return templatedSelectOperationSwitchRight<int8_t,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        case PhysicalType::SMALLINT:
            return templatedSelectOperationSwitchRight<int16_t,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        case PhysicalType::INTEGER:
            return templatedSelectOperationSwitchRight<int32_t,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        case PhysicalType::BIGINT:
            return templatedSelectOperationSwitchRight<int64_t,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        case PhysicalType::UTINYINT:
            return templatedSelectOperationSwitchRight<uint8_t,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        case PhysicalType::USMALLINT:
            return templatedSelectOperationSwitchRight<uint16_t,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        case PhysicalType::UINTEGER:
            return templatedSelectOperationSwitchRight<uint32_t,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        case PhysicalType::UBIGINT:
            return templatedSelectOperationSwitchRight<uint64_t,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        case PhysicalType::FLOAT:
            return templatedSelectOperationSwitchRight<float,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        case PhysicalType::DOUBLE:
            return templatedSelectOperationSwitchRight<double,OP>(left, right, sel, count, trueSel, falseSel, falseCount);
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for select operation!");
    }
    return 0;
}



idx_t VectorOperations::equals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel){
    idx_t falseCount = 0;
    if (left.getType() == right.getType())
        return templatedSelectOperationSwitchEqualType<Equals>(left, right, sel, count, trueSel, nullptr, falseCount);
    return templatedSelectOperationSwitchLeft<Equals>(left, right, sel, count, trueSel, nullptr, falseCount);
}
idx_t VectorOperations::notEquals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel){
    idx_t falseCount = 0;
    if (left.getType() == right.getType())
        return templatedSelectOperationSwitchEqualType<NotEquals>(left, right, sel, count, trueSel, nullptr, falseCount);
    return templatedSelectOperationSwitchLeft<NotEquals>(left, right, sel, count, trueSel, nullptr, falseCount);
}
idx_t VectorOperations::greaterThan(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel){
    idx_t falseCount = 0;
    if (left.getType() == right.getType())
        return templatedSelectOperationSwitchEqualType<GreaterThan>(left, right, sel, count, trueSel, nullptr, falseCount);
    return templatedSelectOperationSwitchLeft<GreaterThan>(left, right, sel, count, trueSel, nullptr, falseCount);

}
idx_t VectorOperations::greaterThanEquals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel){
    idx_t falseCount = 0;
    if (left.getType() == right.getType())
        return templatedSelectOperationSwitchEqualType<GreaterThanEquals>(left, right, sel, count, trueSel, nullptr, falseCount);
    return templatedSelectOperationSwitchLeft<GreaterThanEquals>(left, right, sel, count, trueSel, nullptr, falseCount);

}
idx_t VectorOperations::lessThan(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel){
    idx_t falseCount = 0;
    if (left.getType() == right.getType())
        return templatedSelectOperationSwitchEqualType<LessThan>(left, right, sel, count, trueSel, nullptr, falseCount);
    return templatedSelectOperationSwitchLeft<LessThan>(left, right, sel, count, trueSel, nullptr, falseCount);

}
idx_t VectorOperations::lessThanEquals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel){
    idx_t falseCount = 0;
    if (left.getType() == right.getType())
        return templatedSelectOperationSwitchEqualType<LessThanEquals>(left, right, sel, count, trueSel, nullptr, falseCount);
    return templatedSelectOperationSwitchLeft<LessThanEquals>(left, right, sel, count, trueSel, nullptr, falseCount);
}

idx_t VectorOperations::equals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel,SelectionVector *falseSel, idx_t& falseCount){
    if (left.getType() == right.getType())
        return templatedSelectOperationSwitchEqualType<Equals>(left, right, sel, count, trueSel, falseSel, falseCount);
    return templatedSelectOperationSwitchLeft<Equals>(left, right, sel, count, trueSel, falseSel, falseCount);
}

idx_t VectorOperations::notEquals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel,SelectionVector *falseSel, idx_t& falseCount){
    if (left.getType() == right.getType())
        return templatedSelectOperationSwitchEqualType<NotEquals>(left, right, sel, count, trueSel, falseSel, falseCount);
    return templatedSelectOperationSwitchLeft<NotEquals>(left, right, sel, count, trueSel, falseSel, falseCount);
}

idx_t VectorOperations::greaterThan(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel,SelectionVector *falseSel, idx_t& falseCount){
    if (left.getType() == right.getType())
        return templatedSelectOperationSwitchEqualType<GreaterThan>(left, right, sel, count, trueSel, falseSel, falseCount);
    return templatedSelectOperationSwitchLeft<GreaterThan>(left, right, sel, count, trueSel, falseSel, falseCount);
}

idx_t VectorOperations::greaterThanEquals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel,SelectionVector *falseSel, idx_t& falseCount){
    if (left.getType() == right.getType())
        return templatedSelectOperationSwitchEqualType<GreaterThanEquals>(left, right, sel, count, trueSel, falseSel, falseCount);
    return templatedSelectOperationSwitchLeft<GreaterThanEquals>(left, right, sel, count, trueSel, falseSel, falseCount);
}

idx_t VectorOperations::lessThan(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel,SelectionVector *falseSel, idx_t& falseCount){
    if (left.getType() == right.getType())
        return templatedSelectOperationSwitchEqualType<LessThan>(left, right, sel, count, trueSel, falseSel, falseCount);
    return templatedSelectOperationSwitchLeft<LessThan>(left, right, sel, count, trueSel, falseSel, falseCount);
}

idx_t VectorOperations::lessThanEquals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel,SelectionVector *falseSel, idx_t& falseCount){
    if (left.getType() == right.getType())
        return templatedSelectOperationSwitchEqualType<LessThanEquals>(left, right, sel, count, trueSel, falseSel, falseCount);
    return templatedSelectOperationSwitchLeft<LessThanEquals>(left, right, sel, count, trueSel, falseSel, falseCount);
}

}