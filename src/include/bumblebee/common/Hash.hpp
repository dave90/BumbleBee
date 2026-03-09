

#pragma once

#include <cstdint>

#include "TypeDefs.hpp"
#include "bumblebee/common/types/BumbleString.hpp"
#include "types/Assert.hpp"

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
hash_t Hash(string val);
template <>
hash_t Hash(string_t val);
hash_t Hash(const char *val, size_t size);


struct StringTHash {
	size_t operator()(const string& v) const noexcept {
		return Hash<string>(v);
	}
};


// Avalanche mixer: scrambles bits so outputs look uniform even if inputs don’t.
static inline uint64_t mix64(uint64_t x){
	x ^= x >> 30;                       // fold high bits into low
	x *= 0xbf58476d1ce4e5b9ULL;         // large odd multiplier → nonlinear diffusion
	x ^= x >> 27;
	x *= 0x94d049bb133111ebULL;         // second independent multiplier
	x ^= x >> 31;
	return x;
}

// Derive two independent-looking 32-bit values from one 64-bit hash.
// Use golden-ratio constant to decorrelate the second stream.
static inline void deriveH1H2(uint64_t h, uint32_t& h1, uint32_t& h2){
	uint64_t a = mix64(h);
	uint64_t b = mix64(h ^ 0x9e3779b97f4a7c15ULL); // golden ratio constant
	h1 = (uint32_t)a;                 // low 32 bits (already mixed)
	h2 = (uint32_t)(b | 1u);          // low 32 bits, forced odd to avoid cycles
}

// Build k positions in a 16-bit Bloom via double hashing.
// In practice for m=16:
// k=2 if very few items per filter
// k=3–4 if 3–5 items per filter
// k=5+ if you expect many items, but then the 16-bit filter will saturate quickly
static inline uint16_t bloom16FromHash(uint64_t h, int k){
	BB_ASSERT(k > 0 && k <= 16);
	uint32_t h1, h2;
	deriveH1H2(h, h1, h2);
	uint16_t mask = 0;
	for(int i = 0; i < k; ++i){
		// modulo 16 via bitmask (m must be power of two)
		uint32_t pos = (h1 + (uint32_t)i * h2) & 15u;
		mask |= (uint16_t)(1u << pos);
	}
	return mask;
}

// Bloom Add: set bits for this hash in the 16-bit bloom
static inline void bloom16Add(uint16_t& bloom, uint64_t hash, int k = 4){
	bloom |= bloom16FromHash(hash, k);
}

// Bloom Query: return true if all bits for this hash are present
static inline bool bloom16CouldContains(uint16_t bloom, uint64_t hash, int k = 4){
	uint16_t mask = bloom16FromHash(hash, k);
	return !(mask & ~bloom); // all required bits set?
}

}
