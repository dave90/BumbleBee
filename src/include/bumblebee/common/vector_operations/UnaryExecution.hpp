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
	static inline RESULT_TYPE Operation(INPUT_TYPE input, idx_t idx, void *dataptr) {
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

public:
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

