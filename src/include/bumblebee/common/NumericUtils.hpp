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
#include <cstdint>

#include "types/BumbleString.hpp"
#include "types/Vector.hpp"

namespace bumblebee{


class NumericHelper {
public:
	static const int64_t POWERS_OF_TEN[20];
	static const double DOUBLE_POWERS_OF_TEN[40];
	static const char digits[];

public:
	template <class T>
	static int unsignedLength(T value);
	template <class SIGNED, class UNSIGNED>
	static int signedLength(SIGNED value) {
		int sign = -(value < 0);
		UNSIGNED unsigned_value = (value ^ sign) - sign;
		return UnsignedLength(unsigned_value) - sign;
	}

	// Formats value in reverse and returns a pointer to the beginning.
	template <class T>
	static char * formatUnsigned(T value, char *ptr) {
		while (value >= 100) {
			// Integer division is slow so do it for a group of two digits instead
			// of for every digit. The idea comes from the talk by Alexandrescu
			// "Three Optimization Tips for C++".
			auto index = static_cast<unsigned>((value % 100) * 2);
			value /= 100;
			*--ptr = digits[index + 1];
			*--ptr = digits[index];
		}
		if (value < 10) {
			*--ptr = static_cast<char>('0' + value);
			return ptr;
		}
		auto index = static_cast<unsigned>(value * 2);
		*--ptr = digits[index + 1];
		*--ptr = digits[index];
		return ptr;
	}

	template <class SIGNED, class UNSIGNED>
	static string_t formatSigned(SIGNED value, Vector &vector) {
		int sign = -(value < 0);
		UNSIGNED unsigned_value = UNSIGNED(value ^ sign) - sign;
		int length = unsignedLength<UNSIGNED>(unsigned_value) - sign;
		string_t result = StringVector::emptyString(vector, length);
		auto dataptr = result.getDataWriteable();
		auto endptr = dataptr + length;
		endptr = formatUnsigned(unsigned_value, endptr);
		if (sign) {
			*--endptr = '-';
		}
		return result;
	}
};

template <>
int NumericHelper::unsignedLength(uint8_t value);
template <>
int NumericHelper::unsignedLength(uint16_t value);
template <>
int NumericHelper::unsignedLength(uint32_t value);
template <>
int NumericHelper::unsignedLength(uint64_t value);

struct DecimalToString {
	template <class SIGNED, class UNSIGNED>
	static int decimalLength(SIGNED value, uint8_t scale) {
		if (scale == 0) {
			// scale is 0: regular number
			return NumericHelper::signedLength<SIGNED, UNSIGNED>(value);
		}
		// length is max of either:
		// scale + 2 OR
		// integer length + 1
		// scale + 2 happens when the number is in the range of (-1, 1)
		// in that case we print "0.XXX", which is the scale, plus "0." (2 chars)
		// integer length + 1 happens when the number is outside of that range
		// in that case we print the integer number, but with one extra character ('.')
		return maxValue(scale + 2 + (value < 0 ? 1 : 0), NumericHelper::signedLength<SIGNED, UNSIGNED>(value) + 1);
	}

	template <class SIGNED, class UNSIGNED>
	static void formatDecimal(SIGNED value, uint8_t scale, char *dst, idx_t len) {
		char *end = dst + len;
		if (value < 0) {
			value = -value;
			*dst = '-';
		}
		if (scale == 0) {
			NumericHelper::formatUnsigned<UNSIGNED>(value, end);
			return;
		}
		// we write two numbers:
		// the numbers BEFORE the decimal (major)
		// and the numbers AFTER the decimal (minor)
		UNSIGNED minor = value % (UNSIGNED)NumericHelper::POWERS_OF_TEN[scale];
		UNSIGNED major = value / (UNSIGNED)NumericHelper::POWERS_OF_TEN[scale];
		// write the number after the decimal
		dst = NumericHelper::formatUnsigned<UNSIGNED>(minor, end);
		// (optionally) pad with zeros and add the decimal point
		while (dst > (end - scale)) {
			*--dst = '0';
		}
		*--dst = '.';
		// now write the part before the decimal
		dst = NumericHelper::formatUnsigned<UNSIGNED>(major, dst);
	}

	template <class SIGNED, class UNSIGNED>
	static string_t format(SIGNED value, uint8_t scale, Vector &vector) {
		int len = decimalLength<SIGNED, UNSIGNED>(value, scale);
		string_t result = StringVector::emptyString(vector, len);
		formatDecimal<SIGNED, UNSIGNED>(value, scale, result.getDataWriteable(), len);
		return result;
	}
};

};

