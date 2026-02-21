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




LogicalTypeId physicalTypeToLogicalType(PhysicalType type) {
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

PhysicalType logicalTypeToPhysicalType(LogicalTypeId type) {
    switch (type) {
        case LogicalTypeId::TINYINT:   return PhysicalType::TINYINT;
        case LogicalTypeId::SMALLINT:  return PhysicalType::SMALLINT;
        case LogicalTypeId::DATE:
        case LogicalTypeId::INTEGER:   return PhysicalType::INTEGER;
        case LogicalTypeId::DECIMAL:
        case LogicalTypeId::TIMESTAMP:
        case LogicalTypeId::BIGINT:    return PhysicalType::BIGINT;
        case LogicalTypeId::BOOLEAN:
        case LogicalTypeId::UTINYINT:  return PhysicalType::UTINYINT;
        case LogicalTypeId::USMALLINT: return PhysicalType::USMALLINT;
        case LogicalTypeId::UINTEGER:  return PhysicalType::UINTEGER;
        case LogicalTypeId::HASH:
        case LogicalTypeId::ADDRESS:
        case LogicalTypeId::UBIGINT:   return PhysicalType::UBIGINT;

        case LogicalTypeId::FLOAT:     return PhysicalType::FLOAT;
        case LogicalTypeId::DOUBLE:    return PhysicalType::DOUBLE;

        case LogicalTypeId::STRING:    return PhysicalType::STRING;

        case LogicalTypeId::STRUCT:
        case LogicalTypeId::UNKNOWN:
        default:
            return PhysicalType::UNKNOWN;
    }
}

LogicalType::LogicalType(LogicalTypeId type): type_(type), ctype_(logicalTypeToPhysicalType(type)) {
    if (type == LogicalTypeId::DECIMAL)
        data_ = std::make_shared<LogicalTypeDecimalData>();
}

PhysicalType LogicalType::getPhysicalType() const {
    return ctype_;
}

std::string LogicalType::toString() const {
    string result = "LogicalType(" + logicalTypeToString(type_)+", PType("+physicalTypeToString(ctype_)+"))";
    return result;
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
    type.data_ = std::make_shared<LogicalTypeDecimalData>(width, scale);
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
        case Operator::PLUS:
            return '+';
        case Operator::MINUS:
            return '-';
        case Operator::DIV:
            return '/';
        case Operator::TIMES:
            return '*';
        case Operator::MODULO:
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
            return Operator::PLUS;
        case '-':
            return Operator::MINUS;
        case '/':
            return Operator::DIV;
        case '*':
            return Operator::TIMES;
        case '%':
            return Operator::MODULO;
        default:
            ;
    }
    ErrorHandler::errorNotImplemented("Invalid operator conversion from Operator");
    return Operator::PLUS;
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

bool isUnsigned(const LogicalType& type) {
    switch (type.getPhysicalType()) {
        case PhysicalType::UTINYINT:
        case PhysicalType::USMALLINT:
        case PhysicalType::UINTEGER:
        case PhysicalType::UBIGINT:
            return true;
        default:
            return false;
    }
}

LogicalType getSignedBumpedType(const LogicalType& type) {
    switch (type.getPhysicalType()) {
        case PhysicalType::UTINYINT:   return PhysicalType::SMALLINT;
        case PhysicalType::USMALLINT:  return PhysicalType::INTEGER;
        case PhysicalType::UINTEGER:   return PhysicalType::BIGINT;
        case PhysicalType::UBIGINT:    return PhysicalType::BIGINT;  // Already largest unsigned int

        default:         return type;
    }
}

LogicalType getGeneralBumpedType(LogicalType type) {
    switch (type.type()) {
        case LogicalTypeId::TINYINT:    return LogicalTypeId::BIGINT;
        case LogicalTypeId::SMALLINT:   return LogicalTypeId::BIGINT;
        case LogicalTypeId::INTEGER:    return LogicalTypeId::BIGINT;
        case LogicalTypeId::BIGINT:     return LogicalTypeId::BIGINT;

        case LogicalTypeId::UTINYINT:   return LogicalTypeId::BIGINT;
        case LogicalTypeId::USMALLINT:  return LogicalTypeId::BIGINT;
        case LogicalTypeId::UINTEGER:   return LogicalTypeId::BIGINT;
        case LogicalTypeId::UBIGINT:    return LogicalTypeId::BIGINT;

        case LogicalTypeId::FLOAT:      return LogicalTypeId::DOUBLE;
        case LogicalTypeId::DOUBLE:     return LogicalTypeId::DOUBLE;

        default:         return type;
    }
}

vector<LogicalType> getGeneralBumpedType(const vector<LogicalType>& type) {
    vector<LogicalType> res;
    for (auto& t : type)
        res.push_back(getGeneralBumpedType(t.getPhysicalType()));
    return res;
}

PhysicalType getBumpedPhysicalType(PhysicalType type) {
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

LogicalType getBumpedLogicalType(LogicalType type) {
    switch (type.type()) {
        case LogicalTypeId::BOOLEAN:
            return type;
        case LogicalTypeId::DECIMAL:
            switch (type.getPhysicalType()) {
                case PhysicalType::SMALLINT:
                    return LogicalType::createDecimal(Decimal::MAX_WIDTH_INT32, type.getDecimalData().scale_);
                default:
                    return LogicalType::createDecimal(Decimal::MAX_WIDTH_INT64, type.getDecimalData().scale_);
            }
        case LogicalTypeId::DATE:
            return type;
        case LogicalTypeId::TIMESTAMP:
            return type;
        default:
            return getBumpedPhysicalType(type.getPhysicalType());

    }
}

LogicalType getCommonTypeDecimal(LogicalType t1, LogicalType t2, Operator op ){
    if (t1.type() != LogicalTypeId::DECIMAL && t2.type() != LogicalTypeId::DECIMAL) return LogicalTypeId::UNKNOWN;
    // promote integer to decimal with scale 0
    if (t1.type() == LogicalTypeId::DECIMAL && t2.type() != LogicalTypeId::DECIMAL) {
        if (t2.type() == LogicalTypeId::FLOAT || t2.type() == LogicalTypeId::DOUBLE) return LogicalTypeId::DOUBLE;
        t2 = LogicalType::createDecimal(Decimal::MAX_WIDTH_INT64, 0);
    } else if (t2.type() == LogicalTypeId::DECIMAL && t1.type() != LogicalTypeId::DECIMAL) {
        if (t1.type() == LogicalTypeId::FLOAT || t1.type() == LogicalTypeId::DOUBLE) return LogicalTypeId::DOUBLE;
        t1 = LogicalType::createDecimal(Decimal::MAX_WIDTH_INT64, 0);
    }
    // both are decimal
    // calculate the common scale
    int s1 = t1.getDecimalData().scale_;
    int s2 = t2.getDecimalData().scale_;
    if ( op  == Operator::TIMES ) {
        // we need to sum the scale when multiply 2 decimal
        return LogicalType::createDecimal(Decimal::MAX_WIDTH_INT64, s1+s2);
    }
    // return the max scale
    return LogicalType::createDecimal(Decimal::MAX_WIDTH_INT64, maxValue(s1,s2));
}
LogicalType getCommonType(LogicalType t1, LogicalType t2, Operator op ) {
    // decimal type
    if (t1.type() == LogicalTypeId::DECIMAL || t2.type() == LogicalTypeId::DECIMAL) return getCommonTypeDecimal(t1,t2, op);
    // same types
    if (t1 == t2) return t1;
    // unknown type
    if (t1.type() == LogicalTypeId::UNKNOWN && t2.type() != LogicalTypeId::UNKNOWN) return t2;
    if (t2.type() == LogicalTypeId::UNKNOWN && t1.type() != LogicalTypeId::UNKNOWN) return t1;

    // if one of the type is float or double return double
    if (t1.type() == LogicalTypeId::FLOAT || t1.type() == LogicalTypeId::DOUBLE || t2.type() == LogicalTypeId::FLOAT || t2.type() == LogicalTypeId::DOUBLE) return LogicalTypeId::DOUBLE;
    // unsupported common types
    if (t1.type() == LogicalTypeId::STRING || t2.type() == LogicalTypeId::STRING
        || t1.type() == LogicalTypeId::DATE || t2.type() == LogicalTypeId::DATE
        || t1.type() == LogicalTypeId::TIMESTAMP || t2.type() == LogicalTypeId::TIMESTAMP
    ){
        return LogicalTypeId::UNKNOWN;
    }
    // if the sign is different bump the unsigned one
    if (isUnsigned(t1) && !isUnsigned(t2))
        t1 = getSignedBumpedType(t1);
    if (isUnsigned(t2) && !isUnsigned(t1))
        t2 = getSignedBumpedType(t2);
    if (getPhysicalTypeSize(t1.getPhysicalType()) >= getPhysicalTypeSize(t2.getPhysicalType()))
        return t1;
    return t2;
}

string physicalTypeToString(PhysicalType type) {
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


PhysicalType physicalTypeFromString(const string& typeStr) {
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

LogicalType logicalTypeFromString(const string& typeStr) {
    if (typeStr == "TINYINT")    return LogicalTypeId::TINYINT;
    if (typeStr == "SMALLINT")   return LogicalTypeId::SMALLINT;
    if (typeStr == "INTEGER")    return LogicalTypeId::INTEGER;
    if (typeStr == "BIGINT")     return LogicalTypeId::BIGINT;

    if (typeStr == "UTINYINT")   return LogicalTypeId::UTINYINT;
    if (typeStr == "USMALLINT")  return LogicalTypeId::USMALLINT;
    if (typeStr == "UINTEGER")   return LogicalTypeId::UINTEGER;
    if (typeStr == "UBIGINT")    return LogicalTypeId::UBIGINT;

    if (typeStr == "HASH")       return LogicalTypeId::HASH;
    if (typeStr == "ADDRESS")    return LogicalTypeId::ADDRESS;

    if (typeStr == "FLOAT")      return LogicalTypeId::FLOAT;
    if (typeStr == "DOUBLE")     return LogicalTypeId::DOUBLE;

    if (typeStr == "STRING")     return LogicalTypeId::STRING;

    if (typeStr == "BOOLEAN")    return LogicalTypeId::BOOLEAN;

    // Optional support if you want these (you already have factories)
    if (typeStr == "DATE")       return LogicalType::createDate();
    if (typeStr == "TIMESTAMP")  return LogicalType::createTimestamp();

    if (typeStr == "STRUCT")     return LogicalTypeId::STRUCT;
    if (typeStr == "UNKNOWN")    return LogicalTypeId::UNKNOWN;


    // DECIMAL(precision,scale)
    if (typeStr.rfind("DECIMAL", 0) == 0) {
        // Expect: "DECIMAL(p,s)" with no extra spaces (as requested).
        if (typeStr.size() < 10) {
            ErrorHandler::errorNotImplemented("Invalid DECIMAL type string (expected DECIMAL(p,s))");
            return LogicalTypeId::UNKNOWN;
        }
        if (typeStr[7] != '(' || typeStr.back() != ')') {
            ErrorHandler::errorNotImplemented("Invalid DECIMAL type string (expected DECIMAL(p,s))");
            return LogicalTypeId::UNKNOWN;
        }

        // Extract inside "(...)"
        string inside = typeStr.substr(8, typeStr.size() - 9);
        auto commaPos = inside.find(',');
        if (commaPos == string::npos) {
            ErrorHandler::errorNotImplemented("Invalid DECIMAL type string (expected DECIMAL(p,s))");
            return LogicalTypeId::UNKNOWN;
        }

        string pStr = inside.substr(0, commaPos);
        string sStr = inside.substr(commaPos + 1);

        // Parse ints (no spaces expected)
        int precision = 0;
        int scale = 0;

        if (pStr.empty() || sStr.empty()) {
            ErrorHandler::errorNotImplemented("Invalid DECIMAL type string (expected DECIMAL(p,s))");
            return LogicalTypeId::UNKNOWN;
        }

        for (auto c : pStr) {
            if (c < '0' || c > '9') {
                ErrorHandler::errorNotImplemented("Invalid DECIMAL precision (expected digits only)");
                return LogicalTypeId::UNKNOWN;
            }
            precision = precision * 10 + (c - '0');
        }
        for (auto c : sStr) {
            if (c < '0' || c > '9') {
                ErrorHandler::errorNotImplemented("Invalid DECIMAL scale (expected digits only)");
                return LogicalTypeId::UNKNOWN;
            }
            scale = scale * 10 + (c - '0');
        }
        return LogicalType::createDecimal(precision, scale);
    }

    return LogicalTypeId::UNKNOWN;
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

bool operator==(const LogicalType &lhs, const LogicalType &rhs) {
    return lhs.ctype_ == rhs.ctype_
           && lhs.type_ == rhs.type_
           && (lhs.data_ == rhs.data_ || lhs.data_ && rhs.data_ && lhs.data_->equals( *rhs.data_));
}

bool operator!=(const LogicalType &lhs, const LogicalType &rhs) {
    return !(lhs == rhs);
}
}
