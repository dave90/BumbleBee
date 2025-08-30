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

#include "bumblebee/parser/statement/Atom.h"

#include "bumblebee/common/Hash.h"
#include "bumblebee/common/Log.h"

namespace bumblebee {

Atom::Atom(Predicate* predicate, terms_vector_t &&terms, AtomType type): terms_(std::move(terms)) , negative_(false), predicate_(predicate), type_(type) {
    calculateIsGround();
}

Atom::Atom(AtomType type, bool negative): type_(type), negative_(negative), ground_(true) {
}

Atom::Atom(Atom &&other) noexcept: terms_(std::move(other.terms_)),
                                   predicate_(other.predicate_),
                                   type_(other.type_),
                                   negative_(other.negative_),
                                   binop_(other.binop_),
                                   secondBinop_(other.secondBinop_),
                                   aggregate_(other.aggregate_),
                                   aggAtoms_(std::move(other.aggAtoms_)),
                                   aggTerms_(std::move(other.aggTerms_)){
    calculateIsGround();
}

Atom::Atom(terms_vector_t &&terms, Binop binop) : terms_(std::move(terms)), binop_(binop), type_(AtomType::BUILTIN) {
    calculateIsGround();
}

Atom::Atom(AggregateFunctionType aggFunction, Binop firstBinop, Binop secondBinop, Term &lowerGuard, Term &upperGuard,
    terms_vector_t &&aggTerms, vector<Atom> &&aggAtoms): aggregate_(aggFunction),
    binop_(firstBinop), secondBinop_(secondBinop),aggTerms_(std::move(aggTerms)),
    aggAtoms_(std::move(aggAtoms)), predicate_(nullptr), type_(AGGREGATE){
    terms_.resize(2);
    terms_[0] = std::move(lowerGuard);
    terms_[1] = std::move(upperGuard);
}

terms_vector_t& Atom::getTerms() {
    return terms_;
}

terms_vector_t & Atom::getAggTerms() {
    return aggTerms_;
}

vector<Atom> & Atom::getAggsAtoms() {
    return aggAtoms_;
}

const terms_vector_t& Atom::getTerms() const {
    return terms_;
}
Predicate* Atom::getPredicate() {
    return predicate_;
}

void Atom::getPredicates(predicates_ptr_set_t &predicates) {
    if (type_ == AtomType::CLASSICAL) {
        predicates.insert(predicate_);
    }else if (type_ == AGGREGATE) {
        for (auto& atom : aggAtoms_)
            atom.getPredicates(predicates);
    }
}


AtomType Atom::getType() const {
    return type_;
}

void Atom::setType(AtomType type) {
    type_ = type;
}

bool Atom::isNegative() const {
    return negative_;
}

void Atom::setNegative(bool negative) {
    negative_ = negative;
}

Binop Atom::getBinop() const {
    return binop_;
}

void Atom::setBinop(Binop binop) {
    binop_ = binop;
}

Binop Atom::getSecondBinop() const {
    return secondBinop_;
}

void Atom::setSecondBinop(Binop binop) {
    secondBinop_ = binop;
}

bool Atom::containsAnonymous() const {
    for (auto& term : terms_)
        if (term.containsAnonymous())
            return true;
    return false;
}

bool Atom::containsRange() const {
    for (auto& term : terms_)
        if (term.getType() == TermType::RANGE)
            return true;
    return false;
}

bool Atom::containsArith() const {
    for (auto& term : terms_)
        if (term.getType() == TermType::ARITH)
            return true;
    return false;
}

bool Atom::containsConstant() const {
    for (auto& term : terms_)
        if (term.getType() == TermType::CONSTANT)
            return true;
    return false;
}

bool Atom::isConstantAssignment() {
    if (type_ != BUILTIN) return false;
    if (binop_ != EQUAL && binop_ != ASSIGNMENT) return false;
    auto lType = terms_[0].getType();
    auto rType = terms_[1].getType();

    return (lType == CONSTANT && rType == VARIABLE) ||
           (lType == VARIABLE && rType == CONSTANT);

}

bool Atom::isAggregateAssignment() {
    if (getType() != AGGREGATE) return false;
    if (binop_ == ASSIGNMENT || secondBinop_ == ASSIGNMENT) return true;
    return false;
}

vector<ConstantType> Atom::getTermsCType() {
    vector<ConstantType> types;
    types.reserve(terms_.size());
    for (auto& term : terms_) {
        if (term.getType() == TermType::NONE_TERM) continue;
        types.push_back(term.getConstantType());
    }
    return types;
}

void Atom::replaceVariable(const string &var,const string &newVar) {
    for (auto& term : terms_) {
        term.replaceVariable(var, newVar);
    }
}

string Atom::getAggregateFunctionName() {
    BB_ASSERT(getType() == AGGREGATE);
    return Atom::getAggFunction(aggregate_);
}

Atom & Atom::operator=(Atom &&other) noexcept {
    if (this == &other)
        return *this;
    terms_ = std::move(other.terms_);
    predicate_ = std::move(other.predicate_);
    type_ = other.type_;
    negative_ = other.negative_;
    binop_ = other.binop_;
    ground_ = other.ground_;
    aggregate_ = other.aggregate_;
    secondBinop_ = other.secondBinop_;
    aggAtoms_ = std::move(other.aggAtoms_);
    aggTerms_ = std::move(other.aggTerms_);
    return *this;
}



bool operator==(const Atom &lhs, const Atom &rhs) {
    return  lhs.terms_ == rhs.terms_
           && lhs.predicate_ == rhs.predicate_
           && lhs.type_ == rhs.type_
           && lhs.negative_ == rhs.negative_
           && lhs.binop_ == rhs.binop_;
}

bool operator!=(const Atom &lhs, const Atom &rhs) {
    return !(lhs == rhs);
}

hash_t Atom::hash() {
    hash_t hash = predicate_->hash();
    for (auto& term : terms_) {
        CombineHash(hash, term.hash());
    }
    return hash;
}

void Atom::getVariables(set_term_variable_t &variables) {
    if (getType() == AGGREGATE) {
        if (getBinop() != NONE_OP)
            terms_[0].getVariables(variables);
        if (getSecondBinop() != NONE_OP)
            terms_[1].getVariables(variables);
        return;
    }
    for (auto& term : terms_) {
        term.getVariables(variables);
    }
}

void Atom::getAggAtomsVariables(set_term_variable_t &variables) {
    for (auto& atom: aggAtoms_) {
        atom.getVariables(variables);
    }
}

void Atom::getAggSharedVariables(const set_term_variable_t &globalVariables, set_term_variable_t &sharedVariables) {
    set_term_variable_t internals;
    getAggAtomsVariables(internals);
    Term::intersetVariables(internals, globalVariables, sharedVariables);
}

bool Atom::isGround() {
    return ground_;
}

Term & Atom::operator[](unsigned i) {
    return terms_[i];
}

std::string Atom::toString() const {
    if (type_ == AtomType::CLASSICAL) {
        std::string s = negative_ ? "not " : "";
        s += predicate_->toString() + "(";
        if (!terms_.empty()) s += terms_[0].toString();
        for (unsigned i = 1; i < terms_.size(); ++i) {
            s += ","+terms_[i].toString();
        }
        return s+")";
    }
    if (type_ == AtomType::BUILTIN) {
        return terms_[0].toString() + " "+ getBinopStr(binop_) + " " + terms_[1].toString();
    }
    if (type_ == AGGREGATE) {
        string s ="";
        if (binop_ != NONE_OP)
            s += terms_[0].toString() + " " + getBinopStr(binop_) ;

        s += getAggFunction(aggregate_) + "{";
        for (auto& term : aggTerms_) {
            s += term.toString()+",";
        }
        s.pop_back(); // remove last comma
        s += ":";
        for (auto& atom : aggAtoms_) {
            s += atom.toString()+",";
        }
        s.pop_back(); // remove last comma
        s += "}";
        if (secondBinop_ != NONE_OP)
            s += getBinopStr(secondBinop_) + terms_[1].toString() ;

        return s;
    }
    ErrorHandler::errorNotImplemented("Atom type not implemented");
    return "";
}

void Atom::calculateIsGround() {
    if (type_ == AGGREGATE) {
        ground_ = false;
        return;
    }
    for (auto &term : terms_) {
        if (!term.isGround()) {
            ground_ = false;
            return;
        }
    }
    ground_ = true;
}

Atom Atom::createClassicalAtom(Predicate*p, terms_vector_t &&t) {
    return Atom(p, std::move(t), AtomType::CLASSICAL);
}

Atom Atom::createBuiltinAtom(terms_vector_t &&t, Binop binop) {
    return Atom(std::move(t), binop);
}

Atom Atom::createAggregateAtom(AggregateFunctionType aggFunction, Binop firstBinop, Binop secondBinop, Term &lowerGuard,
    Term &upperGuard, terms_vector_t &&aggTerms, vector<Atom> &&aggAtoms) {
    return Atom(aggFunction, firstBinop, secondBinop, lowerGuard, upperGuard, std::move(aggTerms), std::move(aggAtoms));
}

string Atom::getAggFunction(AggregateFunctionType agg) {
    switch (agg) {
        case NONE:
            return "";
        case MIN:
            return "#min";
        case MAX:
            return "#max";
        case AVG:
            return "#avg";
        case SUM:
            return "#sum";
        case COUNT:
            return "#count";
    }
}


AggregateFunctionType Atom::getAggFunction(const char* aggFunction) {
    if (aggFunction == nullptr) return NONE;

    std::string s(aggFunction);
    if (s == "min")   return MIN;
    if (s == "max")   return MAX;
    if (s == "avg")   return AVG;
    if (s == "sum")   return SUM;
    if (s == "count") return COUNT;

    return NONE; // default if no match
}

}
