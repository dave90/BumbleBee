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
#include "Hash.h"
#include "TypeDefs.h"

namespace bumblebee{

#pragma once


template <typename T>
T maxValue(T a, T b) {
	return a > b ? a : b;
}

template <typename T>
T minValue(T a, T b) {
	return a < b ? a : b;
}

template <typename T>
T absValue(T a) {
	return a < 0 ? -a : a;
}

template<class T, T val=8>
static inline T alignValue(T n) {
	return ((n + (val - 1)) / val) * val;
}

template<class T, T val=8>
static inline bool valueIsAligned(T n) {
	return (n % val) == 0;
}

template <typename T>
T signValue(T a) {
	return a < 0 ? -1 : 1;
}

// copy the value of the first element in ptr
template <typename T>
const T load(const_data_ptr_t ptr) {
	T ret;
	memcpy(&ret, ptr, sizeof(ret));
	return ret;
}

// store the value in ptr
template <typename T>
void store(const T val, data_ptr_t ptr) {
	memcpy(ptr, (void *)&val, sizeof(val));
}

// This assigns a shared pointer, but ONLY assigns if "target" is not equal to "source"
// If this is often the case, this manner of assignment is significantly faster (~20X faster)
// Since it avoids the need of an atomic incref/decref at the cost of a single pointer comparison
// Benchmark: https://gist.github.com/Mytherin/4db3faa8e233c4a9b874b21f62bb4b96
template<class T>
void assignSharedPointer(std::shared_ptr<T> &target, const std::shared_ptr<T> &source) {
	if (target.get() != source.get()) {
		target = source;
	}
}



template <typename T>
bool compareVectors(vector<T> v1, vector<T> v2) {
	if (v1.size() != v2.size()) return false;
	std::sort(v1.begin(), v1.end());
	std::sort(v2.begin(), v2.end());
	return v1 == v2;
}

template <typename T>
bool compareVectorsNoSort(vector<T> const &lhs, vector<T> const &rhs) {
	if (lhs.size() != rhs.size())return false;
	vector<bool> used(lhs.size(), false);
	bool matched = false;
	for (const auto& x : lhs) {
		matched = false;
		for (std::size_t j = 0; j < rhs.size(); ++j) {
			if (!used[j] && x == rhs[j]) {
				used[j] = true;   // consume this occurrence
				matched = true;
				break;
			}
		}
		if (!matched) break; // x has no unused match in atoms
	}
	if (!matched) return false;
	return true;
}


inline uint64_t calcChecksum(uint8_t *buffer, size_t size) {
	uint64_t result = 5381;
	uint64_t *ptr = (uint64_t *)buffer;
	size_t i;
	// for efficiency, we first hash uint64_t values
	for (i = 0; i < size / 8; i++) {
		result ^= Hash(ptr[i]);
	}
	if (size - i * 8 > 0) {
		// the remaining 0-7 bytes we hash using a string hash
		result ^= Hash((const char*)(buffer + i * 8), size - i * 8);
	}
	return result;
}


}
