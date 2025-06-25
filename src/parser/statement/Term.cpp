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
#include "bumblebee/parser/statement/Term.h"

#include "bumblebee/common/Hash.h"


namespace bumblebee {
Term::Term():negative_(false) {}

Term::Term(Term&& term)
    : negative_(term.negative_),
      stringValue_(std::move(term.stringValue_)),
      interval_(std::move(term.interval_)),
      ctype_(term.ctype_),
      type_(term.type_),
      isAnonymous_(term.isAnonymous_),
      terms_(std::move(term.terms_)),
      operators_(std::move(term.operators_)) {

    // Move the value union
    switch (term.ctype_) {
        case TINYINT:
            value_.tinyint = term.value_.tinyint;
            break;
        case SMALLINT:
            value_.smallint = term.value_.smallint;
            break;
        case INTEGER:
            value_.integer = term.value_.integer;
            break;
        case BIGINT:
            value_.bigint = term.value_.bigint;
            break;
        case UTINYINT:
            value_.utinyint = term.value_.utinyint;
            break;
        case USMALLINT:
            value_.usmallint = term.value_.usmallint;
            break;
        case UINTEGER:
            value_.uinteger = term.value_.uinteger;
            break;
        case UBIGINT:
            value_.ubigint = term.value_.ubigint;
            break;
        case FLOAT:
            value_.float_ = term.value_.float_;
            break;
        case DOUBLE:
            value_.double_ = term.value_.double_;
            break;
        case STRING:
            stringValue_ = std::move(term.stringValue_);
            break;
    }
}

Term::Term(bool negative): negative_(negative) {}

Term::Term(int8_t c)
    : ctype_(ConstantType::TINYINT), negative_(false), value_({.tinyint = c}), type_(TermType::CONSTANT) {}

Term::Term(int16_t c)
    : ctype_(ConstantType::SMALLINT), negative_(false), value_({.smallint = c}), type_(TermType::CONSTANT) {}

Term::Term(int32_t c)
    : ctype_(ConstantType::INTEGER), negative_(false), value_({.integer = c}), type_(TermType::CONSTANT) {}

Term::Term(int64_t c)
    : ctype_(ConstantType::BIGINT), negative_(false), value_({.bigint = c}), type_(TermType::CONSTANT) {}

Term::Term(uint8_t c)
    : ctype_(ConstantType::UTINYINT), negative_(false), value_({.utinyint = c}), type_(TermType::CONSTANT) {}

Term::Term(uint16_t c)
    : ctype_(ConstantType::USMALLINT), negative_(false), value_({.usmallint = c}), type_(TermType::CONSTANT) {}

Term::Term(uint32_t c)
    : ctype_(ConstantType::UINTEGER), negative_(false), value_({.uinteger = c}), type_(TermType::CONSTANT) {}

Term::Term(uint64_t c)
    : ctype_(ConstantType::UBIGINT), negative_(false), value_({.ubigint = c}), type_(TermType::CONSTANT) {}

Term::Term(float c)
    : ctype_(ConstantType::FLOAT), negative_(false), value_({.float_ = c}), type_(TermType::CONSTANT) {}

Term::Term(double c)
    : ctype_(ConstantType::DOUBLE), negative_(false), value_({.double_ = c}), type_(TermType::CONSTANT) {}

Term::Term(StringT c)
    : stringValue_(std::move(c)), ctype_(ConstantType::STRING), negative_(false), type_(TermType::CONSTANT) {}

Term::Term(StringT c, bool isVariable)
    : stringValue_(std::move(c)), ctype_(ConstantType::STRING), negative_(false) {
    isAnonymous_ = stringValue_ == anonymous_variable && isVariable;
    type_ = isVariable ? TermType::VARIABLE : TermType::CONSTANT;
}

Term::Term(IntervalTerm interval_) : negative_(false), interval_(std::move(interval_)), type_(RANGE) {}

Term::Term(Term &&t, Operator op): negative_(false), type_(TermType::ARITH)  {
    terms_.push_back(std::move(t));
    operators_.push_back(op);
}

bool Term::isNegative() {return negative_;}

void Term::setNegative(bool n) {
    negative_ = n;
}

void Term::getVariables(set_term_variable &vars) {
    if (type_ == TermType::CONSTANT || type_ == TermType::RANGE || (type_ == TermType::VARIABLE && isAnonymous())) return;
    if (type_ == TermType::VARIABLE) {
        vars.insert(stringValue_);
        return;
    }
    // ARITH term
    for (auto &t:terms_)
        t.getVariables(vars);
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
    switch (ctype_) {
        case ConstantType::TINYINT:
            return Hash<uint8_t>(value_.tinyint);
        case ConstantType::SMALLINT:
            return Hash<uint16_t>(value_.smallint);
        case ConstantType::INTEGER:
            return Hash<uint32_t>(value_.integer);
        case ConstantType::BIGINT:
            return Hash<uint64_t>(value_.bigint);
        case ConstantType::UTINYINT:
            return Hash<uint8_t>(value_.utinyint);
        case ConstantType::USMALLINT:
            return Hash<uint16_t>(value_.usmallint);
        case ConstantType::UINTEGER:
            return Hash<uint32_t>(value_.uinteger);
        case ConstantType::UBIGINT:
            return Hash<uint64_t>(value_.ubigint);
        case ConstantType::FLOAT:
            return Hash<float>(value_.float_);
        case ConstantType::DOUBLE:
            return Hash<double>(value_.double_);
        case ConstantType::STRING:
            return Hash<StringT>(stringValue_);
        default: Hash(stringValue_);
    }
}

TermType Term::getType() {
    return type_;
}

void Term::setType(TermType type) {
    type_ = type;
}

bool Term::isGround() {
    if (type_ == TermType::CONSTANT) return true;
    return false;
}


std::string Term::toString() const {
    if (type_ == TermType::RANGE) {
        return std::to_string(interval_.from)+".."+std::to_string(interval_.to);
    }

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

bool Term::isAnonymous() {
    return isAnonymous_;
}

void Term::addTerm(Term&& term, Operator op) {
    terms_.push_back(std::move(term));
    operators_.push_back(op);
}

// -------------------- Creation template ------------------
template <>
Term Term::createConstantTerm(int8_t c) {
    return Term(c);
}

template <>
Term Term::createConstantTerm(int16_t c) {
    return Term(c);
}

template <>
Term Term::createConstantTerm(int32_t c) {
    return Term(c);
}

template <>
Term Term::createConstantTerm(int64_t c) {
    return Term(c);
}

template <>
Term Term::createConstantTerm(uint8_t c) {
    return Term(c);
}

template <>
Term Term::createConstantTerm(uint16_t c) {
    return Term(c);
}

template <>
Term Term::createConstantTerm(uint32_t c) {
    return Term(c);
}

template <>
Term Term::createConstantTerm(uint64_t c) {
    return Term(c);
}

template <>
Term Term::createConstantTerm(float c) {
    return Term(c);
}

template <>
Term Term::createConstantTerm(double c) {
    return Term(c);
}

template <>
Term Term::createConstantTerm(std::string c) {
    return Term(c);
}

template <>
Term Term::createConstantTerm(IntervalTerm interval_) {
    return Term(interval_);
}

template<>
Term Term::createConstantTerm<bool>(bool value) {
    return Term(value);
}


} // bumblebee