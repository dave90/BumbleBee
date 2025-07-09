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
#include <string>

#include "bumblebee/common/TypeDefs.h"

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
    static constexpr idx_t PREFIX_LENGTH = 12;

    BumbleString() = default;
    BumbleString(const char* data);
    BumbleString(const char* data, uint32_t len);
    BumbleString(const BumbleString& other);

    bool isInlined() const;
    const char *getDataUnsafe() const ;
    char * getDataWriteable() const ;
    const char * getPrefix() const;
    // names convention of string
    idx_t length() const;
    idx_t size() const;
    string getString() const;

    bool operator<(const BumbleString &r) const;
    const char* c_str() const;
private:
    struct {
        // +1 for end termination
        char prefix[PREFIX_LENGTH+1];
        char *ptr;
    } value_;
    uint32_t length_;
};
    using string_t = BumbleString;


}
