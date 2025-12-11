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
#include "bumblebee/parser/statement/Term.hpp"

#include "CLI11.hpp"
#include "bumblebee/common/Hash.hpp"
#include "bumblebee/common/Log.hpp"
#include "bumblebee/common/types/Assert.hpp"


namespace bumblebee {
Term::Term():negative_(false) {}

Term::Term(Term&& term)
    : negative_(term.negative_),
      interval_(term.interval_),
      value_(std::move(term.value_)),
      type_(term.type_),
      anonymous_(term.anonymous_),
      terms_(std::move(term.terms_)),
      operators_(std::move(term.operators_)) {}

Term::Term(bool negative): negative_(negative) {}

Term::Term(int8_t c)
    :  negative_(false), value_( c), type_(CONSTANT) {}

Term::Term(int16_t c)
    :  negative_(false), value_( c), type_(CONSTANT) {}

Term::Term(int32_t c)
    :  negative_(false), value_( c), type_(CONSTANT) {}

Term::Term(int64_t c)
    :  negative_(false), value_( c), type_(CONSTANT) {}

Term::Term(uint8_t c)
    :  negative_(false), value_( c), type_(CONSTANT) {}

Term::Term(uint16_t c)
    :  negative_(false), value_( c), type_(CONSTANT) {}

Term::Term(uint32_t c)
    :  negative_(false), value_( c), type_(CONSTANT) {}

Term::Term(uint64_t c)
    :  negative_(false), value_( c), type_(CONSTANT) {}

Term::Term(float c)
    :  negative_(false), value_( c), type_(CONSTANT) {}

Term::Term(double c)
    :  negative_(false), value_( c), type_(CONSTANT) {}

Term::Term(string&& c)
    : negative_(false), value_(std::move(c)), type_(CONSTANT) {}

Term::Term(char* c)
        : negative_(false), value_(c), type_(CONSTANT) {}

Term::Term(string&& c, bool isVariable)
    : value_(std::move(c)), negative_(false) {
    anonymous_ = value_.stringValue_ == anonymous_variable && isVariable;
    type_ = isVariable ? TermType::VARIABLE : TermType::CONSTANT;
}

Term::Term(IntervalTerm interval_) : negative_(false), interval_(interval_), type_(RANGE) {}

Term::Term(Term &&t1,Term &&t2, Operator op): negative_(false), type_(ARITH)  {
    terms_.push_back(std::move(t1));
    terms_.push_back(std::move(t2));
    operators_.push_back(op);
}

Term::Term(Value &v): value_(std::move(v)), type_(CONSTANT), negative_(false) {
}

Term & Term::operator=(const Term &other) {
    negative_ = other.negative_;
    interval_ = other.interval_;
    type_ = other.type_;
    anonymous_ = other.anonymous_;
    terms_ = other.terms_;
    operators_ = other.operators_ ;
    value_ = other.value_.cast(other.value_.ctype_);
    return *this;
}

bool Term::isNegative() {return negative_;}

void Term::setNegative(bool n) {
    negative_ = n;
}

void Term::getVariables(set_term_variable_t &vars) {
    if (type_ == TermType::CONSTANT || type_ == TermType::RANGE || (type_ == TermType::VARIABLE && isAnonymous())) return;
    if (type_ == TermType::VARIABLE) {
        vars.insert(value_.stringValue_);
        return;
    }
    // ARITH term
    for (auto &t:terms_)
        t.getVariables(vars);
}

void Term::getVariablesList(vector<string> &variables) {
    if (type_ == TermType::CONSTANT || type_ == TermType::RANGE || (type_ == TermType::VARIABLE && isAnonymous())) return;
    if (type_ == TermType::VARIABLE) {
        variables.push_back(value_.stringValue_);
        return;
    }
    // ARITH term
    for (auto &t:terms_)
        t.getVariablesList(variables);
}


hash_t Term::hash() {
    if (type_ == TermType::RANGE) {
        return  CombineHash(Hash<uint64_t>(interval_.from), Hash<uint64_t>(interval_.to));
    }
    if (type_ == TermType::ARITH) {
        hash_t h = terms_[0].hash();
        for (unsigned int i = 1; i < terms_.size(); i++) {
            h = CombineHash(h, terms_[i].hash());
        }
        return h;
    }

    // Constant or Variable
    switch (value_.ctype_) {
        case ConstantType::TINYINT:
            return Hash<uint8_t>(value_.getValueUnsafe<uint8_t>());
        case ConstantType::SMALLINT:
            return Hash<uint16_t>(value_.getValueUnsafe<uint16_t>());
        case ConstantType::INTEGER:
            return Hash<uint32_t>(value_.getValueUnsafe<uint32_t>());
        case ConstantType::BIGINT:
            return Hash<uint64_t>(value_.getValueUnsafe<uint64_t>());
        case ConstantType::UTINYINT:
            return Hash<uint8_t>(value_.getValueUnsafe<uint8_t>());
        case ConstantType::USMALLINT:
            return Hash<uint16_t>(value_.getValueUnsafe<uint16_t>());
        case ConstantType::UINTEGER:
            return Hash<uint32_t>(value_.getValueUnsafe<uint32_t>());
        case ConstantType::UBIGINT:
            return Hash<uint64_t>(value_.getValueUnsafe<uint64_t>());
        case ConstantType::FLOAT:
            return Hash<float>(value_.getValueUnsafe<float>());
        case ConstantType::DOUBLE:
            return Hash<double>(value_.getValueUnsafe<double>());
        case ConstantType::STRING:
            return Hash<string>(value_.getValueUnsafe<string>());
        default: return Hash(value_.getValueUnsafe<string>());
    }

}

TermType Term::getType() const{
    return type_;
}

ConstantType Term::getConstantType() {
    if ( type_ == RANGE)
        return ConstantType::BIGINT;
    BB_ASSERT(type_ == CONSTANT);
    return value_.ctype_;
}
void Term::setType(TermType type) {
    type_ = type;
}

bool Term::isGround() {
    if (type_ == TermType::VARIABLE) return false;
    if (type_ == TermType::CONSTANT || type_ == TermType::RANGE) return true;
    if (type_ == TermType::ARITH) {
        for (auto &t:terms_)
            if (!t.isGround()) return false;
    }
    return true;
}


std::string Term::toString() const {
    if (type_ == TermType::RANGE) {
        return std::to_string(interval_.from)+".."+std::to_string(interval_.to);
    }
    if ( type_ == TermType::ARITH) {
        std::string s = "";
        for (unsigned int i = 0; i < terms_.size() - 1; i++) {
            s += terms_[i].toString() + getOperatorChar(operators_[i]);
        }
        return s + terms_.back().toString();
    }
    return value_.toString();
}

bool Term::isAnonymous() {
    return anonymous_;
}

void Term::addInArithTerm(Term&& term, Operator op) {
    terms_.push_back(std::move(term));
    operators_.push_back(op);
}

void Term::addInArithTermBegin(Term&& term, Operator op) {
    terms_.insert(terms_.begin(), std::move(term));
    operators_.insert(operators_.begin(), op);
}

void Term::addInArithTerm(Term&& term, char sop) {
    Operator op = getOperator(sop);
    addInArithTerm(std::move(term), op);
}

Term Term::createVariable(std::string &&value) {
    return Term(std::move(value), true);
}

Term Term::createVariable(const char *value) {
    string s = value;
    return createVariable(std::move(s));
}

bool Term::containsAnonymous() const {
    return anonymous_;
}
bool Term::containsConstant() const {
    for (auto& term : terms_)
        if (term.getType() == TermType::CONSTANT)
            return true;
    return false;
}

bool Term::containsOrIsConstant() const {
    if (type_ == TermType::CONSTANT) return true;
    return containsConstant();
}

void Term::replaceVariable(const string &var,const string &newVar) {
    if (type_ == CONSTANT || type_ == RANGE || type_ == NONE_TERM)return;
    if (type_ == VARIABLE) {
        if (value_.stringValue_ == var) {
            value_.stringValue_ = newVar;
            anonymous_ = newVar == anonymous_variable;
        }
        return;
    }
    if (type_ == ARITH) {
        for (auto& t:terms_)
            t.replaceVariable(var, newVar);
        return;
    }
    ErrorHandler::errorNotImplemented("Replace variable for term type not supported");
}

Operator Term::getOperator(char sop) {
    Operator op;
    switch (sop) {
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
    }
    ErrorHandler::errorNotImplemented("Invalid operator conversion from char");
    return PLUS;
}


void Term::setConstantNumericTerm(Term &term, long long num) {

    // Check for signed types
    if (num >= CHAR_MIN && num <= CHAR_MAX) {
        term.value_.value_.tinyint = static_cast<int8_t>(num);
        term.value_.ctype_ = ConstantType::TINYINT;
        return;
    }
    if (num >= SHRT_MIN && num <= SHRT_MAX) {
        term.value_.value_.smallint = static_cast<int16_t>(num);
        term.value_.ctype_ = ConstantType::SMALLINT;
        return ;
    }
    if (num >= INT_MIN && num <= INT_MAX) {
        term.value_.value_.integer = static_cast<int32_t>(num);
        term.value_.ctype_ = ConstantType::INTEGER;
        return ;
    }
    term.value_.value_.bigint = static_cast<int64_t>(num);
    term.value_.ctype_ = ConstantType::BIGINT;
}

void Term::setConstantNumericTerm(Term &term, unsigned long long value) {
    // Check for unsigned types and create the variable
    if (value <= UCHAR_MAX) {
        term.value_.value_.utinyint = static_cast<uint8_t>(value);
        term.value_.ctype_ = ConstantType::UTINYINT;
        return ;
    }
    if (value <= USHRT_MAX) {
        term.value_.value_.usmallint = static_cast<uint16_t>(value);
        term.value_.ctype_ = ConstantType::USMALLINT;
        return ;
    }
    if (value <= UINT_MAX) {
        term.value_.value_.uinteger = static_cast<uint32_t>(value);
        term.value_.ctype_ = ConstantType::UINTEGER;
        return ;
    }
    term.value_.value_.ubigint = static_cast<uint64_t>(value);
    term.value_.ctype_ = ConstantType::UBIGINT;
}

Term Term::createSmallestConstantNumericTerm(unsigned long long value) {
    if (value <= UCHAR_MAX) {
        return Term(static_cast<uint8_t>(value));;
    }
    if (value <= USHRT_MAX) {
        return Term(static_cast<uint16_t>(value));;
    }
    if (value <= UINT_MAX) {
        return Term(static_cast<uint32_t>(value));;
    }
    return Term(static_cast<uint64_t>(value));;
}

Term Term::createSmallestConstantNumericTerm(long long num) {
    // Check for signed types
    if (num >= CHAR_MIN && num <= CHAR_MAX) {
        return Term(static_cast<int8_t>(num));;
    }
    if (num >= SHRT_MIN && num <= SHRT_MAX) {
        return Term(static_cast<int16_t>(num));;
    }
    if (num >= INT_MIN && num <= INT_MAX) {
        return Term(static_cast<int32_t>(num));;
    }
    return Term(static_cast<int64_t>(num));;
}

Term Term::createRange(int from, int to) {
    IntervalTerm t{from, to};
    return Term(t);
}

Term Term::createArith(Term &&t1, Term &&t2, char sop) {
    Operator op = getOperator(sop);
    return Term(std::move(t1),std::move(t2) ,op);
}

Term Term::createArith(Term &&t1, Term &&t2, Operator op) {
    return Term(std::move(t1),std::move(t2) ,op);
}

bool operator==(const Term &lhs, const Term &rhs) {
    if (lhs.type_ != rhs.type_) return false;
    // constant and variable checks
    if (lhs.type_ == VARIABLE || lhs.type_ == CONSTANT) {
        return lhs.value_ == rhs.value_;
    }
    // range comparison
    if (lhs.type_ == RANGE) return lhs.interval_ == rhs.interval_;
    // arith comparison
    return lhs.terms_ == rhs.terms_
           && lhs.operators_ == rhs.operators_;
}

bool operator!=(const Term &lhs, const Term &rhs) {
    return !(lhs == rhs);
}



} // bumblebee