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

#include "bumblebee/common/TypeDefs.h"

#include "bumblebee/common/ErrorHandler.h"
#include "bumblebee/common/types/BumbleString.h"
namespace bumblebee {


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

idx_t getCTypeSize(ConstantType type) {
    switch (type) {
        case TINYINT:    return sizeof(int8_t);
        case SMALLINT:   return sizeof(int16_t);
        case INTEGER:    return sizeof(int32_t);
        case BIGINT:     return sizeof(int64_t);
        case UTINYINT:   return sizeof(uint8_t);
        case USMALLINT:  return sizeof(uint16_t);
        case UINTEGER:   return sizeof(uint32_t);
        case UBIGINT:    return sizeof(uint64_t);
        case FLOAT:      return sizeof(float);
        case DOUBLE:     return sizeof(double);
        case STRING:     return sizeof(bumblebee::string_t);  // size of string_t object, not content ---> Make sure to not call for string
        default:
            ErrorHandler::errorNotImplemented("Invalid type conversion from ConstantType");
            return 0; // or throw an error
    }
}

bool isUnsigned(ConstantType type) {
    switch (type) {
        case UTINYINT:
        case USMALLINT:
        case UINTEGER:
        case UBIGINT:
            return true;
        default:
            return false;
    }
}

ConstantType getSignedBumpedType(ConstantType type) {
    switch (type) {
        case UTINYINT:   return SMALLINT;
        case USMALLINT:  return INTEGER;
        case UINTEGER:   return BIGINT;
        case UBIGINT:    return BIGINT;  // Already largest unsigned int

        default:         return type;
    }
}

ConstantType getBumpedType(ConstantType type) {
    switch (type) {
        case TINYINT:    return SMALLINT;
        case SMALLINT:   return INTEGER;
        case INTEGER:    return BIGINT;
        case BIGINT:     return BIGINT;   // Already largest signed int

        case UTINYINT:   return USMALLINT;
        case USMALLINT:  return UINTEGER;
        case UINTEGER:   return UBIGINT;
        case UBIGINT:    return UBIGINT;  // Already largest unsigned int

        case FLOAT:      return DOUBLE;
        case DOUBLE:     return DOUBLE;   // Already largest floating point

        default:         return type;
    }
}

ConstantType getCommonType(ConstantType t1, ConstantType t2) {
    if (t1 == t2)return t1;
    if (t1 == UNKNOWN && t2 != UNKNOWN) return t2;
    if (t2 == UNKNOWN && t1 != UNKNOWN) return t1;
    // if one of the type is float or double return double
    if (t1 == FLOAT || t1 == DOUBLE || t2 == FLOAT || t2 == DOUBLE) return DOUBLE;
    // if the sign is different bump the unsigned one
    if (isUnsigned(t1) && !isUnsigned(t2))
        t1 = getSignedBumpedType(t1);
    if (isUnsigned(t2) && !isUnsigned(t1))
        t2 = getSignedBumpedType(t2);
    if (getCTypeSize(t1) >= getCTypeSize(t2))
        return t1;
    return t2;
}

string ctypeToString(ConstantType type) {
    switch (type) {
        case TINYINT:    return "TINYINT";
        case SMALLINT:   return "SMALLINT";
        case INTEGER:    return "INTEGER";
        case BIGINT:     return "BIGINT";
        case UTINYINT:   return "UTINYINT";
        case USMALLINT:  return "USMALLINT";
        case UINTEGER:   return "UINTEGER";
        case UBIGINT:    return "UBIGINT";
        case FLOAT:      return "FLOAT";
        case DOUBLE:     return "DOUBLE";
        case STRING:     return "STRING";
        case UNKNOWN:    return "UNKNOWN";
        default:         return "INVALID_CONSTANT_TYPE";
    }
}

Binop getFlippedBinop(Binop op) {
    switch (op) {
        case EQUAL:         return EQUAL;
        case UNEQUAL:       return UNEQUAL;
        case LESS:          return GREATER;
        case GREATER:       return LESS;
        case LESS_OR_EQ:    return GREATER_OR_EQ;
        case GREATER_OR_EQ: return LESS_OR_EQ;
        case ASSIGNMENT:    return ASSIGNMENT;
        case NONE_OP:       return NONE_OP;
        default:            return NONE_OP;
    }
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

}
