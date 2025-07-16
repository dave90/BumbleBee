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
#include "bumblebee/common/types/BumbleString.h"
#include "bumblebee/common/types/Value.h"

namespace bumblebee {


enum TermType{
	CONSTANT = 0,
	VARIABLE = 1,
	ARITH = 2,
	RANGE = 3
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

using set_term_variable_t = std::unordered_set<string>;

/*  class that store a Term
 *
 **/
class Term {
	using term_vector_t = std::vector<Term>;
	using op_vector_t = std::vector<Operator>;

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
	Term(string&& c);
	Term(char* c);
	Term(string&& c, bool isVariable);
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
	TermType getType() const;
	ConstantType getConstantType();
	void setType(TermType type);
	bool isGround();
	void getVariables(set_term_variable_t& vars);
	std::string toString()const;
	bool isAnonymous();
	void addInArithTermBegin(Term&& term, Operator op);
	void addInArithTerm(Term&& term, Operator op);
	void addInArithTerm(Term&& term, char op);
	bool containsAnonymous()const;

	inline const Value& getValue()const {
		return value_;
	}

	inline const IntervalTerm& getInterval()const {
		return interval_;
	}

	// template

	template <class T>
	T getNumericValue() {
		return value_.getValueUnsafe<T>();
	}


private:
    // if it is negative term
    bool negative_;

	// Numeric Values
	Value value_;
	// If it is interval the interval from to
	IntervalTerm interval_{};
	// Type of the term Constant/Variable/etc.
	TermType type_{TermType::CONSTANT};
	// True if is anonymous variable
	bool anonymous_{false};
	// ArithTerm is an arithmetic with the list of a terms
	// and the relative operator(+, -, *, /,\)
	term_vector_t terms_{};
	op_vector_t operators_{};

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



} // bumblebee
