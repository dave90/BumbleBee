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

using atoms_vector_t = std::vector<Atom>;

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

    bool isFact();
    bool isGround();

    void getPredicatesInHead(predicates_ptr_set_t &predicates);
    void getPredicatesInBody(predicates_ptr_set_t &predicates);

    atoms_vector_t& getBody();
    atoms_vector_t& getHead();
    void addAtomInHead(Atom &&atom);
    void addAtomInBody(Atom &&atom);
    std::string toString();
    bool isAStrongConstraint();
    void getVariables(set_term_variable_t &variables);
    void getVariablesInHead(set_term_variable_t &variables);
    void getVariablesInBody(set_term_variable_t &variables);
    predicates_ptr_set_t getPredicates();
    void replaceVariable(const string& var,const string& newVar);
    void setBody(atoms_vector_t &body);
    bool containsAggregate() const;

private:

    atoms_vector_t body_;
    atoms_vector_t head_;
};

using rules_vector_t = std::vector<Rule>;

// Bucket of rules divided based on the type
struct RulesBucket {
    rules_vector_t exit_;
    rules_vector_t recursive_;
    rules_vector_t constraints_;
};

using rules_bucket_vector_t = std::vector<RulesBucket>;

} // bumblebee
