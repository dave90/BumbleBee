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

#include "bumblebee/common/TypeDefs.hpp"
#include "bumblebee/common/types/Vector.hpp"
#include "bumblebee/execution/NestedLoopJoin.hpp"

#include "bumblebee/common/operator/ComparisonOperators.hpp"

namespace bumblebee {

struct InitNestedLoopJoin {

    template <class LEFT_TYPE,class RIGHT_TYPE, class OP>
    static idx_t operation( Vector& left, Vector& right,
        idx_t lsize, idx_t rsize, idx_t &lpos, idx_t &rpos,
        SelectionVector& lsel, SelectionVector& rsel, idx_t currentMatch) {

        VectorData left_data, right_data;
        left.orrify(lsize, left_data);
        right.orrify(rsize, right_data);

        auto ldata = (LEFT_TYPE*) left_data.data_;
        auto ldatasel = left_data.sel_;
        auto rdata = (RIGHT_TYPE*) right_data.data_;
        auto rdatasel = right_data.sel_;

        idx_t result_count = 0;
        for (; rpos < rsize; rpos++) {
            idx_t rposition = rdatasel->getIndex(rpos);
            for (; lpos < lsize; lpos++) {
                if (result_count == STANDARD_VECTOR_SIZE) {
                    // out of space!
                    return result_count;
                }
                idx_t lposition = ldatasel->getIndex(lpos);
                if (OP::operation(ldata[lposition], rdata[rposition])) {
                    // emit tuple
                    lsel.setIndex(result_count, lpos);
                    rsel.setIndex(result_count, rpos);
                    result_count++;
                }
            }
            lpos = 0;
        }
        return result_count;
    }

};

struct RefineNestedLoopJoin {

    template <class LEFT_TYPE,class RIGHT_TYPE, class OP>
    static idx_t operation( Vector& left, Vector& right,
        idx_t lsize, idx_t rsize, idx_t &lpos, idx_t &rpos,
        SelectionVector& lsel, SelectionVector& rsel, idx_t currentMatch) {

        VectorData left_data, right_data;
        left.orrify(lsize, left_data);
        right.orrify(rsize, right_data);

        BB_ASSERT(currentMatch > 0);

        auto ldata = (LEFT_TYPE*) left_data.data_;
        auto ldatasel = left_data.sel_;
        auto rdata = (RIGHT_TYPE*) right_data.data_;
        auto rdatasel = right_data.sel_;

        idx_t result_count = 0;
        for (idx_t i = 0; i < currentMatch; i++) {
            auto lidx = lsel.getIndex(i);
            auto ridx = rsel.getIndex(i);
            auto left_idx = ldatasel->getIndex(lidx);
            auto right_idx = rdatasel->getIndex(ridx);
            if (OP::operation(ldata[left_idx], rdata[right_idx])) {
                lsel.setIndex(result_count, lidx);
                rsel.setIndex(result_count, ridx);
                result_count++;
            }
        }
        return result_count;

    }

};

template <class NLTYPE, class OP>
idx_t nestedLoopJoinEqualTypeSwitch(Vector &left, Vector &right, idx_t lsize, idx_t rsize, idx_t &lpos,
                                     idx_t &rpos, SelectionVector &lsel, SelectionVector &rsel,
                                     idx_t currentMatch) {
    BB_ASSERT(left.getType() == right.getType());
    switch (left.getType()) {
        case PhysicalType::TINYINT:
            return NLTYPE::template operation<int8_t,int8_t, OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case PhysicalType::SMALLINT:
            return NLTYPE::template operation<int16_t,int16_t,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case PhysicalType::INTEGER:
            return NLTYPE::template operation<int32_t,int32_t,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case PhysicalType::BIGINT:
            return NLTYPE::template operation<int64_t,int64_t,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case PhysicalType::UTINYINT:
            return NLTYPE::template operation<uint8_t,uint8_t,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case PhysicalType::USMALLINT:
            return NLTYPE::template operation<uint16_t,uint16_t,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case PhysicalType::UINTEGER:
            return NLTYPE::template operation<uint32_t,uint32_t,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case PhysicalType::UBIGINT:
            return NLTYPE::template operation<uint64_t,uint64_t,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case PhysicalType::FLOAT:
            return NLTYPE::template operation<float,float,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case PhysicalType::DOUBLE:
            return NLTYPE::template operation<double,double,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case PhysicalType::STRING:	{
            return NLTYPE::template operation<string_t,string_t,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
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

template <class NLTYPE, class LEFT_TYPE, class RIGHT_TYPE , class OP>
idx_t nestedLoopJoinCommonTypeSwitch(Vector &left, Vector &right, idx_t lsize, idx_t rsize, idx_t &lpos,
                                         idx_t &rpos, SelectionVector &lsel, SelectionVector &rsel,
                                         idx_t currentMatch) {
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
            return NLTYPE::template operation<LEFT_TYPE,RIGHT_TYPE, ComparisonCommonCast<LEFT_TYPE, RIGHT_TYPE, int64_t, OP>>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case PhysicalType::FLOAT:
        case PhysicalType::DOUBLE:
            return NLTYPE::template operation<LEFT_TYPE,RIGHT_TYPE, ComparisonCommonCast<LEFT_TYPE, RIGHT_TYPE, double, OP>>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for select operation!");
    }
    return 0;
}

template <class NLTYPE, class LEFT_TYPE, class OP>
idx_t nestedLoopJoinRightTypeSwitch(Vector &left, Vector &right, idx_t lsize, idx_t rsize, idx_t &lpos,
                                         idx_t &rpos, SelectionVector &lsel, SelectionVector &rsel,
                                         idx_t currentMatch) {
    switch (right.getType()) {
        case PhysicalType::TINYINT:
            return nestedLoopJoinCommonTypeSwitch<NLTYPE,LEFT_TYPE,int8_t,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case PhysicalType::SMALLINT:
            return nestedLoopJoinCommonTypeSwitch<NLTYPE,LEFT_TYPE,int16_t,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case PhysicalType::INTEGER:
            return nestedLoopJoinCommonTypeSwitch<NLTYPE,LEFT_TYPE,int32_t,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case PhysicalType::BIGINT:
            return nestedLoopJoinCommonTypeSwitch<NLTYPE,LEFT_TYPE,int64_t,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case PhysicalType::UTINYINT:
            return nestedLoopJoinCommonTypeSwitch<NLTYPE,LEFT_TYPE,uint8_t,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case PhysicalType::USMALLINT:
            return nestedLoopJoinCommonTypeSwitch<NLTYPE,LEFT_TYPE,uint16_t,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case PhysicalType::UINTEGER:
            return nestedLoopJoinCommonTypeSwitch<NLTYPE,LEFT_TYPE,uint32_t,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case PhysicalType::UBIGINT:
            return nestedLoopJoinCommonTypeSwitch<NLTYPE,LEFT_TYPE,uint64_t,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case PhysicalType::FLOAT:
            return nestedLoopJoinCommonTypeSwitch<NLTYPE,LEFT_TYPE,float,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case PhysicalType::DOUBLE:
            return nestedLoopJoinCommonTypeSwitch<NLTYPE,LEFT_TYPE,double,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for select operation!");
            return 0;
    }
}


template <class NLTYPE, class OP>
idx_t nestedLoopJoinLeftTypeSwitch(Vector &left, Vector &right, idx_t lsize, idx_t rsize, idx_t &lpos,
                                         idx_t &rpos, SelectionVector &lsel, SelectionVector &rsel,
                                         idx_t currentMatch) {
    // cannot compare string with different types
    BB_ASSERT(left.getType() != PhysicalType::STRING && right.getType() != PhysicalType::STRING);
    // left type != right type
    switch (left.getType()) {
        case PhysicalType::TINYINT:
            return nestedLoopJoinRightTypeSwitch<NLTYPE,int8_t,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case PhysicalType::SMALLINT:
            return nestedLoopJoinRightTypeSwitch<NLTYPE,int16_t,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case PhysicalType::INTEGER:
            return nestedLoopJoinRightTypeSwitch<NLTYPE,int32_t,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case PhysicalType::BIGINT:
            return nestedLoopJoinRightTypeSwitch<NLTYPE,int64_t,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case PhysicalType::UTINYINT:
            return nestedLoopJoinRightTypeSwitch<NLTYPE,uint8_t,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case PhysicalType::USMALLINT:
            return nestedLoopJoinRightTypeSwitch<NLTYPE,uint16_t,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case PhysicalType::UINTEGER:
            return nestedLoopJoinRightTypeSwitch<NLTYPE,uint32_t,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case PhysicalType::UBIGINT:
            return nestedLoopJoinRightTypeSwitch<NLTYPE,uint64_t,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case PhysicalType::FLOAT:
            return nestedLoopJoinRightTypeSwitch<NLTYPE,float,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case PhysicalType::DOUBLE:
            return nestedLoopJoinRightTypeSwitch<NLTYPE,double,OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for select operation!");
    }
    return 0;
}



template <class NLTYPE, class OP>
idx_t nestedLoopJoinTypeSwitch(Vector &left, Vector &right, idx_t lsize, idx_t rsize, idx_t &lpos,
                                     idx_t &rpos, SelectionVector &lsel, SelectionVector &rsel,
                                     idx_t currentMatch) {
    if (left.getType() == right.getType())
        return nestedLoopJoinEqualTypeSwitch<NLTYPE, OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
    return nestedLoopJoinLeftTypeSwitch<NLTYPE, OP>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
}

template <class NLTYPE>
idx_t nestedLoopJoinComparisonSwitch(Vector &left, Vector &right, idx_t lsize, idx_t rsize, idx_t &lpos,
                                     idx_t &rpos, SelectionVector &lsel, SelectionVector &rsel,
                                     idx_t currentMatch, Binop op) {
	switch (op) {
	    case EQUAL:
	        return nestedLoopJoinTypeSwitch<NLTYPE, Equals>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
        case LESS:
	        return nestedLoopJoinTypeSwitch<NLTYPE, LessThan>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
	    case GREATER:
	        return nestedLoopJoinTypeSwitch<NLTYPE, GreaterThan>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
	    case LESS_OR_EQ:
	        return nestedLoopJoinTypeSwitch<NLTYPE, LessThanEquals>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
	    case GREATER_OR_EQ:
	        return nestedLoopJoinTypeSwitch<NLTYPE, GreaterThanEquals>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);
	    case UNEQUAL:
	        return nestedLoopJoinTypeSwitch<NLTYPE, NotEquals>(left, right, lsize, rsize, lpos, rpos, lsel, rsel, currentMatch);

	    default:
		    ErrorHandler::errorNotImplemented("Error, join comparison not supported");
	        return 0;
	}
}


idx_t NestedLoopJoinInner::execute(idx_t &lpos, idx_t &rpos, DataChunk &lchunk, DataChunk &rchunk,
    SelectionVector &lsel, SelectionVector &rsel, const vector<Expression> &conditions) {
    BB_ASSERT(!conditions.empty());
    // accept only simplified conditions with 2 columns
    for (auto& condition : conditions)
        BB_ASSERT(condition.left_.cols_.size() == 1 && condition.left_.cols_.size() == condition.right_.cols_.size());

    if (lpos >= lchunk.getSize() || rpos >= rchunk.getSize()) return 0;

    // for first tuple we execute the InitNestedLoopJoin to init the selection vectors
    auto& icondition = conditions[0];

    auto lidx = icondition.left_.cols_[0];
    auto ridx = icondition.right_.cols_[0];


    BB_ASSERT(rchunk.columnCount() > ridx);
    auto matchCount = nestedLoopJoinComparisonSwitch<InitNestedLoopJoin>( lchunk.data_[lidx], rchunk.data_[ridx],
        lchunk.getSize(), rchunk.getSize(),lpos, rpos, lsel, rsel, 0, icondition.op_);

    for (idx_t i = 1; i < conditions.size(); i++) {
        if (matchCount == 0)
            return 0;
        auto& condition = conditions[i];
        lidx = condition.left_.cols_[0];
        ridx = condition.right_.cols_[0];

        // now refine the join with the condition
        matchCount = nestedLoopJoinComparisonSwitch<RefineNestedLoopJoin>( lchunk.data_[lidx], rchunk.data_[ridx],
            lchunk.getSize(), rchunk.getSize(),lpos, rpos, lsel, rsel, matchCount, condition.op_);

    }
    return matchCount;
}
}
