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
#include "TypeDefs.hpp"

namespace bumblebee{

template <class T>
struct NumericLimits {
	static T minimum();
	static T maximum();
	static bool isSigned();
	static idx_t digits();
};

template <>
struct NumericLimits<int8_t> {
	static int8_t minimum();
	static int8_t maximum();
	static bool isSigned() {
		return true;
	}
	static idx_t digits() {
		return 3;
	}
};
template <>
struct NumericLimits<int16_t> {
	static int16_t minimum();
	static int16_t maximum();
	static bool isSigned() {
		return true;
	}
	static idx_t digits() {
		return 5;
	}
};
template <>
struct NumericLimits<int32_t> {
	static int32_t minimum();
	static int32_t maximum();
	static bool isSigned() {
		return true;
	}
	static idx_t digits() {
		return 10;
	}
};
template <>
struct NumericLimits<int64_t> {
	static int64_t minimum();
	static int64_t maximum();
	static bool isSigned() {
		return true;
	}
	static idx_t digits() {
		return 19;
	}
};
template <>
struct NumericLimits<uint8_t> {
	static uint8_t minimum();
	static uint8_t maximum();
	static bool isSigned() {
		return false;
	}
	static idx_t digits() {
		return 3;
	}
};
template <>
struct NumericLimits<uint16_t> {
	static uint16_t minimum();
	static uint16_t maximum();
	static bool isSigned() {
		return false;
	}
	static idx_t digits() {
		return 5;
	}
};
template <>
struct NumericLimits<uint32_t> {
	static uint32_t minimum();
	static uint32_t maximum();
	static bool isSigned() {
		return false;
	}
	static idx_t digits() {
		return 10;
	}
};
template <>
struct NumericLimits<uint64_t> {
	static uint64_t minimum();
	static uint64_t maximum();
	static bool isSigned() {
		return false;
	}
	static idx_t digits() {
		return 20;
	}
};
template <>
struct NumericLimits<float> {
	static float minimum();
	static float maximum();
	static bool isSigned() {
		return true;
	}
	static idx_t digits() {
		return 127;
	}
};
template <>
struct NumericLimits<double> {
	static double minimum();
	static double maximum();
	static bool isSigned() {
		return true;
	}
	static idx_t digits() {
		return 250;
	}
};


}
