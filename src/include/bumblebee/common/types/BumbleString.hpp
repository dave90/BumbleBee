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
#include <string>

#include "bumblebee/common/TypeDefs.hpp"

namespace bumblebee {

/*
* BumbleString is a custom string class designed for high-performance scenarios
* where string data is allocated and managed externally. It provides efficient
* access to string data without copying, and supports a small-string optimization
* (SSO) that inlines short strings directly into the object for better cache locality.
*
* Key Features:
* - Non-owning: Does not allocate or free memory; assumes external ownership of data.
* - Small-String Optimization: Stores strings shorter than or equal to PREFIX_LENGTH
*   directly inside the object to avoid heap access.
* - Minimal footprint and overhead.
* - Safe read access to data, but assumes external responsibility for data lifetime.
*
* Usage Note:
* The caller must ensure that any externally referenced data (i.e., not inlined)
* remains valid for the lifetime of the BumbleString instance.
*/
class BumbleString {
public:
    // lenght set to 11 as total data should fit in 24 bytes (multiple of 8)
    static constexpr idx_t PREFIX_LENGTH = 11;

    // These member functions are deliberately defined inline in the header:
    // construction, hashing and comparison of strings run once per row in
    // scans, group-bys and joins, and an out-of-line call per value dominated
    // string-heavy query profiles.
    BumbleString() = default;
    inline BumbleString(uint32_t len) {
        value_.length = len;
    }
    inline BumbleString(const char* data, uint32_t len) {
        value_.length = len;
        if (isInlined()) {
            // store the data in prefix
            // +1 for string termination
            memcpy(value_.prefix, data, len * sizeof(char));
            value_.prefix[len] = '\0';
            return;
        }
        memcpy(value_.prefix, data, PREFIX_LENGTH * sizeof(char));
        value_.prefix[PREFIX_LENGTH] = '\0';
        value_.ptr = (char *)(data);
    }
    inline BumbleString(const char* data): BumbleString(data, strlen(data)) {}
    // Member-wise copy: the prefix always mirrors the referenced data, so the
    // default copy is equivalent to re-deriving it (and matches the implicitly
    // defaulted copy assignment). Keeping the type trivially copyable lets it
    // be passed in registers.
    BumbleString(const BumbleString& other) = default;


    inline bool isInlined() const {
        return isInlined(size());
    }
    inline const char *getDataUnsafe() const {
        if (isInlined())
            return value_.prefix;
        return value_.ptr;
    }
    inline char *getDataWriteable() const {
        if (isInlined())
            return (char*)value_.prefix;
        return value_.ptr;
    }
    inline const char * getPrefix() const {
        return value_.prefix;
    }
    // names convention of string
    inline idx_t length() const {
        return size();
    }
    inline idx_t size() const {
        return value_.length;
    }
    string getString() const;

    inline bool operator<(const BumbleString &r) const {
        // compare the data: length-aware memcmp. strcmp is unusable here because
        // byte-comparable sort keys contain embedded NUL bytes.
        auto left_length = size();
        auto right_length = r.size();
        auto min_length = left_length < right_length ? left_length : right_length;
        auto memcmp_res = memcmp(getDataUnsafe(), r.getDataUnsafe(), min_length);
        return memcmp_res < 0 || (memcmp_res == 0 && left_length < right_length);
    }
    inline bool operator>(const BumbleString &r) const {
        auto left_length = size();
        auto right_length = r.size();
        auto min_length = left_length < right_length ? left_length : right_length;
        auto memcmp_res = memcmp(getDataUnsafe(), r.getDataUnsafe(), min_length);
        return memcmp_res > 0 || (memcmp_res == 0 && left_length > right_length);
    }
    inline bool operator==(const BumbleString &r) const {
        auto left_length = size();
        return left_length == r.size() && memcmp(getDataUnsafe(), r.getDataUnsafe(), left_length) == 0;
    }
    inline const char* c_str() const {
        return getDataUnsafe();
    }

    // return true if the string is inline
    static inline bool isInlined(uint32_t len) {
        return len <= PREFIX_LENGTH;
    }
private:
    struct {
        // +1 for end termination
        char prefix[PREFIX_LENGTH+1];
        // moved into the struct to avoid memory padding
        uint32_t length;
        char *ptr;
    } value_;
};
using string_t = BumbleString;


}
