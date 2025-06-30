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
#include "bumblebee/parser/statement/Rule.h"

namespace bumblebee {
Rule::Rule(Rule &&other) noexcept: body_(std::move(other.body_)),
                                   head_(std::move(other.head_)) {
}

Rule & Rule::operator=(Rule &&other) noexcept {
    if (this == &other)
        return *this;
    body_ = std::move(other.body_);
    head_ = std::move(other.head_);
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

predicates_ptr_set Rule::getPredicates() {
    predicates_ptr_set predicates;
    for (auto& atom :head_)
        atom.getPredicates(predicates);
    for (auto& atom :body_)
        atom.getPredicates(predicates);
    return predicates;
}

atoms_vector& Rule::getBody()  {
    return body_;
}

atoms_vector& Rule::getHead() {
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
    if (!head_.empty()) s += head_.front().toString();
    for (unsigned int i = 1; i < head_.size(); i++) {
        s += SEPARATOR_HAED + head_[i].toString();
    }
    s += std::string(" ")+ SEPARATOR+" ";
    if (!body_.empty()) s += body_.front().toString();
    for (unsigned int i = 1; i < body_.size(); i++) {
        s += SEPARATOR_BODY + body_[i].toString();
    }
    return s + ".";
}

bool Rule::isAStrongConstraint() {
    return head_.empty();
}

void Rule::getVariables(set_term_variable &variables) {
    getVariablesInBody(variables);
    getVariablesInBody(variables);
}

void Rule::getVariablesInHead(set_term_variable &variables) {
    for (auto& atom : head_) {
        atom.getVariables(variables);
    }
}

void Rule::getVariablesInBody(set_term_variable &variables) {
    for (auto& atom : body_) {
        atom.getVariables(variables);
    }
}

} // bumblebee