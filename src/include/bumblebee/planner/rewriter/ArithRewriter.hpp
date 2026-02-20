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
#include "Rewriter.hpp"
#include "bumblebee/ClientContext.hpp"

namespace bumblebee{

// Rewrites the Arith and Builtin atoms
class ArithRewriter : public Rewriter{
public:

    ~ArithRewriter() override;
    void rewrite(Rule &rule) override;

private:
    // return true if contains variables shared in terms
    bool containsSharedVariables(Atom& atom, string& sharedVar);
    // remove the duplicates variables sharedVar creating a new binop
    atoms_vector_t removeSharedVariables(Atom& atom,string& sharedVar);
    // rewrite the aggregate as assignment
    atoms_vector_t rewriteAggregate(Atom& atom);
    // extract constant values from arith term
    Atom extractConstantBuiltinArith(Term& left, Term& right);
    // extract nested ARITH sub-terms into separate builtins with fresh variables
    void extractNestedArith(vector<Atom>& builtins);

    // extract the first arith term found in the atom and replace with
    // a fresh variable and return the builtin created with the arith term
    Atom extractArith(Atom& atom);

    idx_t counter_{0};
};


}
