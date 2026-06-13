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
#include "bumblebee/common/types/BumbleString.hpp"

namespace bumblebee{

//===--------------------------------------------------------------------===//
// Comparison Operations
//===--------------------------------------------------------------------===//
struct Equals {
	template <class T>
	static inline bool operation(const T& left, const T& right) {
		return left == right;
	}
};

struct NotEquals {
	template <class T>
	static inline bool operation(const T& left, const T& right) {
		return left != right;
	}
};
struct GreaterThan {
	template <class T>
	static inline bool operation(const T& left, const T& right) {
		return left > right;
	}
};
struct GreaterThanEquals {
	template <class T>
	static inline bool operation(const T& left, const T& right) {
		return left >= right;
	}
};

struct LessThan {
	template <class T>
	static inline bool operation(const T& left, const T& right) {
		return left < right;
	}
};
struct LessThanEquals {
	template <class T>
	static inline bool operation(const T& left, const T& right) {
		return left <= right;
	}
};

//===--------------------------------------------------------------------===//
// Specialized Boolean Comparison Operators
//===--------------------------------------------------------------------===//
template <>
inline bool GreaterThan::operation(const bool& left, const bool& right) {
	return !right && left;
}
template <>
inline bool LessThan::operation(const bool& left, const bool& right) {
	return !left && right;
}
//===--------------------------------------------------------------------===//
// Specialized String Comparison operations
//===--------------------------------------------------------------------===//
struct StringComparisonOperators {
	template <bool INVERSE>
	static inline bool EqualsOrNot(const string_t& a, const string_t& b) {
		// Compare via getDataUnsafe() (inlined prefix or external ptr) rather than the
		// raw prefix field. The prefix is only guaranteed consistent for strings built
		// through the (data,len) constructor; comparing the actual data keeps this
		// correct for every string_t source (matching BumbleString::operator==).
		const auto size = a.size();
		if (size != b.size()) return INVERSE ? true : false;
		const bool equal = memcmp(a.getDataUnsafe(), b.getDataUnsafe(), size) == 0;
		return INVERSE ? !equal : equal;
	}
};

template <>
inline bool Equals::operation(const string_t& left, const string_t& right) {
	return StringComparisonOperators::EqualsOrNot<false>(left, right);
}
template <>
inline bool NotEquals::operation(const string_t& left, const string_t& right) {
	return StringComparisonOperators::EqualsOrNot<true>(left, right);
}

// compare up to shared length. if still the same, compare lengths
template <class OP>
static bool templated_string_compare_op(const string_t& left, const string_t& right) {
	auto memcmp_res =
	    memcmp(left.getDataUnsafe(), right.getDataUnsafe(), std::min(left.size(), right.size()));
	auto final_res = memcmp_res == 0 ? OP::operation(left.size(), right.size()) : OP::operation(memcmp_res, 0);
	return final_res;
}

template <>
inline bool GreaterThan::operation(const string_t& left, const string_t& right) {
	return templated_string_compare_op<GreaterThan>(left, right);
}

template <>
inline bool GreaterThanEquals::operation(const string_t& left, const string_t& right) {
	return templated_string_compare_op<GreaterThanEquals>(left, right);
}

template <>
inline bool LessThan::operation(const string_t& left, const string_t& right) {
	return templated_string_compare_op<LessThan>(left, right);
}

template <>
inline bool LessThanEquals::operation(const string_t& left, const string_t& right) {
	return templated_string_compare_op<LessThanEquals>(left, right);
}


}
