


#include "bumblebee/common/Hash.hpp"

#include <cstring>
#include <functional>


namespace bumblebee {


template <>
hash_t Hash(uint64_t val) {
    return murmurhash64(val);
}

template <>
hash_t Hash(int64_t val) {
    return murmurhash64((uint64_t)val);
}


template <>
hash_t Hash(float val) {
    return std::hash<float> {}(val);
}

template <>
hash_t Hash(double val) {
    return std::hash<double> {}(val);
}


template <>
hash_t Hash(const char *str) {
    return Hash(str, strlen(str));
}

template <>
hash_t Hash(string val) {
    return Hash(val.c_str(), val.size());
}

template <>
hash_t Hash(string_t val) {
    return Hash(val.c_str(), val.size());
}

template <>
hash_t Hash(char *val) {
    return Hash<const char *>(val);
}

// Jenkins hash function: https://en.wikipedia.org/wiki/Jenkins_hash_function
uint32_t JenkinsOneAtATimeHash(const char *key, size_t length) {
    size_t i = 0;
    uint32_t hash = 0;
    while (i != length) {
        hash += key[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

// Word-at-a-time string hash (FNV-style accumulation with a strong finalizer).
// Processes 8 bytes per iteration instead of 1, which dominates grouping/join
// performance on string keys (e.g. URL GROUP BY). The hash value is used only
// for bucketing and bloom filters — equality is always re-checked — so the
// exact algorithm is a pure performance/distribution choice and cannot change
// query results.
hash_t Hash(const char *val, size_t size) {
    const uint64_t m = 0x9E3779B97F4A7C15ULL; // golden-ratio odd multiplier
    uint64_t h = 0xcbf29ce484222325ULL ^ (size * m);
    const char *p = val;
    size_t n = size;
    while (n >= 8) {
        uint64_t k;
        memcpy(&k, p, 8);
        h ^= k;
        h *= m;
        h ^= h >> 29;
        p += 8;
        n -= 8;
    }
    if (n) {
        // Read the remaining 1..7 bytes into a zero-initialised word.
        uint64_t k = 0;
        memcpy(&k, p, n);
        h ^= k;
        h *= m;
    }
    // Avalanche finalizer.
    h ^= h >> 32;
    h *= m;
    h ^= h >> 29;
    return h;
}


}
