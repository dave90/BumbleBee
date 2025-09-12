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
#include "bumblebee/common/Vector.hpp"

namespace bumblebee {


struct Storage {
    // The size of a hard disk sector, only really needed for Direct IO
    constexpr static int SECTOR_SIZE = 4096;
    // Block header size for blocks written to the storage
    constexpr static int BLOCK_HEADER_SIZE = sizeof(uint64_t);
    // Size of a memory slot managed by the StorageManager. This is the quantum of allocation for Blocks. We
    // default to 256KB. (1 << 18)
    constexpr static int BLOCK_ALLOC_SIZE = 262144;
    // The actual memory space that is available within the blocks
    constexpr static int BLOCK_SIZE = BLOCK_ALLOC_SIZE - BLOCK_HEADER_SIZE;
    // The size of the headers. This should be small and written more or less atomically by the hard disk. We default
    // to the page size, which is 4KB. (1 << 12)
    constexpr static int FILE_HEADER_SIZE = 4096;
};

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
    UNKNOWN = 100
};

struct VectorConstHash {
    std::size_t operator()(const vector<ConstantType>& vec) const {
        std::size_t seed = 0;
        for (auto v : vec) {
            // A simple hash combination: shift and xor
            seed ^= std::hash<uint8_t>{}(static_cast<uint8_t>(v)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

enum Operator {
    PLUS=0,
    MINUS=1,
    DIV=2,
    TIMES=3,
    MODULO=4
};


enum Binop {
    NONE_OP = 0,
    EQUAL = 1,
    UNEQUAL = 2,
    LESS = 3,
    GREATER = 4,
    LESS_OR_EQ = 5,
    GREATER_OR_EQ = 6,
    ASSIGNMENT = 7,
};

enum PhysicalHashType : uint8_t {
    PROBE = 0,
    COLLECT = 1,
    BUILD = 2
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
ConstantType getSignedBumpedType(ConstantType type);
// Determines the dominant constant type between two types, resolving UNKNOWN values,
// adjusting for signedness differences, and returning the type with the larger size.
ConstantType getCommonType(ConstantType t1, ConstantType t2);
char getOperatorChar(Operator op);
string getBinopStr(Binop binop);
string ctypeToString(ConstantType type);
Binop getFlippedBinop(Binop op);
idx_t nextPowerOfTwo(idx_t n);
bool typeIsConstantSize(ConstantType type);
}
