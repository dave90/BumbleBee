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

#include "BumbleString.hpp"
#include "bumblebee/common/ErrorHandler.hpp"
#include "bumblebee/common/TypeDefs.hpp"

namespace bumblebee{


class Value {

    friend class Vector;
    friend class Term;

public:
    Value();
	Value(Value&& term);
    Value(int8_t c);
	Value(int16_t c);
	Value(int32_t c);
	Value(int64_t c);
	Value(uint8_t c);
	Value(uint16_t c);
	Value(uint32_t c);
	Value(uint64_t c);
	Value(float c);
	Value(double c);
	Value(string&& c);
	Value(const string& c);
	Value(string_t c);
	Value(const char* c);
	~Value() = default;

	Value& operator=(Value&&) = default;
	Value& operator=(Value& term) = delete;

	friend bool operator==(const Value &lhs, const Value &rhs);
	friend bool operator!=(const Value &lhs, const Value &rhs);

	bool operator<(const Value &r) const;
	bool operator<=(const Value &r) const;
	bool operator>(const Value &r) const;
	bool operator>=(const Value &r) const;

	bool isDoubleQuotedString() const;

	ConstantType getConstantType() const;
	void setConstantType(ConstantType type);
	std::string toString()const;
	// Create a new Value casted base on type
	Value cast(ConstantType type)const;

	bool tryCastAs(ConstantType type, Value & newValue, string* error) const;

	// template
	template <class T>
	T getNumericValue() const{
		switch (ctype_) {
			case ConstantType::TINYINT:
				return static_cast<T>(value_.tinyint);
			case ConstantType::SMALLINT:
				return static_cast<T>(value_.smallint);
			case ConstantType::INTEGER:
				return static_cast<T>(value_.integer);
			case ConstantType::BIGINT:
				return static_cast<T>(value_.bigint);
			case ConstantType::UTINYINT:
				return static_cast<T>(value_.utinyint);
			case ConstantType::USMALLINT:
				return static_cast<T>(value_.usmallint);
			case ConstantType::UINTEGER:
				return static_cast<T>(value_.uinteger);
			case ConstantType::UBIGINT:
				return static_cast<T>(value_.ubigint);
			case ConstantType::FLOAT:
				return static_cast<T>(value_.float_);
			case ConstantType::DOUBLE:
				return static_cast<T>(value_.double_);
			default:
				;
		}
		ErrorHandler::errorNotImplemented("Term getValue not supported");
		return {};
	}


    // Returns the internal value. This method does not perform casting, and assumes T matches the
	// type of the value. Only use this if you know what you are doing.
	template <class T>
	T& getValueUnsafe() {
		ErrorHandler::errorNotImplemented("Unimplemented template type for Value::GetValueUnsafe");
		return this;
	}

	static Value cast(ConstantType type, data_ptr_t data);


private:

	// Numeric Values
	union Val {
		int8_t tinyint;
		int16_t smallint;
		int32_t integer;
		int64_t bigint;
		uint8_t utinyint;
		uint16_t usmallint;
		uint32_t uinteger;
		uint64_t ubigint;
		float float_;
		double double_;
	} value_;
	// String value
	string stringValue_{};
	// If constant the constant type
	ConstantType ctype_{INTEGER};

};


template<>
uint8_t&  Value::getValueUnsafe();
template <>
uint16_t&  Value::getValueUnsafe();
template <>
uint32_t&  Value::getValueUnsafe();
template <>
uint64_t&  Value::getValueUnsafe();
template <>
int8_t&  Value::getValueUnsafe();
template <>
int16_t&  Value::getValueUnsafe();
template <>
int32_t&  Value::getValueUnsafe();
template <>
int64_t&  Value::getValueUnsafe();
template <>
string&  Value::getValueUnsafe();
template <>
float&  Value::getValueUnsafe();
template <>
double&  Value::getValueUnsafe();



}
