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
#include "bumblebee/common/types/Vector.hpp"

namespace bumblebee{


struct UnaryOperatorWrapper {
	template <class OP, class INPUT_TYPE, class RESULT_TYPE>
	static inline RESULT_TYPE operation(INPUT_TYPE input, idx_t idx, void *dataptr) {
		return OP::template operation<INPUT_TYPE, RESULT_TYPE>(input);
	}
};

struct UnaryLambdaWrapper {
	template <class FUNC, class INPUT_TYPE, class RESULT_TYPE>
	static inline RESULT_TYPE operation(INPUT_TYPE input, idx_t idx, void *dataptr) {
		auto fun = (FUNC *)dataptr;
		return (*fun)(input);
	}
};

struct GenericUnaryWrapper {
	template <class OP, class INPUT_TYPE, class RESULT_TYPE>
	static inline RESULT_TYPE operation(INPUT_TYPE input, idx_t idx, void *dataptr) {
		return OP::template operation<INPUT_TYPE, RESULT_TYPE>(input, dataptr);
	}
};

// for the string we need to pass a vector because contains the string heap
template <class OP>
struct UnaryStringOperator {
	template <class INPUT_TYPE, class RESULT_TYPE>
	static RESULT_TYPE operation(INPUT_TYPE input, idx_t idx, void *dataptr) {
		auto vector = (Vector *)dataptr;
		return OP::template operation<INPUT_TYPE, RESULT_TYPE>(input, *vector);
	}
};

struct UnaryExecution {
private:
	template <class INPUT_TYPE, class RESULT_TYPE, class OPWRAPPER, class OP>
	static inline void executeLoop(INPUT_TYPE *__restrict ldata, RESULT_TYPE *__restrict result_data, idx_t count,
	                               const SelectionVector *__restrict sel_vector,
	                               void *dataptr) {

		for (idx_t i = 0; i < count; i++) {
			auto idx = sel_vector->getIndex(i);
			result_data[i] =
			    OPWRAPPER::template operation<OP, INPUT_TYPE, RESULT_TYPE>(ldata[idx], i, dataptr);
		}
	}

	template <class INPUT_TYPE, class RESULT_TYPE, class OPWRAPPER, class OP>
	static inline void executeFlat(INPUT_TYPE *__restrict ldata, RESULT_TYPE *__restrict result_data, idx_t count,
	                               void *dataptr) {

			for (idx_t i = 0; i < count; i++) {
				result_data[i] =
				    OPWRAPPER::template operation<OP, INPUT_TYPE, RESULT_TYPE>(ldata[i], i, dataptr);
			}

	}

	template <class INPUT_TYPE, class RESULT_TYPE, class OPWRAPPER, class OP>
	static inline void executeStandard(Vector &input, Vector &result, idx_t count, void *dataptr) {
		switch (input.getVectorType()) {
		case VectorType::CONSTANT_VECTOR: {
			result.setVectorType(VectorType::CONSTANT_VECTOR);
			auto result_data = ConstantVector::getData<RESULT_TYPE>(result);
			auto ldata = ConstantVector::getData<INPUT_TYPE>(input);

			*result_data = OPWRAPPER::template operation<OP, INPUT_TYPE, RESULT_TYPE>(*ldata, 0, dataptr);
			break;
		}
		case VectorType::FLAT_VECTOR: {
			result.setVectorType(VectorType::FLAT_VECTOR);
			auto result_data = FlatVector::getData<RESULT_TYPE>(result);
			auto ldata = FlatVector::getData<INPUT_TYPE>(input);

			executeFlat<INPUT_TYPE, RESULT_TYPE, OPWRAPPER, OP>(ldata, result_data, count, dataptr);
			break;
		}
		default: {
			VectorData vdata;
			input.orrify(count, vdata);

			result.setVectorType(VectorType::FLAT_VECTOR);
			auto result_data = FlatVector::getData<RESULT_TYPE>(result);
			auto ldata = (INPUT_TYPE *)vdata.data_;

			executeLoop<INPUT_TYPE, RESULT_TYPE, OPWRAPPER, OP>(ldata, result_data, count, vdata.sel_, dataptr);
			break;
		}
		}
	}

	template <class INPUT_TYPE, class OPWRAPPER, class OP>
	static idx_t selectConstant(Vector &input, const SelectionVector *sel, idx_t count,
								SelectionVector *trueSel,SelectionVector *falseSel, idx_t& false_count, void *dataptr) {
		auto idata = ConstantVector::getData<INPUT_TYPE>(input);

		if (! OPWRAPPER::template operation<OP, INPUT_TYPE, bool>(*idata, 0, dataptr)) {
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

	template <class INPUT_TYPE, class OPWRAPPER, class OP, bool HAS_TRUE_SEL, bool HAS_FALSE_SEL>
	static idx_t selectFlatLoop(INPUT_TYPE *__restrict idata, const SelectionVector *sel, idx_t count,
								SelectionVector *trueSel, SelectionVector *falseSel, idx_t& false_count, void *dataptr) {
		idx_t true_count = 0;
		false_count = 0;
		for (idx_t idx = 0; idx < count; idx++) {
			auto comparison_result = OPWRAPPER::template operation<OP, INPUT_TYPE, bool>(idata[sel->getIndex(idx)], 0, dataptr);
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


	template <class INPUT_TYPE, class OPWRAPPER, class OP>
	static idx_t selectFlat(INPUT_TYPE *__restrict idata, const SelectionVector *sel, idx_t count,
								SelectionVector *trueSel, SelectionVector *falseSel, idx_t& false_count, void *dataptr) {
		if (trueSel && falseSel)
			return selectFlatLoop<INPUT_TYPE, OPWRAPPER, OP, true, true>(idata, sel, count, trueSel, falseSel, false_count, dataptr);
		if (trueSel)
			return selectFlatLoop<INPUT_TYPE, OPWRAPPER, OP, true, false>(idata, sel, count, trueSel, falseSel, false_count, dataptr);
		if (falseSel)
			return selectFlatLoop<INPUT_TYPE, OPWRAPPER, OP, false, true>(idata, sel, count, trueSel, falseSel, false_count, dataptr);
		return selectFlatLoop<INPUT_TYPE, OPWRAPPER, OP, false, false>(idata, sel, count, trueSel, falseSel, false_count, dataptr);
	}

	template <class INPUT_TYPE, class OPWRAPPER, class OP, bool HAS_TRUE_SEL, bool HAS_FALSE_SEL>
	static idx_t selectGenericLoop(INPUT_TYPE *__restrict idata, const SelectionVector *sel, const SelectionVector *isel, idx_t count,
							SelectionVector *trueSel, SelectionVector *falseSel, idx_t& false_count, void *dataptr) {

		idx_t true_count = 0;
		false_count = 0;
		for (idx_t i = 0; i < count; i++) {
			auto idx = sel->getIndex(i);
			auto iindex = isel->getIndex(idx);
			auto comparison_result = OPWRAPPER::template operation<OP, INPUT_TYPE, bool>(idata[iindex], 0, dataptr);
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

	template <class INPUT_TYPE, class OPWRAPPER, class OP>
	static idx_t selectGeneric(INPUT_TYPE *__restrict idata, const SelectionVector *sel,  const SelectionVector * isel, idx_t count,
							SelectionVector *trueSel, SelectionVector *falseSel, idx_t& false_count, void *dataptr) {
		if (trueSel && falseSel)
			return selectGenericLoop<INPUT_TYPE, OPWRAPPER, OP, true, true>(idata, sel, isel, count, trueSel, falseSel, false_count, dataptr);
		if (trueSel)
			return selectGenericLoop<INPUT_TYPE, OPWRAPPER, OP, true, false>(idata, sel, isel, count, trueSel, falseSel, false_count, dataptr);
		if (falseSel)
			return selectGenericLoop<INPUT_TYPE, OPWRAPPER, OP, false, true>(idata, sel, isel, count, trueSel, falseSel, false_count, dataptr);
		return selectGenericLoop<INPUT_TYPE, OPWRAPPER, OP, false, false>(idata, sel, isel, count, trueSel, falseSel, false_count, dataptr);
	}


	template <class INPUT_TYPE, class OPWRAPPER, class OP>
	static inline idx_t executeSelect(Vector &input, const SelectionVector *sel, idx_t count, SelectionVector *trueSel, SelectionVector *falseSel, idx_t& falseCount, void *dataptr) {
		if (!sel) {
			sel = &FlatVector::INCREMENTAL_SELECTION_VECTOR;
		}
		switch (input.getVectorType()) {
			case VectorType::CONSTANT_VECTOR: {
				return selectConstant<INPUT_TYPE, OPWRAPPER, OP>(input, sel, count, trueSel, falseSel, falseCount, dataptr);
			}
			case VectorType::FLAT_VECTOR: {
				auto idata = FlatVector::getData<INPUT_TYPE>(input);
				return selectFlat<INPUT_TYPE, OPWRAPPER, OP>(idata, sel, count, trueSel, falseSel, falseCount, dataptr);
			}
			default: {
				VectorData vdata;
				input.orrify(count, vdata);
				auto idata = (INPUT_TYPE *)vdata.data_;
				return selectGeneric<INPUT_TYPE, OPWRAPPER, OP>(idata, sel, vdata.sel_, count, trueSel, falseSel, falseCount, dataptr);
			}
		}
	}





public:
	template <class INPUT_TYPE, class FUNC = std::function<bool(INPUT_TYPE)>>
	static idx_t select(Vector &input, const SelectionVector *sel, idx_t count, SelectionVector *trueSel, SelectionVector *falseSel, idx_t& falseCount, FUNC fun ) {
		return executeSelect<INPUT_TYPE, UnaryLambdaWrapper, FUNC>(input, sel, count, trueSel, falseSel, falseCount, (void *)&fun);
	}

	template <class INPUT_TYPE, class RESULT_TYPE, class OP>
	static void execute(Vector &input, Vector &result, idx_t count) {
		executeStandard<INPUT_TYPE, RESULT_TYPE, UnaryOperatorWrapper, OP>(input, result, count, nullptr);
	}

	template <class INPUT_TYPE, class RESULT_TYPE, class FUNC = std::function<RESULT_TYPE(INPUT_TYPE)>>
	static void execute(Vector &input, Vector &result, idx_t count, FUNC fun) {
		executeStandard<INPUT_TYPE, RESULT_TYPE, UnaryLambdaWrapper, FUNC>(input, result, count, (void *)&fun);
	}

	template <class INPUT_TYPE, class RESULT_TYPE, class OP>
	static void genericExecute(Vector &input, Vector &result, idx_t count, void *dataptr) {
		executeStandard<INPUT_TYPE, RESULT_TYPE, GenericUnaryWrapper, OP>(input, result, count, dataptr);
	}

	template <class INPUT_TYPE, class RESULT_TYPE, class OP>
	static void executeString(Vector &input, Vector &result, idx_t count) {
		genericExecute<string_t, string_t, UnaryStringOperator<OP>>(input, result, count,
		                                                                           (void *)&result);
	}
};

};

