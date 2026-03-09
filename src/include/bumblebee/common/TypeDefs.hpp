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
#include "types/Assert.hpp"

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



// Physical types, if you change it change also the LOGICAL TYPE
// HASH is constant type UBIGINT
enum class PhysicalType: uint16_t  {
    TINYINT = 1,
    SMALLINT = 2,
    INTEGER = 3,
    BIGINT = 4,
    UTINYINT = 5,
    USMALLINT = 6,
    UINTEGER = 7,
    UBIGINT = 8,

    FLOAT = 80,
    DOUBLE = 81,

    STRING = 100,

    UNKNOWN = 999

};


// Logical type
enum class LogicalTypeId: uint16_t  {
    BOOLEAN = 0,
    TINYINT = 1,
    SMALLINT = 2,
    INTEGER = 3,
    BIGINT = 4,
    UTINYINT = 5,
    USMALLINT = 6,
    UINTEGER = 7,
    UBIGINT = 8,
    HASH = 9, // UBIGINT
    ADDRESS = 10, // UBIGINT

    FLOAT = 80,
    DOUBLE = 81,

    DECIMAL = 90,

    STRING = 100,

    STRUCT = 110,

    DATE = 200,
    TIMESTAMP = 201,

    UNKNOWN = 999
};

LogicalTypeId physicalTypeToLogicalType(PhysicalType type);
PhysicalType logicalTypeToPhysicalType(LogicalTypeId type);

struct LogicalTypeExtraData {
    virtual bool equals(const LogicalTypeExtraData& other) const = 0;
};
struct LogicalTypeDecimalData : public LogicalTypeExtraData {
    LogicalTypeDecimalData() = default;
    LogicalTypeDecimalData(int width, int scale): width_(width),scale_(scale) {}

    bool equals(const LogicalTypeExtraData& other) const override {
        if (typeid(*this) != typeid(other)) return false;
        auto& o = static_cast<const LogicalTypeDecimalData&>(other);
        return width_ == o.width_ && scale_ == o.scale_;
    }
    int width_{0};
    int scale_{0};
};
using logical_type_date_ptr_t = std::shared_ptr<LogicalTypeExtraData>;

class LogicalType  {
public:
    LogicalType(LogicalTypeId type);
    LogicalType(PhysicalType type): ctype_(type), type_(physicalTypeToLogicalType(type)) {};
    LogicalType():type_(LogicalTypeId::UNKNOWN), ctype_(PhysicalType::UNKNOWN) {};
    LogicalType(const LogicalType& other) = default;
    PhysicalType getPhysicalType() const;
    LogicalTypeId type() const{return type_;};
    std::string toString() const;
    const LogicalTypeDecimalData& getDecimalData() const{
        BB_ASSERT(type_ == LogicalTypeId::DECIMAL);
        return (LogicalTypeDecimalData&)*data_;
    }

    static LogicalType createDecimal(int width, int scale);
    static LogicalType createDate();
    static LogicalType createTimestamp();

    friend bool operator==(const LogicalType &lhs, const LogicalType &rhs);
    friend bool operator!=(const LogicalType &lhs, const LogicalType &rhs);

private:
    PhysicalType ctype_;
    LogicalTypeId type_;
    // Data for complex type
    logical_type_date_ptr_t data_;
};



constexpr bool operator==(PhysicalType a, LogicalTypeId b) noexcept {
    return static_cast<std::underlying_type_t<PhysicalType>>(a) ==
           static_cast<std::underlying_type_t<LogicalTypeId>>(b);
}
constexpr bool operator!=(PhysicalType a, LogicalTypeId b) noexcept {
    return !(a == b);
}
constexpr bool operator==(LogicalTypeId a, PhysicalType b) noexcept {
    return b == a;
}
constexpr bool operator!=(LogicalTypeId a, PhysicalType b) noexcept {
    return !(a == b);
}

struct VectorLogicTypeHash {
    std::size_t operator()(const vector<LogicalType>& vec) const {
        std::size_t seed = 0;
        for (auto& v : vec) {
            // A simple hash combination: shift and xor
            seed ^= std::hash<uint8_t>{}(static_cast<uint8_t>(v.type())) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
    bool operator()(const vector<LogicalType>& vec1, const vector<LogicalType>& vec2) const {
        return vec1 == vec2;
    }
};

enum class Operator : uint8_t {
    PLUS=0,
    MINUS=1,
    DIV=2,
    TIMES=3,
    MODULO=4,
    NONE=5
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
    BUILD = 2,
    SOURCE = 3
};

using hash_t = uint64_t;
using idx_t = uint64_t;
using date_t = int32_t;
using timestamp_t = int64_t;

using data_t = uint8_t;
using data_ptr_t = data_t*;
using const_data_ptr_t = const data_t*;

using sel_t = uint32_t;

using string = std::string;

idx_t getPhysicalTypeSize(PhysicalType type);
bool isUnsigned(const LogicalType& type);
LogicalType getBumpedLogicalType(LogicalType type);
LogicalType getGeneralBumpedType(LogicalType type);
vector<LogicalType> getGeneralBumpedType(const vector<LogicalType>& type);
LogicalType getSignedBumpedType(const LogicalType& type);
// Determines the dominant constant type between two types, resolving UNKNOWN values,
// adjusting for signedness differences, and returning the type with the larger size.
LogicalType getCommonType(LogicalType t1, LogicalType t2, Operator op = Operator::NONE);
char getOperatorChar(Operator op);
Operator getCharOperator(char op);
string getBinopStr(Binop binop);
Binop getFlippedBinop(Binop binop);
string physicalTypeToString(PhysicalType type);
string logicalTypeToString(LogicalTypeId type);
PhysicalType physicalTypeFromString(const string& typeStr);
LogicalType logicalTypeFromString(const string& typeStr);
Binop getFlippedBinop(Binop op);
idx_t nextPowerOfTwo(idx_t n);
bool typeIsConstantSize(PhysicalType type);
}
