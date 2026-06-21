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
#include <cstring>
#include <memory>

#include "bumblebee/common/TypeDefs.hpp"
#include "bumblebee/common/types/ValidityMask.hpp"

namespace bumblebee{



// The Serialize class is a base class that can be used to serializing objects into a binary buffer
class Serializer {
public:
	virtual ~Serializer() {
	}

	virtual void writeData(const_data_ptr_t buffer, idx_t write_size) = 0;

	template <class T>
	void write(T element) {
		writeData((const_data_ptr_t)&element, sizeof(T));
	}

	// Write data from a string buffer directly (wihtout length prefix)
	void writeBufferData(const string &str) {
		writeData((const_data_ptr_t)str.c_str(), str.size());
	}
	// Write a string with a length prefix
	void writeString(const string &val) {
		write<uint32_t>((uint32_t)val.size());
		if (!val.empty()) {
			writeData((const_data_ptr_t)val.c_str(), val.size());
		}
	}
	void writeStringLen(const_data_ptr_t val, idx_t len) {
		write<uint32_t>((uint32_t)len);
		if (len > 0) {
			writeData(val, len);
		}
	}

	// Serialize the first `count` bits of `mask`. Layout:
	//   [allValid:1B] [packed:(count+7)/8 B if !allValid]
	// Packed bytes are LSB-first within each byte (bit i of row r lives in
	// byte r/8, bit position r%8); bit=1 means valid (matches ValidityMask).
	// All-valid fast path writes only the flag byte — zero overhead in the
	// common case.
	void writeBitmask(const ValidityMask &mask, idx_t count) {
		bool allValid = mask.checkAllValid(count);
		write<uint8_t>(allValid ? 1 : 0);
		if (allValid || count == 0) {
			return;
		}
		idx_t byteCount = (count + 7) / 8;
		std::unique_ptr<uint8_t[]> buf(new uint8_t[byteCount]);
		std::memset(buf.get(), 0, byteCount);
		for (idx_t r = 0; r < count; r++) {
			if (mask.rowIsValid(r)) {
				buf[r / 8] |= static_cast<uint8_t>(1u << (r % 8));
			}
		}
		writeData((const_data_ptr_t)buf.get(), byteCount);
	}

	template <class T>
	void WriteList(vector<std::unique_ptr<T>> &list) {
		write<uint32_t>((uint32_t)list.size());
		for (auto &child : list) {
			child->Serialize(*this);
		}
	}

	void writeStringVector(const vector<string> &list) {
		write<uint32_t>((uint32_t)list.size());
		for (auto &child : list) {
			writeString(child);
		}
	}

	template <class T>
	void WriteOptional(const std::unique_ptr<T> &element) {
		write<bool>(element ? true : false);
		if (element) {
			element->Serialize(*this);
		}
	}
};

// The Deserializer class assists in deserializing a binary blob back into an
// object
class Deserializer {
public:
	virtual ~Deserializer() {
	}

	// Reads [read_size] bytes into the buffer
	virtual void readData(data_ptr_t buffer, idx_t read_size) = 0;

	template <class T>
	T read() {
		T value;
		readData((data_ptr_t)&value, sizeof(T));
		return value;
	}
	template <class T>
	void readList(vector<std::unique_ptr<T>> &list) {
		auto select_count = read<uint32_t>();
		for (uint32_t i = 0; i < select_count; i++) {
			auto child = T::deserialize(*this);
			list.push_back(std::move(child));
		}
	}

	template <class T, class RETURN_TYPE = T>
	std::unique_ptr<RETURN_TYPE> readOptional() {
		auto has_entry = read<bool>();
		if (has_entry) {
			return T::deserialize(*this);
		}
		return nullptr;
	}

	// Mirror of writeBitmask: read the allValid flag, then (if !allValid) the
	// packed mask bytes for `count` rows. Resets the mask before reading so
	// stale bits from a previous use don't leak. All-valid restores the
	// no-buffer fast path.
	void readBitmask(ValidityMask &mask, idx_t count) {
		mask.setAllValid();
		auto allValid = read<uint8_t>();
		if (allValid || count == 0) {
			return;
		}
		idx_t byteCount = (count + 7) / 8;
		std::unique_ptr<uint8_t[]> buf(new uint8_t[byteCount]);
		readData((data_ptr_t)buf.get(), byteCount);
		for (idx_t r = 0; r < count; r++) {
			if ((buf[r / 8] & (1u << (r % 8))) == 0) {
				mask.setInvalid(r);
			}
		}
	}

	void readStringVector(vector<string> &list);
};

template <>
string Deserializer::read();



}
