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
#include <cfloat>

#include "ErrorHandler.hpp"
#include "Helper.hpp"
#include "TypeDefs.hpp"
#include "types/BumbleString.hpp"
#include "BSwap.hpp"

namespace bumblebee{


struct SortKeyEncoding {
public:
	static inline bool isLittleEndian() {
		int n = 1;
		if (*(char*)(&n) == 1) {
			return true;
		} else {
			return false;
		}
	}

	template <class T>
	static inline void encodeData(data_ptr_t dataptr, T value) {
		ErrorHandler::errorNotImplemented("Cannot create sort key encode from this type");
	}

	// flip the byte for the signed integer
	static inline uint8_t flipSign(uint8_t key_byte) {
		return key_byte ^ 128;
	}

	static inline uint32_t encodeFloat(float x) {
		uint32_t buff;
		//! zero
		if (x == 0) {
			buff = 0;
			buff |= (1u << 31);
			return buff;
		}

		//! infinity
		if (x > FLT_MAX) {
			return UINT_MAX - 1;
		}
		//! -infinity
		if (x < -FLT_MAX) {
			return 0;
		}
		buff = load<uint32_t>((const_data_ptr_t)&x);
		if ((buff & (1U << 31)) == 0) { //! +0 and positive numbers
			buff |= (1U << 31);
		} else {          //! negative numbers
			buff = ~buff; //! complement 1
		}

		return buff;
	}


	static inline uint64_t encodeDouble(double x) {
		uint64_t buff;
		//! zero
		if (x == 0) {
			buff = 0;
			buff += (1ULL << 63);
			return buff;
		}
		//! infinity
		if (x > DBL_MAX) {
			return ULLONG_MAX - 1;
		}
		//! -infinity
		if (x < -DBL_MAX) {
			return 0;
		}
		buff = load<uint64_t>((const_data_ptr_t)&x);
		if (buff < (1ULL << 63)) { //! +0 and positive numbers
			buff += (1ULL << 63);
		} else {          //! negative numbers
			buff = ~buff; //! complement 1
		}
		return buff;
	}

private:
	template <class T>
	static void encodeSigned(data_ptr_t dataptr, T value);
};



template <class T>
void SortKeyEncoding::encodeSigned(data_ptr_t dataptr, T value) {
	using type = std::make_unsigned_t<T>;
	type bytes;
	store<T>(value, (data_ptr_t)(&bytes));
	store<type>(bswap(bytes), dataptr);
	dataptr[0] = flipSign(dataptr[0]);
}

template <>
inline void SortKeyEncoding::encodeData(data_ptr_t dataptr, int8_t value) {
	encodeSigned<int8_t>(dataptr, value);
}

template <>
inline void SortKeyEncoding::encodeData(data_ptr_t dataptr, int16_t value) {
	encodeSigned<int16_t>(dataptr, value);
}

template <>
inline void SortKeyEncoding::encodeData(data_ptr_t dataptr, int32_t value) {
	encodeSigned<int32_t>(dataptr, value);
}

template <>
inline void SortKeyEncoding::encodeData(data_ptr_t dataptr, int64_t value) {
	encodeSigned<int64_t>(dataptr, value);
}

template <>
inline void SortKeyEncoding::encodeData(data_ptr_t dataptr, uint8_t value) {
	store<uint8_t>(value, dataptr);
}

template <>
inline void SortKeyEncoding::encodeData(data_ptr_t dataptr, uint16_t value) {
	store<uint16_t>(bswap(value), dataptr);
}

template <>
inline void SortKeyEncoding::encodeData(data_ptr_t dataptr, uint32_t value) {
	store<uint32_t>(bswap(value), dataptr);
}

template <>
inline void SortKeyEncoding::encodeData(data_ptr_t dataptr, uint64_t value) {
	store<uint64_t>(bswap(value), dataptr);
}


template <>
inline void SortKeyEncoding::encodeData(data_ptr_t dataptr, float value) {
	uint32_t converted_value = encodeFloat(value);
	store<uint32_t>(bswap(converted_value), dataptr);
}

template <>
inline void SortKeyEncoding::encodeData(data_ptr_t dataptr, double value) {
	uint64_t converted_value = encodeDouble(value);
	store<uint64_t>(bswap(converted_value), dataptr);
}

}
