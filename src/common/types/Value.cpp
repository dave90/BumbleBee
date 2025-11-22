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
#include "bumblebee/common/types/Value.hpp"

#include "bumblebee/common/types/Vector.hpp"
#include "bumblebee/common/vector_operations/VectorOperations.hpp"

namespace bumblebee{
Value::Value() {}

Value::Value(Value &&term):ctype_(term.ctype_), value_(term.value_), stringValue_(std::move(term.stringValue_)) {
}

Value::Value(int8_t c)
    : ctype_(ConstantType::TINYINT),  value_({.tinyint = c}) {}

Value::Value(int16_t c)
    : ctype_(ConstantType::SMALLINT),  value_({.smallint = c}) {}

Value::Value(int32_t c)
    : ctype_(ConstantType::INTEGER),  value_({.integer = c}) {}

Value::Value(int64_t c)
    : ctype_(ConstantType::BIGINT),  value_({.bigint = c}) {}

Value::Value(uint8_t c)
    : ctype_(ConstantType::UTINYINT),  value_({.utinyint = c}) {}

Value::Value(uint16_t c)
    : ctype_(ConstantType::USMALLINT),  value_({.usmallint = c}) {}

Value::Value(uint32_t c)
    : ctype_(ConstantType::UINTEGER),  value_({.uinteger = c}) {}

Value::Value(uint64_t c)
    : ctype_(ConstantType::UBIGINT),  value_({.ubigint = c}) {}

Value::Value(float c)
    : ctype_(ConstantType::FLOAT),  value_({.float_ = c}) {}

Value::Value(double c)
    : ctype_(ConstantType::DOUBLE),  value_({.double_ = c}) {}

Value::Value(string &&c) :ctype_(STRING), stringValue_(std::move(c)) {}

Value::Value(const string& c) :ctype_(STRING), stringValue_(c) {}

Value::Value(string_t c):ctype_(STRING), stringValue_(c.c_str(), c.size()) {}

Value::Value(const char *c):ctype_(STRING), stringValue_(c) {}

bool operator==(const Value &lhs, const Value &rhs) {
    if (lhs.ctype_ != rhs.ctype_) return false;
    switch (lhs.ctype_) {
        case ConstantType::TINYINT:
            return lhs.value_.utinyint == rhs.value_.utinyint;
        case ConstantType::SMALLINT:
            return lhs.value_.smallint == rhs.value_.smallint;
        case ConstantType::INTEGER:
            return lhs.value_.integer == rhs.value_.integer;
        case ConstantType::BIGINT:
            return lhs.value_.bigint == rhs.value_.bigint;
        case ConstantType::UTINYINT:
            return lhs.value_.utinyint == rhs.value_.utinyint;
        case ConstantType::USMALLINT:
            return lhs.value_.usmallint == rhs.value_.usmallint;
        case ConstantType::UINTEGER:
            return lhs.value_.uinteger == rhs.value_.uinteger;
        case ConstantType::UBIGINT:
            return lhs.value_.ubigint == rhs.value_.ubigint;
        case ConstantType::FLOAT:
            return lhs.value_.float_ == rhs.value_.float_;
        case ConstantType::DOUBLE:
            return lhs.value_.double_ == rhs.value_.double_;
        case ConstantType::STRING:
            return lhs.stringValue_ == rhs.stringValue_;

        default:
            ;
    }
    return false;
}

bool operator!=(const Value &lhs, const Value &rhs) {
    return !(lhs == rhs);
}

bool Value::operator<(const Value &rhs) const {
    BB_ASSERT(ctype_ == rhs.ctype_);
    switch (ctype_) {
        case ConstantType::TINYINT:
            return value_.utinyint < rhs.value_.utinyint;
        case ConstantType::SMALLINT:
            return value_.smallint < rhs.value_.smallint;
        case ConstantType::INTEGER:
            return value_.integer < rhs.value_.integer;
        case ConstantType::BIGINT:
            return value_.bigint < rhs.value_.bigint;
        case ConstantType::UTINYINT:
            return value_.utinyint < rhs.value_.utinyint;
        case ConstantType::USMALLINT:
            return value_.usmallint < rhs.value_.usmallint;
        case ConstantType::UINTEGER:
            return value_.uinteger < rhs.value_.uinteger;
        case ConstantType::UBIGINT:
            return value_.ubigint < rhs.value_.ubigint;
        case ConstantType::FLOAT:
            return value_.float_ < rhs.value_.float_;
        case ConstantType::DOUBLE:
            return value_.double_ < rhs.value_.double_;
        case ConstantType::STRING:
            return stringValue_ < rhs.stringValue_;

        default:
            ;
    }
    return false;
}

bool Value::operator>(const Value &r) const {
    return !(*this <= r);
}

bool Value::operator<=(const Value &r) const {
    if (*this == r) return true;
    return *this < r;
}

bool Value::operator>=(const Value &r) const {
    return ! (*this < r);
}

bool Value::isDoubleQuotedString() const {
    return ctype_ == STRING
     && stringValue_.length() >= 2
     && stringValue_[0] == '"' && stringValue_[stringValue_.length() - 1] == '"';
}

ConstantType Value::getConstantType() const{
    return ctype_;
}

void Value::setConstantType(ConstantType type) {
    ctype_ = type;
}

std::string Value::toString() const {
    switch (ctype_) {
        case ConstantType::TINYINT:
            return std::to_string(value_.tinyint);
        case ConstantType::SMALLINT:
            return std::to_string(value_.smallint);
        case ConstantType::INTEGER:
            return std::to_string(value_.integer);
        case ConstantType::BIGINT:
            return std::to_string(value_.bigint);
        case ConstantType::UTINYINT:
            return std::to_string(value_.utinyint);
        case ConstantType::USMALLINT:
            return std::to_string(value_.usmallint);
        case ConstantType::UINTEGER:
            return std::to_string(value_.uinteger);
        case ConstantType::UBIGINT:
            return std::to_string(value_.ubigint);
        case ConstantType::FLOAT:
            return std::to_string(value_.float_);
        case ConstantType::DOUBLE:
            return std::to_string(value_.double_);
        case ConstantType::STRING:
            return stringValue_;
        default:
            return "";
    }
}


Value Value::cast(ConstantType type) const {
    if (ctype_ == ConstantType::STRING && type == ConstantType::STRING)
        return Value(stringValue_.c_str());

    switch (type) {
        case ConstantType::TINYINT:
            return Value(getNumericValue<int8_t>());
        case ConstantType::SMALLINT:
            return Value(getNumericValue<int16_t>());
        case ConstantType::INTEGER:
            return Value(getNumericValue<int32_t>());
        case ConstantType::BIGINT:
            return Value(getNumericValue<int64_t>());
        case ConstantType::UTINYINT:
            return Value(getNumericValue<uint8_t>());
        case ConstantType::USMALLINT:
            return Value(getNumericValue<uint16_t>());
        case ConstantType::UINTEGER:
            return Value(getNumericValue<uint32_t>());
        case ConstantType::UBIGINT:
            return Value(getNumericValue<uint64_t>());
        case ConstantType::FLOAT:
            return Value(getNumericValue<float>());
        case ConstantType::DOUBLE:
            return Value(getNumericValue<double>());
        case ConstantType::STRING:
            return Value(toString());
        case ConstantType::UNKNOWN:
            ;
    }
    ErrorHandler::errorNotImplemented("Cast not implemented.");
    return {};
}

bool Value::tryCastAs(ConstantType type, Value &newValue, string *error) const {
    newValue = cast(ctype_);
    if (ctype_ == type) {
        return true;
    }
    if (newValue.isDoubleQuotedString()) {
        // remove the '"' before casting
        newValue.stringValue_ = newValue.stringValue_.substr(1, newValue.stringValue_.length() - 2);
    }

    Vector input(newValue);
    Vector result(type);
    if (!VectorOperations::tryCast(input, result, 1, error)) {
        return false;
    }
    newValue = result.getValue(0);
    return true;
}

Value Value::cast(ConstantType type, data_ptr_t data) {
    switch (type) {
        case ConstantType::TINYINT:
            return Value( * ((int8_t*)data ));
        case ConstantType::SMALLINT:
            return Value(*((int16_t*)data));
        case ConstantType::INTEGER:
            return Value(*((int32_t*)data));
        case ConstantType::BIGINT:
            return Value(*((int64_t*)data));
        case ConstantType::UTINYINT:
            return Value(*((uint8_t*)data));
        case ConstantType::USMALLINT:
            return Value(*((uint16_t*)data));
        case ConstantType::UINTEGER:
            return Value(*((uint32_t*)data));
        case ConstantType::UBIGINT:
            return Value(*((uint64_t*)data));
        case ConstantType::FLOAT:
            return Value(*((float*)data));
        case ConstantType::DOUBLE:
            return Value(*((double*)data));
        case ConstantType::STRING:
            return Value(*(string_t*)data );
        case ConstantType::UNKNOWN:
            ;
    }
    ErrorHandler::errorNotImplemented("Cast not implemented.");
    return {};
}


// --------------------  Template ------------------

template<>
uint8_t& Value::getValueUnsafe() {
    return value_.utinyint;
}

template<>
uint16_t& Value::getValueUnsafe() {
    return value_.usmallint;
}

template<>
uint32_t& Value::getValueUnsafe() {
    return value_.uinteger;
}

template<>
uint64_t& Value::getValueUnsafe() {
    return value_.ubigint;
}

template<>
int8_t& Value::getValueUnsafe() {
    return value_.tinyint;
}

template<>
int16_t& Value::getValueUnsafe() {
    return value_.smallint;
}

template<>
int32_t& Value::getValueUnsafe() {
    return value_.integer;
}

template<>
int64_t& Value::getValueUnsafe() {
    return value_.bigint;
}

template<>
string & Value::getValueUnsafe() {
    return stringValue_;
}


template<>
float & Value::getValueUnsafe() {
    return value_.float_;
}

template<>
double & Value::getValueUnsafe() {
    return value_.double_;
}
}
