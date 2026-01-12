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

#include "bumblebee/common/TypeDefs.hpp"

#include "bumblebee/common/ErrorHandler.hpp"
#include "bumblebee/common/Hash.hpp"
#include "bumblebee/common/types/BumbleString.hpp"
#include "bumblebee/common/types/Decimal.hpp"
#include "parquet/parquet_types.h"

namespace bumblebee {




LogicalTypeId cTypeToLogicalType(PhysicalType type) {
    switch (type) {
        case PhysicalType::TINYINT:   return LogicalTypeId::TINYINT;
        case PhysicalType::SMALLINT:  return LogicalTypeId::SMALLINT;
        case PhysicalType::INTEGER:   return LogicalTypeId::INTEGER;
        case PhysicalType::BIGINT:    return LogicalTypeId::BIGINT;
        case PhysicalType::UTINYINT:  return LogicalTypeId::UTINYINT;
        case PhysicalType::USMALLINT: return LogicalTypeId::USMALLINT;
        case PhysicalType::UINTEGER:  return LogicalTypeId::UINTEGER;
        case PhysicalType::UBIGINT:   return LogicalTypeId::UBIGINT;

        case PhysicalType::FLOAT:     return LogicalTypeId::FLOAT;
        case PhysicalType::DOUBLE:    return LogicalTypeId::DOUBLE;

        case PhysicalType::STRING:    return LogicalTypeId::STRING;

        case PhysicalType::UNKNOWN:
        default:
            return LogicalTypeId::UNKNOWN;
    }
}

PhysicalType logicalTypeToCType(LogicalTypeId type) {
    switch (type) {
        case LogicalTypeId::TINYINT:   return PhysicalType::TINYINT;
        case LogicalTypeId::SMALLINT:  return PhysicalType::SMALLINT;
        case LogicalTypeId::INTEGER:   return PhysicalType::INTEGER;
        case LogicalTypeId::BIGINT:    return PhysicalType::BIGINT;
        case LogicalTypeId::UTINYINT:  return PhysicalType::UTINYINT;
        case LogicalTypeId::USMALLINT: return PhysicalType::USMALLINT;
        case LogicalTypeId::UINTEGER:  return PhysicalType::UINTEGER;
        case LogicalTypeId::HASH:
        case LogicalTypeId::ADDRESS:
        case LogicalTypeId::UBIGINT:   return PhysicalType::UBIGINT;

        case LogicalTypeId::FLOAT:     return PhysicalType::FLOAT;
        case LogicalTypeId::DOUBLE:    return PhysicalType::DOUBLE;

        case LogicalTypeId::STRING:    return PhysicalType::STRING;

        case LogicalTypeId::BOOLEAN:
        case LogicalTypeId::DECIMAL:
        case LogicalTypeId::STRUCT:
        case LogicalTypeId::UNKNOWN:
        default:
            return PhysicalType::UNKNOWN;
    }
}



PhysicalType LogicalType::getPhysicalType() const {
    return ctype_;
}

std::string LogicalType::toString() const {
    string result = "LogicalType(" + logicalTypeToString(type_)+", PType("+ctypeToString(ctype_)+"))";
    return result;
}

void LogicalType::getDecimalWidhtAndScale(int &width, int &scale) const{
    width = width_;
    scale = scale_;
}

LogicalType LogicalType::createDecimal(int width, int scale) {
    LogicalType type(LogicalTypeId::DECIMAL);
    if (width <= Decimal::MAX_WIDTH_INT16) {
        type.ctype_ = PhysicalType::SMALLINT;
    } else if (width <= Decimal::MAX_WIDTH_INT32) {
        type.ctype_ = PhysicalType::INTEGER;
    } else if (width <= Decimal::MAX_WIDTH_INT64) {
        type.ctype_ = PhysicalType::BIGINT;
    }else {
        ErrorHandler::errorNotImplemented("DECIMAL type with precision > 18 not supported!");
    }
    type.scale_ = scale;
    type.width_ = width;
    return type;
}

LogicalType LogicalType::createDate() {
    LogicalType type(LogicalTypeId::DATE);
    type.ctype_ = PhysicalType::INTEGER;
    return type;
}

LogicalType LogicalType::createTimestamp() {
    LogicalType type(LogicalTypeId::TIMESTAMP);
    type.ctype_ = PhysicalType::BIGINT;
    return type;
}


char getOperatorChar(Operator op) {
    switch (op) {
        case PLUS:
            return '+';
        case MINUS:
            return '-';
        case DIV:
            return '/';
        case TIMES:
            return '*';
        case MODULO:
            return '%';
        default:
            ;
    }
    ErrorHandler::errorNotImplemented("Invalid operator conversion from Operator");
    return ' ';
}

Operator getCharOperator(char op) {
    switch (op) {
        case '+':
            return PLUS;
        case '-':
            return MINUS;
        case '/':
            return DIV;
        case '*':
            return TIMES;
        case '%':
            return MODULO;
        default:
            ;
    }
    ErrorHandler::errorNotImplemented("Invalid operator conversion from Operator");
    return PLUS;
}

std::string getBinopStr(Binop binop) {
    switch (binop) {
        case NONE_OP:        return "";
        case EQUAL:          return "==";
        case UNEQUAL:        return "!=";
        case LESS:           return "<";
        case GREATER:        return ">";
        case LESS_OR_EQ:     return "<=";
        case GREATER_OR_EQ:  return ">=";
        case ASSIGNMENT:     return "=";
    }
    ErrorHandler::errorNotImplemented("Binop not implemented");
    return "";
}

Binop getFlippedBinop(Binop binop) {
    switch (binop) {
        case NONE_OP:        return NONE_OP;
        case EQUAL:          return EQUAL;
        case UNEQUAL:        return UNEQUAL;
        case LESS:           return GREATER;
        case GREATER:        return LESS;
        case LESS_OR_EQ:     return GREATER_OR_EQ;
        case GREATER_OR_EQ:  return LESS_OR_EQ;
        case ASSIGNMENT:     return ASSIGNMENT;
    }
    ErrorHandler::errorNotImplemented("Binop not implemented");
    return NONE_OP;
}

idx_t getPhysicalTypeSize(PhysicalType type) {
    switch (type) {
        case PhysicalType::TINYINT:    return sizeof(int8_t);
        case PhysicalType::SMALLINT:   return sizeof(int16_t);
        case PhysicalType::INTEGER:    return sizeof(int32_t);
        case PhysicalType::BIGINT:     return sizeof(int64_t);
        case PhysicalType::UTINYINT:   return sizeof(uint8_t);
        case PhysicalType::USMALLINT:  return sizeof(uint16_t);
        case PhysicalType::UINTEGER:   return sizeof(uint32_t);
        case PhysicalType::UBIGINT:    return sizeof(uint64_t);
        case PhysicalType::FLOAT:      return sizeof(float);
        case PhysicalType::DOUBLE:     return sizeof(double);
        case PhysicalType::STRING:     return sizeof(bumblebee::string_t);  // size of string_t object, not content ---> Make sure to not call for string
        default:
            ErrorHandler::errorNotImplemented("Invalid type conversion from ConstantType");
            return 0; // or throw an error
    }
}

bool isUnsigned(PhysicalType type) {
    switch (type) {
        case PhysicalType::UTINYINT:
        case PhysicalType::USMALLINT:
        case PhysicalType::UINTEGER:
        case PhysicalType::UBIGINT:
            return true;
        default:
            return false;
    }
}

PhysicalType getSignedBumpedType(PhysicalType type) {
    switch (type) {
        case PhysicalType::UTINYINT:   return PhysicalType::SMALLINT;
        case PhysicalType::USMALLINT:  return PhysicalType::INTEGER;
        case PhysicalType::UINTEGER:   return PhysicalType::BIGINT;
        case PhysicalType::UBIGINT:    return PhysicalType::BIGINT;  // Already largest unsigned int

        default:         return type;
    }
}

PhysicalType getGeneralBumpedType(PhysicalType type) {
    switch (type) {
        case PhysicalType::TINYINT:    return PhysicalType::BIGINT;
        case PhysicalType::SMALLINT:   return PhysicalType::BIGINT;
        case PhysicalType::INTEGER:    return PhysicalType::BIGINT;
        case PhysicalType::BIGINT:     return PhysicalType::BIGINT;

        case PhysicalType::UTINYINT:   return PhysicalType::BIGINT;
        case PhysicalType::USMALLINT:  return PhysicalType::BIGINT;
        case PhysicalType::UINTEGER:   return PhysicalType::BIGINT;
        case PhysicalType::UBIGINT:    return PhysicalType::BIGINT;

        case PhysicalType::FLOAT:      return PhysicalType::DOUBLE;
        case PhysicalType::DOUBLE:     return PhysicalType::DOUBLE;

        default:         return type;
    }
}

vector<LogicalType> getGeneralBumpedType(const vector<LogicalType>& type) {
    vector<LogicalType> res;
    for (auto& t : type)
        res.push_back(getGeneralBumpedType(t.getPhysicalType()));
    return res;
}

PhysicalType getBumpedType(PhysicalType type) {
    switch (type) {
        case PhysicalType::TINYINT:    return PhysicalType::SMALLINT;
        case PhysicalType::SMALLINT:   return PhysicalType::INTEGER;
        case PhysicalType::INTEGER:    return PhysicalType::BIGINT;
        case PhysicalType::BIGINT:     return PhysicalType::BIGINT;   // Already largest signed int

        case PhysicalType::UTINYINT:   return PhysicalType::USMALLINT;
        case PhysicalType::USMALLINT:  return PhysicalType::UINTEGER;
        case PhysicalType::UINTEGER:   return PhysicalType::UBIGINT;
        case PhysicalType::UBIGINT:    return PhysicalType::UBIGINT;  // Already largest unsigned int

        case PhysicalType::FLOAT:      return PhysicalType::DOUBLE;
        case PhysicalType::DOUBLE:     return PhysicalType::DOUBLE;   // Already largest floating point

        default:         return type;
    }
}

LogicalType getCommonType(LogicalType t1, LogicalType t2) {
    if (t1 == t2)return t1;
    if (t1.type() == LogicalTypeId::UNKNOWN && t2.type() != LogicalTypeId::UNKNOWN) return t2;
    if (t2.type() == LogicalTypeId::UNKNOWN && t1.type() != LogicalTypeId::UNKNOWN) return t1;
    // if one of the type is float or double return double
    if (t1.type() == LogicalTypeId::FLOAT || t1.type() == LogicalTypeId::DOUBLE || t2.type() == LogicalTypeId::FLOAT || t2.type() == LogicalTypeId::DOUBLE) return LogicalTypeId::DOUBLE;
    if (t1.type() == LogicalTypeId::DECIMAL || t2.type() == LogicalTypeId::DECIMAL
        || t1.type() == LogicalTypeId::STRING || t2.type() == LogicalTypeId::STRING){
        // error cannot cast string or decimal
        ErrorHandler::errorNotImplemented("Invalid type conversion from LogicalType");
        return LogicalTypeId::UNKNOWN;
    }
    // if the sign is different bump the unsigned one
    if (isUnsigned(t1.getPhysicalType()) && !isUnsigned(t2.getPhysicalType()))
        t1 = getSignedBumpedType(t1.getPhysicalType());
    if (isUnsigned(t2.getPhysicalType()) && !isUnsigned(t1.getPhysicalType()))
        t2 = getSignedBumpedType(t2.getPhysicalType());
    if (getPhysicalTypeSize(t1.getPhysicalType()) >= getPhysicalTypeSize(t2.getPhysicalType()))
        return t1;
    return t2;
}

string ctypeToString(PhysicalType type) {
    switch (type) {
        case PhysicalType::TINYINT:    return "TINYINT";
        case PhysicalType::SMALLINT:   return "SMALLINT";
        case PhysicalType::INTEGER:    return "INTEGER";
        case PhysicalType::BIGINT:     return "BIGINT";
        case PhysicalType::UTINYINT:   return "UTINYINT";
        case PhysicalType::USMALLINT:  return "USMALLINT";
        case PhysicalType::UINTEGER:   return "UINTEGER";
        case PhysicalType::UBIGINT:    return "UBIGINT";
        case PhysicalType::FLOAT:      return "FLOAT";
        case PhysicalType::DOUBLE:     return "DOUBLE";
        case PhysicalType::STRING:     return "STRING";
        case PhysicalType::UNKNOWN:    return "UNKNOWN";
        default:         return "INVALID_CONSTANT_TYPE";
    }
}


string logicalTypeToString(LogicalTypeId type) {
    switch (type) {
            case LogicalTypeId::TINYINT:    return "TINYINT";
            case LogicalTypeId::SMALLINT:   return "SMALLINT";
            case LogicalTypeId::INTEGER:    return "INTEGER";
            case LogicalTypeId::BIGINT:     return "BIGINT";
            case LogicalTypeId::UTINYINT:   return "UTINYINT";
            case LogicalTypeId::USMALLINT:  return "USMALLINT";
            case LogicalTypeId::UINTEGER:   return "UINTEGER";
            case LogicalTypeId::UBIGINT:    return "UBIGINT";
            case LogicalTypeId::HASH:       return "HASH";
            case LogicalTypeId::ADDRESS:    return "ADDRESS";
            case LogicalTypeId::FLOAT:      return "FLOAT";
            case LogicalTypeId::DOUBLE:     return "DOUBLE";
            case LogicalTypeId::STRING:     return "STRING";
            case LogicalTypeId::UNKNOWN:    return "UNKNOWN";
            case LogicalTypeId::BOOLEAN:    return "BOOLEAN";;
            case LogicalTypeId::DECIMAL:    return "DECIMAL";
            case LogicalTypeId::STRUCT:     return "STRUCT";
        default:         return "INVALID_CONSTANT_TYPE";
    }
}


PhysicalType ctypeFromString(const string& typeStr) {
    if (typeStr == "TINYINT")    return PhysicalType::TINYINT;
    if (typeStr == "SMALLINT")   return PhysicalType::SMALLINT;
    if (typeStr == "INTEGER")    return PhysicalType::INTEGER;
    if (typeStr == "BIGINT")     return PhysicalType::BIGINT;
    if (typeStr == "UTINYINT")   return PhysicalType::UTINYINT;
    if (typeStr == "USMALLINT")  return PhysicalType::USMALLINT;
    if (typeStr == "UINTEGER")   return PhysicalType::UINTEGER;
    if (typeStr == "UBIGINT")    return PhysicalType::UBIGINT;
    if (typeStr == "FLOAT")      return PhysicalType::FLOAT;
    if (typeStr == "DOUBLE")     return PhysicalType::DOUBLE;
    if (typeStr == "STRING")     return PhysicalType::STRING;
    if (typeStr == "UNKNOWN")    return PhysicalType::UNKNOWN;

    return PhysicalType::UNKNOWN; // default case
}

idx_t nextPowerOfTwo(idx_t n) {
    if (n == 0) return 1;
    // If already a power of two, return n
    if ((n & (n - 1)) == 0) return n;

    // Fill all bits below the highest set bit
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32; // Needed for 64-bit numbers
    n++;
    return n;
}

bool typeIsConstantSize(PhysicalType type) {
    switch (type) {
        case PhysicalType::TINYINT:
        case PhysicalType::SMALLINT:
        case PhysicalType::INTEGER:
        case PhysicalType::BIGINT:
        case PhysicalType::UTINYINT:
        case PhysicalType::USMALLINT:
        case PhysicalType::UINTEGER:
        case PhysicalType::UBIGINT:
        case PhysicalType::FLOAT:
        case PhysicalType::DOUBLE:
            return true;
        case PhysicalType::STRING:
        default:
            return false;
    }
}

}
