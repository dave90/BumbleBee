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
#include <cstdint>
#include <string>

namespace bumblebee {

// HASH is constant type UBIGINT
enum ConstantType: uint8_t  {
    TINYINT = 0,
    SMALLINT = 1,
    INTEGER = 2,
    BIGINT = 3,
    UTINYINT = 4,
    USMALLINT = 5,
    UINTEGER = 6,
    UBIGINT = 7,
    FLOAT = 8,
    DOUBLE = 9,
    STRING = 10,
    UNKNOWN = 11
};

using hash_t = uint64_t;
using idx_t = uint64_t;

using data_t = uint8_t;
using data_ptr_t = data_t*;
using const_data_ptr_t = const data_t*;

using sel_t = uint32_t;

using string = std::string;

idx_t getCTypeSize(ConstantType type);
bool isUnsigned(ConstantType type);
ConstantType getBumpedType(ConstantType type);

}
