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
#include <unordered_set>

#include "bumblebee/common/ErrorHandler.h"
#include "bumblebee/common/TypeDefs.h"
#include "bumblebee/common/type/StringT.h"

namespace bumblebee {


enum TermType{
	CONSTANT = 0,
	VARIABLE = 1,
	ARITH = 2,
	RANGE = 3
};

enum ConstantType {
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
	STRING = 10
};

enum Operator {
	PLUS=0,
	MINUS=1,
	DIV=2,
	TIMES=3,
	MODULO=4
};

struct IntervalTerm {
	int from;
	int to;
};

using set_term_variable = std::unordered_set<StringT>;

/*  class that store a Term
 *
 **/
class Term {
	using TermVector = std::vector<Term>;
	using OpVector = std::vector<Operator>;

public:
	Term();
	Term(bool negative);
	Term(Term&& term);
	Term(const Term& term) = delete;
	Term(int8_t c);
	Term(int16_t c);
	Term(int32_t c);
	Term(int64_t c);
	Term(uint8_t c);
	Term(uint16_t c);
	Term(uint32_t c);
	Term(uint64_t c);
	Term(float c);
	Term(double c);
	Term(StringT&& c);
	Term(char* c);
	Term(StringT&& c, bool isVariable);
	Term(IntervalTerm interval_);
	Term(Term&& t1,Term&& t2, Operator op);
	~Term() = default;

	Term& operator=(Term&&) = default;
	Term& operator=(Term& term) = delete;

	friend bool operator==(const Term &lhs, const Term &rhs);
	friend bool operator!=(const Term &lhs, const Term &rhs);

	hash_t hash();
	bool isNegative();
	void setNegative(bool n);
	TermType getType();
	ConstantType getConstantType();
	void setType(TermType type);
	bool isGround();
	void getVariables(set_term_variable& vars);
	std::string toString()const;
	bool isAnonymous();
	void addInArithTermBegin(Term&& term, Operator op);
	void addInArithTerm(Term&& term, Operator op);
	void addInArithTerm(Term&& term, char op);

	// template

	template <class T>
	T getNumericValue() {
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

	template<class T>
	static Term createConstantTerm(T value) {
		ErrorHandler::errorNotImplemented("Term values not supported");
		return {};
	}


private:
    // if it is negative term
    bool negative_;

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
	StringT stringValue_{};
	// If it is interval the interval from to
	IntervalTerm interval_{};
	// If constant the constant type
	ConstantType ctype_;
	// Type of the term Constant/Variable/etc.
	TermType type_;
	// True if is anonymous variable
	bool anonymous_{false};
	// ArithTerm is an arithmetic with the list of a terms
	// and the relative operator(+, -, *, /,\)
	TermVector terms_{};
	OpVector operators_{};

public:
	// static functions
	static Term createVariable(std::string&& value);
	static Operator getOperator(char sop);
	static char getOperatorChar(Operator op);
	static Term createRange(int from, int to);
	static Term createArith(Term&& t1,Term&& t2, char op );
	static void setConstantNumericTerm(Term& term, long long value);
	static void setConstantNumericTerm(Term& term, unsigned long long value);
	static Term createSmallestConstantNumericTerm(unsigned long long value);
	static Term createSmallestConstantNumericTerm(long long value);

	static constexpr const char* anonymous_variable = "_";
};


template<>
Term Term::createConstantTerm(bool value);

template<>
Term  Term::createConstantTerm(uint8_t value);
template <>
Term  Term::createConstantTerm(uint16_t value);
template <>
Term  Term::createConstantTerm(uint32_t value);
template <>
Term  Term::createConstantTerm(uint64_t value);
template <>
Term  Term::createConstantTerm(int8_t value);
template <>
Term  Term::createConstantTerm(int16_t value);
template <>
Term  Term::createConstantTerm(int32_t value);
template <>
Term  Term::createConstantTerm(int64_t value);
template <>
Term  Term::createConstantTerm(std::string&& value);
template <>
Term  Term::createConstantTerm(char* value);
template <>
Term  Term::createConstantTerm(float value);
template <>
Term  Term::createConstantTerm(double value);
template <>
Term  Term::createConstantTerm(IntervalTerm value);

} // bumblebee
