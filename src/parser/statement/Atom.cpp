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
                                   binop_(other.binop_){
    calculateIsGround();
}

Atom::Atom(terms_vector_t &&terms, Binop binop) : terms_(std::move(terms)), binop_(binop), type_(AtomType::BUILTIN) {
    calculateIsGround();
}

terms_vector_t& Atom::getTerms() {
    return terms_;
}

Predicate* Atom::getPredicate() {
    return predicate_;
}

void Atom::getPredicates(predicates_ptr_set_t &predicates) {
    if (type_ == AtomType::CLASSICAL) {
        predicates.insert(predicate_);
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

std::vector<ConstantType> Atom::getTermsCType() {
    std::vector<ConstantType> types;
    types.reserve(terms_.size());
    for (auto& term : terms_) {
        types.push_back(term.getConstantType());
    }
    return types;
}

void Atom::replaceVariable(const string &var,const string &newVar) {
    for (auto& term : terms_) {
        term.replaceVariable(var, newVar);
    }
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
    for (auto& term : terms_) {
        term.getVariables(variables);
    }
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
    ErrorHandler::errorNotImplemented("Atom type not implemented");
    return "";
}

void Atom::calculateIsGround() {
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


}
