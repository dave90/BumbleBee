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
#include "bumblebee/planner/rewriter/ArithRewriter.h"

#include "bumblebee/common/types/Assert.h"

namespace bumblebee{

void ArithRewriter::rewrite(Rule &rule) {
    std::vector<Atom> newAtoms ;
    // extract arith term or constant
    for (auto& atom: rule.getHead()) {
        while (atom.containsArith() || atom.containsConstant()) {
            auto builtinAtom = extractArith(atom);
            newAtoms.push_back(std::move(builtinAtom));
        }
    }
    for (auto& atom: rule.getBody()) {
        while (atom.containsArith() || atom.containsConstant()) {
            auto builtinAtom = extractArith(atom);
            newAtoms.push_back(std::move(builtinAtom));
        }
    }
    for (auto& atom: newAtoms)
        rule.addAtomInBody(std::move(atom));
}

Atom ArithRewriter::extractArith(Atom &atom) {
    auto& terms = atom.getTerms();
    idx_t arithTermIdx = terms.size();
    for (idx_t i = 0; i < terms.size(); i++) {
        auto & term = terms[i];
        if (term.getType() == TermType::ARITH || term.getType() == TermType::CONSTANT) {
            arithTermIdx = i;
            break;
        }
    }
    BB_ASSERT(arithTermIdx < terms.size());

    Atom newAtom(BUILTIN, false);
    newAtom.setBinop(EQUAL);

    terms_vector_t binopTerms;
    auto newVarName1 = NEW_VARS_PREFIX + std::to_string(counter_);
    auto newVarName2 = newVarName1;

    binopTerms.emplace_back(NEW_VARS_PREFIX + std::to_string(counter_), true);
    binopTerms.push_back(std::move(terms[arithTermIdx]));

    Term newVariable(NEW_VARS_PREFIX + std::to_string(counter_), true);
    terms[arithTermIdx] = std::move(newVariable);
    counter_++;
    return Atom::createBuiltinAtom(std::move(binopTerms), EQUAL);
}

ArithRewriter::~ArithRewriter() {}
}
