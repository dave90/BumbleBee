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
#include <vector>

#include "Atom.h"

namespace bumblebee {

using atoms_vector = std::vector<Atom>;

class Rule {
public:
    static constexpr char SEPARATOR_BODY = ',';
    static constexpr char SEPARATOR_HAED = '|';
    static constexpr const char* SEPARATOR = ":-";

    Rule() = default;
    Rule(const Rule &other) = delete;
    Rule(Rule &&other) noexcept;
    ~Rule() = default;

    Rule & operator=(const Rule &other) = delete;
    Rule & operator=(Rule &&other) noexcept;

    bool isGround();
    atoms_vector& getBody();
    atoms_vector& getHead();
    void addAtomInHead(Atom &&atom);
    void addAtomInBody(Atom &&atom);
    std::string toString();
    bool isAStrongConstraint();
    void getVariables(set_term_variable &variables);
    void getVariablesInHead(set_term_variable &variables);
    void getVariablesInBody(set_term_variable &variables);
    predicates_ptr_set getPredicates();

private:

    atoms_vector body_;
    atoms_vector head_;
    bool fact_;
};

} // bumblebee
