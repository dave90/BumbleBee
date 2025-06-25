

#pragma once

#include <cstdint>

#include "TypeDefs.h"
#include "type/StringT.h"

namespace bumblebee {


// efficient hash function that maximizes the avalanche effect and minimizes
// bias
// see: https://nullprogram.com/blog/2018/07/31/
inline hash_t murmurhash64(uint64_t x) {
	return x * UINT64_C(0xbf58476d1ce4e5b9);
}

inline hash_t murmurhash32(uint32_t x) {
	return murmurhash64(x);
}

template <class T>
hash_t Hash(T value) {
	return murmurhash32(value);
}

//! Combine two hashes by XORing them
inline hash_t CombineHash(hash_t left, hash_t right) {
	return left ^ right;
}

template <>
hash_t Hash(uint64_t val);
template <>
hash_t Hash(int64_t val);
template <>
hash_t Hash(float val);
template <>
hash_t Hash(double val);
template <>
hash_t Hash(const char *val);
template <>
hash_t Hash(char *val);
template <>
hash_t Hash(StringT val);
hash_t Hash(const char *val, size_t size);

}
