


#include "bumblebee/common/Hash.hpp"

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

hash_t Hash(const char *val, size_t size) {
    auto hash_val = JenkinsOneAtATimeHash(val, size);
    return Hash<uint32_t>(hash_val);
}


}
