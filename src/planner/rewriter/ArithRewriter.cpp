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
    std::vector<Atom> builtins ;
    // extract arith term or constant
    for (auto& atom: rule.getHead()) {
        while (atom.containsArith() || atom.containsConstant()) {
            auto builtinAtom = extractArith(atom);
            builtins.push_back(std::move(builtinAtom));
        }
    }
    // extract shared variables
    for (auto& atom: rule.getHead()) {
        string var;
        while (containsSharedVariables(atom, var)) {
            auto builtinAtoms = removeSharedVariables(atom, var);
            for (auto& builtinAtom: builtinAtoms)
                builtins.push_back(std::move(builtinAtom));
        }
    }

    // separate the builtin with the classical atoms
    // and put the arith in the builtins vector
    std::vector<Atom> newBody;
    for (auto& atom: rule.getBody()) {
        if (atom.getType() == CLASSICAL)
            newBody.push_back(std::move(atom));
        else if (atom.getType() == BUILTIN)
            builtins.push_back(std::move(atom));
        else
            BB_ASSERT(false && "Atom type not supported");
    }
    rule.setBody(newBody);


    for (auto& atom: rule.getBody()) {
        while (atom.containsArith() || atom.containsConstant()) {
            auto builtinAtom = extractArith(atom);
            builtins.push_back(std::move(builtinAtom));
        }
    }
    for (auto& atom: rule.getBody()) {
        string var;
        while (containsSharedVariables(atom, var)) {
            auto builtinAtoms = removeSharedVariables(atom, var);
            for (auto& builtinAtom: builtinAtoms)
                builtins.push_back(std::move(builtinAtom));
        }
    }
    // finally extract the constant in the arith formula
    auto size = builtins.size(); // store the size because new atoms will be pushed
    for (idx_t i = 0; i < size; ++i) {
        auto& atom = builtins[i];
        if (atom.getType() != BUILTIN || atom.isConstantAssignment())continue;
        auto& left = atom.getTerms()[0];
        auto& right = atom.getTerms()[1];
        while (left.containsOrIsConstant() || right.containsOrIsConstant()) {
            auto builtinAtom = extractConstantBuiltinArith(atom);
            builtins.push_back(std::move(builtinAtom));
        }
    }
    for (auto& atom: builtins)
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

bool ArithRewriter::containsSharedVariables(Atom &atom, string& sharedVar) {
    set_term_variable_t sharedVariables;
    for (auto& term: atom.getTerms()) {
        BB_ASSERT(term.getType() == TermType::VARIABLE);
        if (term.isAnonymous())continue;
        auto& var = term.getVariable();
        if (sharedVariables.contains(var)) {
            sharedVar = var;
            return true;
        }
        sharedVariables.insert(var);
    }
    return false;
}

atoms_vector_t ArithRewriter::removeSharedVariables(Atom &atom, string& sharedVar) {
    atoms_vector_t atoms;
    idx_t counter = 0;
    for (idx_t i = 0; i < atom.getTerms().size(); i++) {
        auto& term = atom.getTerms()[i];
        BB_ASSERT(term.getType() == TermType::VARIABLE);
        if (term.getVariable() != sharedVar)continue;
        if (counter == 0) {
            // skip if it is the first time that we see the variable
            ++counter;
            continue;
        }
        // shared variable create an builtin atom and replace it
        auto newVarName1 = NEW_VARS_PREFIX + std::to_string(counter_++);
        auto newVarName2 = newVarName1;
        string sharedVarName = sharedVar;

        terms_vector_t binopTerms;
        binopTerms.emplace_back(std::move(sharedVarName), true);
        binopTerms.emplace_back(std::move(newVarName1), true);
        auto newBuiltin = Atom::createBuiltinAtom(std::move(binopTerms), EQUAL);
        atoms.push_back(std::move(newBuiltin));

        Term newVariable(std::move(newVarName2), true);

        atom.getTerms()[i] = std::move(newVariable);

    }
    BB_ASSERT(!atoms.empty());
    return atoms;
}

Atom ArithRewriter::extractConstantBuiltinArith(Atom &atom) {
    BB_ASSERT(atom.getType() == BUILTIN);
    BB_ASSERT(atom.getTerms().size() == 2);
    BB_ASSERT(!atom.isConstantAssignment());

    auto& left = atom.getTerms()[0];
    auto& right = atom.getTerms()[1];
    // we do not want to have builtin with 2 contant, must be evaluated directly in the optimizer
    BB_ASSERT(left.getType() != TermType::CONSTANT || right.getType() != TermType::CONSTANT );
    Term cterm;
    Term newVariable(NEW_VARS_PREFIX + std::to_string(counter_), true);

    if (left.getType() == TermType::CONSTANT) {
        cterm = std::move(left);
        left = std::move(newVariable);
    }
    else if (right.getType() == TermType::CONSTANT) {
        cterm = std::move(right);
        right = std::move(newVariable);
    }else {
        bool foundConstant = false;
        for (idx_t i = 0; i < left.getTerms().size(); i++) {
            if (left.getTerms()[i].getType() == TermType::CONSTANT) {
                cterm = std::move(left.getTerms()[i]);
                left.getTerms()[i] = std::move(newVariable);
                foundConstant = true;
                break;
            }
        }
        for (idx_t i = 0; i < right.getTerms().size() && !foundConstant; i++) {
            if (right.getTerms()[i].getType() == TermType::CONSTANT) {
                cterm = std::move(right.getTerms()[i]);
                right.getTerms()[i] = std::move(newVariable);
                break;
            }
        }
    }

    terms_vector_t binopTerms;
    binopTerms.emplace_back(NEW_VARS_PREFIX + std::to_string(counter_), true);
    binopTerms.push_back(std::move(cterm));
    counter_++;

    return Atom::createBuiltinAtom(std::move(binopTerms), ASSIGNMENT);
}

ArithRewriter::~ArithRewriter() {}
}
