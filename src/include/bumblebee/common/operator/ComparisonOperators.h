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
#include "bumblebee/common/types/BumbleString.h"

namespace bumblebee{

//===--------------------------------------------------------------------===//
// Comparison Operations
//===--------------------------------------------------------------------===//
struct Equals {
	template <class T>
	static inline bool operation(T left, T right) {
		return left == right;
	}
};

struct NotEquals {
	template <class T>
	static inline bool operation(T left, T right) {
		return left != right;
	}
};
struct GreaterThan {
	template <class T>
	static inline bool operation(T left, T right) {
		return left > right;
	}
};
struct GreaterThanEquals {
	template <class T>
	static inline bool operation(T left, T right) {
		return left >= right;
	}
};

struct LessThan {
	template <class T>
	static inline bool operation(T left, T right) {
		return left < right;
	}
};
struct LessThanEquals {
	template <class T>
	static inline bool operation(T left, T right) {
		return left <= right;
	}
};

//===--------------------------------------------------------------------===//
// Specialized Boolean Comparison Operators
//===--------------------------------------------------------------------===//
template <>
inline bool GreaterThan::operation(bool left, bool right) {
	return !right && left;
}
template <>
inline bool LessThan::operation(bool left, bool right) {
	return !left && right;
}
//===--------------------------------------------------------------------===//
// Specialized String Comparison operations
//===--------------------------------------------------------------------===//
struct StringComparisonOperators {
	template <bool INVERSE>
	static inline bool EqualsOrNot(const string_t a, const string_t b) {
		auto size = a.size();
		if (size != b.length()) return INVERSE ? true : false;
		if (a.isInlined()) {
			// small string: compare entire string
			if (memcmp(a.getPrefix(), b.getPrefix(), size) == 0) {
				// entire string is equal
				return INVERSE ? false : true;
			}
		} else {
			// large string: first check prefix and length
			if (memcmp(a.getPrefix(), b.getPrefix(), string_t::PREFIX_LENGTH) == 0) {
				// prefix and length are equal: check main string
				if (memcmp(a.c_str(), b.c_str(), size) == 0) {
					// entire string is equal
					return INVERSE ? false : true;
				}
			}
		}
		// not equal
		return INVERSE ? true : false;
	}
};

template <>
inline bool Equals::operation(string_t left, string_t right) {
	return StringComparisonOperators::EqualsOrNot<false>(left, right);
}
template <>
inline bool NotEquals::operation(string_t left, string_t right) {
	return StringComparisonOperators::EqualsOrNot<true>(left, right);
}

// compare up to shared length. if still the same, compare lengths
template <class OP>
static bool templated_string_compare_op(string_t left, string_t right) {
	auto memcmp_res =
	    memcmp(left.getDataUnsafe(), right.getDataUnsafe(), std::min(left.size(), right.size()));
	auto final_res = memcmp_res == 0 ? OP::operation(left.size(), right.size()) : OP::operation(memcmp_res, 0);
	return final_res;
}

template <>
inline bool GreaterThan::operation(string_t left, string_t right) {
	return templated_string_compare_op<GreaterThan>(left, right);
}

template <>
inline bool GreaterThanEquals::operation(string_t left, string_t right) {
	return templated_string_compare_op<GreaterThanEquals>(left, right);
}

template <>
inline bool LessThan::operation(string_t left, string_t right) {
	return templated_string_compare_op<LessThan>(left, right);
}

template <>
inline bool LessThanEquals::operation(string_t left, string_t right) {
	return templated_string_compare_op<LessThanEquals>(left, right);
}


}
