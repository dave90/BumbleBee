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
#include "bumblebee/parser/statement/Rule.hpp"

namespace bumblebee {
Rule::Rule(Rule &&other) noexcept: body_(std::move(other.body_)),
                                   head_(std::move(other.head_)),
                                   limit_(other.limit_),
                                   modifiers_(std::move(other.modifiers_)){
}

Rule::Rule(Atom &head, vector<Atom> &body) {
    body_ = std::move(body);
    head_.push_back(std::move(head));
}

Rule::Rule(Atom &head, Atom &body) {
    head_.push_back(std::move(head));
    body_.push_back(std::move(body));
}

Rule::Rule(const Atom &head, vector<Atom> &body) {
    head_.push_back(head.clone());
    body_ = std::move(body);
}

Rule::Rule(const Atom &head, Atom &body) {
    head_.push_back(head.clone());
    body_.push_back(std::move(body));
}

Rule & Rule::operator=(Rule &&other) noexcept {
    if (this == &other)
        return *this;
    body_ = std::move(other.body_);
    head_ = std::move(other.head_);
    limit_ = other.limit_;
    modifiers_ = std::move(other.modifiers_);
    return *this;
}

bool Rule::isFact() {
    return body_.size() == 0 && head_.size() == 1 && head_[0].getType() == CLASSICAL && head_[0].isGround();
}

bool Rule::isGround() {
    for (auto& atom :head_)
        if (!atom.isGround())return false;
    for (auto& atom :body_)
        if (!atom.isGround())return false;
    return true;

}

void Rule::replaceVariable(const string &var, const string &newVar) {
    for (auto& atom:head_) atom.replaceVariable(var, newVar);
    for (auto& atom:body_) atom.replaceVariable(var, newVar);
}

void Rule::setBody(atoms_vector_t &body) {
    body_ = std::move(body);
}

bool Rule::containsAggregate() const {
    for (auto& atom: body_)
        if (atom.getType() == AGGREGATE) return true;
    return false;
}

Rule Rule::clone() {
    Rule clonedRule;
    for (auto& atom: body_) clonedRule.addAtomInBody(atom.clone());
    for (auto& atom: head_) clonedRule.addAtomInHead(atom.clone());
    clonedRule.limit_ = limit_;
    clonedRule.modifiers_ = modifiers_;
    return clonedRule;
}

idx_t & Rule::getLimit() {
    return limit_;
}

void Rule::setLimit(idx_t limit) {
    limit_ = limit;
}

vector<ColModifier> & Rule::getModifiers() {
    return modifiers_;
}

void Rule::setModifiers(vector<ColModifier> &modifiers) {
    modifiers_ = std::move(modifiers);
}

void Rule::getPredicatesInHead(predicates_ptr_set_t &predicates) {
    for (auto& atom :head_)
        atom.getPredicates(predicates);
}

void Rule::getPredicatesInBody(predicates_ptr_set_t &predicates) {
    for (auto& atom :body_)
        atom.getPredicates(predicates);
}
predicates_ptr_set_t Rule::getPredicates() {
    predicates_ptr_set_t predicates;
    getPredicatesInHead(predicates);
    for (auto& atom :body_)
        atom.getPredicates(predicates);
    return predicates;
}

atoms_vector_t& Rule::getBody()  {
    return body_;
}

atoms_vector_t& Rule::getHead() {
    return head_;
}

void Rule::addAtomInHead(Atom &&atom) {
    head_.push_back(std::move(atom));
}

void Rule::addAtomInBody(Atom &&atom) {
    body_.push_back(std::move(atom));
}

std::string Rule::toString() {
    std::string s;
    if (limit_ > 0)
        s+="#limit "+std::to_string(limit_)+"\n";
    if (modifiers_.size() > 0) {
        s +="#order_by ";
        auto& head = head_[0];
        for (idx_t i = 0; i < modifiers_.size(); i++) {
            if (i > 0) s+=", ";
            s += head.getTerms()[modifiers_[i].col_].toString()+" "+modifiers_[i].modifier_.toString();
        }
        s+="\n";
    }

    if (!head_.empty()) s += head_.front().toString();
    for (unsigned int i = 1; i < head_.size(); i++) {
        s += SEPARATOR_HAED + head_[i].toString();
    }
    s += std::string(" ")+ SEPARATOR+" ";
    if (!body_.empty()) s += body_.front().toString();
    for (unsigned int i = 1; i < body_.size(); i++) {
        s += SEPARATOR_BODY;
        s += " " + body_[i].toString();
    }
    return s + ".";
}

bool Rule::isAStrongConstraint() {
    return head_.empty();
}

void Rule::getVariables(set_term_variable_t &variables) {
    getVariablesInBody(variables);
    getVariablesInBody(variables);
}

void Rule::getVariablesInHead(set_term_variable_t &variables) {
    for (auto& atom : head_) {
        atom.getVariables(variables);
    }
}

void Rule::getVariablesInBody(set_term_variable_t &variables) {
    for (auto& atom : body_) {
        atom.getVariables(variables);
    }
}

} // bumblebee