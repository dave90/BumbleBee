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

	void readStringVector(vector<string> &list);
};

template <>
string Deserializer::read();



}
