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
#pragma once
#include "bumblebee/common/TypeDefs.hpp"
#include "bumblebee/common/types/Vector.hpp"

namespace bumblebee {
// Contains binary operations between two Vectors
// Execute is for operation that generate new data (+, -, * etc.)
// Select filter data ( >, < , <= etc.)
struct BinaryExecution {

protected:
	template <class LEFT_TYPE, class RIGHT_TYPE, class RESULT_TYPE, class OP,
	          bool LEFT_CONSTANT, bool RIGHT_CONSTANT>
	static void executeFlatLoop(LEFT_TYPE *__restrict ldata, RIGHT_TYPE *__restrict rdata,
	                            RESULT_TYPE *__restrict result_data, idx_t count) {

		for (idx_t i = 0; i < count; i++) {
			auto lentry = ldata[LEFT_CONSTANT ? 0 : i];
			auto rentry = rdata[RIGHT_CONSTANT ? 0 : i];
			result_data[i] = OP::operation(lentry, rentry);
		}

	}

	template <class LEFT_TYPE, class RIGHT_TYPE, class RESULT_TYPE, class OP,
			  bool LEFT_CONSTANT, bool RIGHT_CONSTANT>
	static void executeFlat(Vector &left, Vector &right, Vector &result, idx_t count) {
		BB_ASSERT(!LEFT_CONSTANT || !RIGHT_CONSTANT );
		result.setVectorType(VectorType::FLAT_VECTOR);
		auto result_data = FlatVector::getData<RESULT_TYPE>(result);
		auto ldata = FlatVector::getData<LEFT_TYPE>(left);
		auto rdata = FlatVector::getData<RIGHT_TYPE>(right);
		executeFlatLoop<LEFT_TYPE, RIGHT_TYPE, RESULT_TYPE, OP, LEFT_CONSTANT, RIGHT_CONSTANT>(ldata, rdata, result_data, count);
	}

	template <class LEFT_TYPE, class RIGHT_TYPE, class RESULT_TYPE, class OP>
	static void executeConstant(Vector &left, Vector &right, Vector &result) {
		result.setVectorType(VectorType::CONSTANT_VECTOR);

		auto ldata = ConstantVector::getData<LEFT_TYPE>(left);
		auto rdata = ConstantVector::getData<RIGHT_TYPE>(right);
		auto result_data = ConstantVector::getData<RESULT_TYPE>(result);

		*result_data = OP::operation(*ldata, *rdata);
	}

	template <class LEFT_TYPE, class RIGHT_TYPE, class RESULT_TYPE, class OP>
	static void ExecuteGenericLoop(LEFT_TYPE *__restrict ldata, RIGHT_TYPE *__restrict rdata,
								   RESULT_TYPE *__restrict result_data, const SelectionVector *__restrict lsel,
								   const SelectionVector *__restrict rsel, idx_t count) {

		for (idx_t i = 0; i < count; i++) {
			auto lentry = ldata[lsel->getIndex(i)];
			auto rentry = rdata[rsel->getIndex(i)];
			result_data[i] = OP::operation(lentry, rentry);
		}

	}

	template <class LEFT_TYPE, class RIGHT_TYPE, class RESULT_TYPE, class OP>
	static void executeGeneric(Vector &left, Vector &right, Vector &result, idx_t count) {
		VectorData ldata, rdata;

		left.orrify(count, ldata);
		right.orrify(count, rdata);

		result.setVectorType(VectorType::FLAT_VECTOR);
		auto result_data = FlatVector::getData<RESULT_TYPE>(result);
		ExecuteGenericLoop<LEFT_TYPE, RIGHT_TYPE, RESULT_TYPE, OP>(
			(LEFT_TYPE *)ldata.data_, (RIGHT_TYPE *)rdata.data_, result_data, ldata.sel_, rdata.sel_, count);
	}

	template <class LEFT_TYPE, class RIGHT_TYPE, class RESULT_TYPE, class OP>
	static void executeSwitch(Vector &left, Vector &right, Vector &result, idx_t count) {
		auto left_vector_type = left.getVectorType();
		auto right_vector_type = right.getVectorType();
		if (left_vector_type == VectorType::CONSTANT_VECTOR && right_vector_type == VectorType::CONSTANT_VECTOR) {
			executeConstant<LEFT_TYPE, RIGHT_TYPE, RESULT_TYPE, OP>(left, right, result);
		} else if (left_vector_type == VectorType::FLAT_VECTOR && right_vector_type == VectorType::CONSTANT_VECTOR) {
			executeFlat<LEFT_TYPE, RIGHT_TYPE, RESULT_TYPE, OP, false, true>(left, right, result, count);
		} else if (left_vector_type == VectorType::CONSTANT_VECTOR && right_vector_type == VectorType::FLAT_VECTOR) {
			executeFlat<LEFT_TYPE, RIGHT_TYPE, RESULT_TYPE, OP, true, false>(left, right, result, count);
		} else if (left_vector_type == VectorType::FLAT_VECTOR && right_vector_type == VectorType::FLAT_VECTOR) {
			executeFlat<LEFT_TYPE, RIGHT_TYPE, RESULT_TYPE, OP, false, false>(left, right, result, count);
		} else {
			executeGeneric<LEFT_TYPE, RIGHT_TYPE, RESULT_TYPE, OP>(left, right, result, count);
		}
	}

protected:
	template <class LEFT_TYPE, class RIGHT_TYPE, class OP>
	static idx_t selectConstant(Vector &left, Vector &right, const SelectionVector *sel, idx_t count,
								SelectionVector *trueSel,SelectionVector *falseSel, idx_t& false_count) {
		auto ldata = ConstantVector::getData<LEFT_TYPE>(left);
		auto rdata = ConstantVector::getData<RIGHT_TYPE>(right);

		// both sides are constant, return either 0 or the count
		// in this case we do not fill in the result selection vector at all
		if (! OP::operation(*ldata, *rdata)) {
			if (falseSel) {
				for (idx_t i = 0; i < count; i++) {
					falseSel->setIndex(i, sel->getIndex(i));
				}
			}
			false_count = count;
			return 0;
		}
		if (trueSel) {
			for (idx_t i = 0; i < count; i++) {
				trueSel->setIndex(i, sel->getIndex(i));
			}
		}
		false_count = 0;
		return count;

	}

	template <class LEFT_TYPE, class RIGHT_TYPE, class OP, bool LEFT_CONSTANT, bool RIGHT_CONSTANT, bool HAS_TRUE_SEL, bool HAS_FALSE_SEL>
	static inline idx_t selectFlatLoop(LEFT_TYPE *__restrict ldata, RIGHT_TYPE *__restrict rdata,
	                                   const SelectionVector *sel, idx_t count,
	                                   SelectionVector *trueSel,SelectionVector *falseSel, idx_t& false_count) {
		idx_t true_count = 0;
		false_count = 0;
		for (idx_t idx = 0; idx < count; idx++) {
			idx_t lidx = LEFT_CONSTANT ? 0 : sel->getIndex(idx);
			idx_t ridx = RIGHT_CONSTANT ? 0 : sel->getIndex(idx);
			bool comparison_result = OP::operation(ldata[lidx], rdata[ridx]);
			if (HAS_TRUE_SEL) {
				trueSel->setIndex(true_count, sel->getIndex(idx));
			}
			if (HAS_FALSE_SEL) {
				falseSel->setIndex(false_count, sel->getIndex(idx));
			}
			true_count += comparison_result;
			false_count += !comparison_result;
		}
		return true_count;
	}


	template <class LEFT_TYPE, class RIGHT_TYPE, class OP, bool LEFT_CONSTANT, bool RIGHT_CONSTANT>
	static inline idx_t selectFlat(Vector &left, Vector &right, const SelectionVector *sel, idx_t count,
	                        SelectionVector *trueSel,SelectionVector *falseSel, idx_t& false_count) {
		auto ldata = FlatVector::getData<LEFT_TYPE>(left);
		auto rdata = FlatVector::getData<RIGHT_TYPE>(right);
		if (trueSel && falseSel)
			return selectFlatLoop<LEFT_TYPE, RIGHT_TYPE, OP, LEFT_CONSTANT, RIGHT_CONSTANT, true, true>(
			    ldata, rdata, sel, count, trueSel, falseSel, false_count);
		if (trueSel)
			return selectFlatLoop<LEFT_TYPE, RIGHT_TYPE, OP, LEFT_CONSTANT, RIGHT_CONSTANT, true, false>(
				ldata, rdata, sel, count, trueSel, falseSel, false_count);
		if (falseSel)
			return selectFlatLoop<LEFT_TYPE, RIGHT_TYPE, OP, LEFT_CONSTANT, RIGHT_CONSTANT, false, true>(
				ldata, rdata, sel, count, trueSel, falseSel, false_count);
		return selectFlatLoop<LEFT_TYPE, RIGHT_TYPE, OP, LEFT_CONSTANT, RIGHT_CONSTANT, false, false>(
				ldata, rdata, sel, count, trueSel, falseSel, false_count);

	}




	template <class LEFT_TYPE, class RIGHT_TYPE, class OP, bool HAS_TRUE_SEL,bool HAS_FALSE_SEL >
	static inline idx_t
	selectGenericLoop(LEFT_TYPE *__restrict ldata, RIGHT_TYPE *__restrict rdata, const SelectionVector *__restrict lsel,
	                  const SelectionVector *__restrict rsel, const SelectionVector *__restrict sel, idx_t count,
	                  SelectionVector *trueSel,SelectionVector *falseSel, idx_t& false_count) {

		idx_t true_count = 0;
		false_count = 0;
		for (idx_t i = 0; i < count; i++) {
			auto idx = sel->getIndex(i);
			auto lindex = lsel->getIndex(idx);
			auto rindex = rsel->getIndex(idx);
			bool comparison_result = OP::operation(ldata[lindex], rdata[rindex]);
			// write same index if does not match ( branchless operation ;) )
			if (HAS_TRUE_SEL) {
				trueSel->setIndex(true_count, idx);
			}
			if (HAS_FALSE_SEL)
				falseSel->setIndex(false_count, idx);
			true_count += comparison_result;
			false_count += !comparison_result;
		}
		return true_count;
	}

	template <class LEFT_TYPE, class RIGHT_TYPE, class OP>
	static inline idx_t selectGenericLoopSelSwitch(LEFT_TYPE *__restrict ldata, RIGHT_TYPE *__restrict rdata,
						   const SelectionVector *__restrict lsel, const SelectionVector *__restrict rsel,
						   const SelectionVector *__restrict resultSel, idx_t count, SelectionVector *trueSel, SelectionVector *falseSel, idx_t& falseCount) {
		if (trueSel && falseSel)
			return selectGenericLoop<LEFT_TYPE, RIGHT_TYPE, OP, true, true>(
				ldata, rdata, lsel, rsel, resultSel, count,  trueSel, falseSel, falseCount);
		if (trueSel)
			return selectGenericLoop<LEFT_TYPE, RIGHT_TYPE, OP, true, false>(
				ldata, rdata, lsel, rsel, resultSel, count,  trueSel, falseSel, falseCount);
		if (falseSel)
			return selectGenericLoop<LEFT_TYPE, RIGHT_TYPE, OP, false, true>(
				ldata, rdata, lsel, rsel, resultSel, count,  trueSel, falseSel, falseCount);
		return selectGenericLoop<LEFT_TYPE, RIGHT_TYPE, OP, false, false>(
				ldata, rdata, lsel, rsel, resultSel, count, trueSel, falseSel, falseCount);

	}


	template <class LEFT_TYPE, class RIGHT_TYPE, class OP>
	static idx_t selectGeneric(Vector &left, Vector &right, const SelectionVector *sel, idx_t count,
	                           SelectionVector *trueSel, SelectionVector *falseSel, idx_t& falseCount) {
		VectorData ldata, rdata;

		left.orrify(count, ldata);
		right.orrify(count, rdata);

		return selectGenericLoopSelSwitch<LEFT_TYPE, RIGHT_TYPE, OP>((LEFT_TYPE *)ldata.data_, (RIGHT_TYPE *)rdata.data_,
		                                                          ldata.sel_, rdata.sel_, sel, count, trueSel, falseSel, falseCount);
	}

public:
	// Applies a binary arithmetic or logical operation (defined by the OP template) between two input vectors.
	// The result is stored in the output vector.
	//
	// Parameters:
	// - left:   Left-hand side input vector.
	// - right:  Right-hand side input vector.
	// - result: Output vector where the result of the operation is stored.
	// - count:  Number of elements to process.
	// Notes:
	// - This function handles all combinations of vector types (constant, flat, generic).
	// - OP must define a static method `operation(a, b)` implementing the desired operation (e.g., addition, multiplication).
	template <class LEFT_TYPE, class RIGHT_TYPE, class RESULT_TYPE, class OP>
	static void execute(Vector &left, Vector &right, Vector &result, idx_t count) {
		executeSwitch<LEFT_TYPE, RIGHT_TYPE, RESULT_TYPE, OP>(left, right, result, count);
	}

	// Applies a binary comparison operation (defined by the OP template) between two input vectors,
	// returning the number of elements that satisfy the condition. Optionally outputs the indices of matching rows.
	//
	// Parameters:
	// - left:     Left-hand side input vector.
	// - right:    Right-hand side input vector.
	// - sel:      Optional input selection vector (can be nullptr to use full input).
	// - count:    Number of elements to process.
	// - trueSel:  Optional output selection vector for indices of elements satisfying the condition.
	//
	// Returns:
	// - The number of elements that satisfy the condition defined by OP.
	//
	// Notes:
	// - OP must define a static method `operation(a, b)` returning a boolean result.
	// - Supports all vector types and combinations (constant, flat, generic).
	// - If `trueSel` is nullptr, only the count of matching elements is returned without writing any indices.
    template <class LEFT_TYPE, class RIGHT_TYPE, class OP>
    static idx_t select(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel, SelectionVector *falseSel, idx_t& falseCount) {
        if (!sel) {
			sel = &FlatVector::INCREMENTAL_SELECTION_VECTOR;
		}
		if (left.getVectorType() == VectorType::CONSTANT_VECTOR &&
		    right.getVectorType() == VectorType::CONSTANT_VECTOR) {
			return selectConstant<LEFT_TYPE, RIGHT_TYPE, OP>(left, right, sel, count, trueSel, falseSel, falseCount);
		} else if (left.getVectorType() == VectorType::CONSTANT_VECTOR &&
		           right.getVectorType() == VectorType::FLAT_VECTOR) {
			return selectFlat<LEFT_TYPE, RIGHT_TYPE, OP, true, false>(left, right, sel, count, trueSel, falseSel, falseCount);
		} else if (left.getVectorType() == VectorType::FLAT_VECTOR &&
		           right.getVectorType() == VectorType::CONSTANT_VECTOR) {
			return selectFlat<LEFT_TYPE, RIGHT_TYPE, OP, false, true>(left, right, sel, count, trueSel, falseSel, falseCount);
		} else if (left.getVectorType() == VectorType::FLAT_VECTOR &&
		           right.getVectorType() == VectorType::FLAT_VECTOR) {
			return selectFlat<LEFT_TYPE, RIGHT_TYPE, OP, false, false>(left, right, sel, count, trueSel, falseSel, falseCount);
		} else {
			return selectGeneric<LEFT_TYPE, RIGHT_TYPE, OP>(left, right, sel, count, trueSel, falseSel, falseCount);
		}
    };

};
}