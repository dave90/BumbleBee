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
#include "bumblebee/planner/rewriter/VariablesRewriter.h"

#include "bumblebee/common/types/Assert.h"

namespace bumblebee{


void VariablesRewriter::pushAnonymous(Rule &rule) {
    set_term_variable_t usedVariables;
    rule.getVariablesInHead(usedVariables);
    // collect all the variables for each atom in the body
    vector<set_term_variable_t> variablesInBody;
    set_term_variable_t allVariables;
    variablesInBody.resize(rule.getBody().size());
    for (idx_t i = 0; i < rule.getBody().size(); ++i) {
        rule.getBody()[i].getVariables(variablesInBody[i]);
        allVariables.insert(variablesInBody[i].begin(), variablesInBody[i].end());
    }

    for (idx_t i = 0; i < variablesInBody.size(); ++i) {
        // count the vars present in the atom, if multiple terms share the same variable will increment the counter
        std::unordered_map<string,idx_t> variableCounter;
        for (auto& term: rule.getBody()[i].getTerms()) {
            set_term_variable_t termVariables;
            term.getVariables(termVariables);
            for (auto& var : termVariables) {
                if (!variableCounter.contains(var))
                    variableCounter[var] = 0;
                ++variableCounter[var];
            }
        }
        // find the vars shared with other atoms, this vars are used varaibles
        for (idx_t j = i +1; j < variablesInBody.size(); ++j)
            Term::intersetVariables(variableCounter, variablesInBody[j], usedVariables);

        for (auto& [var, count]: variableCounter) {
            if (count > 1) {
                // count > 1 variables is shared in the same atoms, i.e a(X,X)
                usedVariables.insert(var);
                continue;
            }
            if (usedVariables.contains(var))continue;
        }
    }

    for (auto& var : allVariables) {
        if (usedVariables.contains(var))continue;
        // unused variable, replace it
        rule.replaceVariable(var, Term::anonymous_variable);
    }
}

VariablesRewriter::VariablesRewriter(const ClientContext &context): context_(context) {
}

void VariablesRewriter::rewrite(Rule &rule) {
    // start with the variables in the head
    pushAnonymous(rule);
    // check no anonymous in head and binop and remove atoms all anonymous
    verifyAndPruneAtoms(rule);
}

void VariablesRewriter::verifyAndPruneAtoms(Rule &rule) {

    // check if a source atom is present
    bool sourceAtomPresent = false;
    for (auto& atom:rule.getBody()) {
        if (atom.getType() == CLASSICAL)
            sourceAtomPresent = true;
    }
    if (!sourceAtomPresent) {
        // no source atoms are present
        // add a fake classical atom as source just to have a source in the rule
        terms_vector_t terms;
        terms.emplace_back(Predicate::INTERNAL_SOURCE_ONE_ROW.c_str(), true); // add variable that is not possible to exist in the body
        auto atom = Atom::createClassicalAtom(context_.defaultSchema_.getFASOPredicate(), std::move(terms));
        rule.addAtomInBody(std::move(atom));
        return;
    }

    // check no anonymous in head
    // check if all atoms has anonymous if yes prune it
    // check if it is a binop atom that not contains anonymous
    for (auto& atom : rule.getHead())
        if (atom.containsAnonymous())
            ErrorHandler::errorParsing("Error while rewriting rule with VariablesRewriter. Anonymous variable in head atom. Please report the issue.");
    for (idx_t i = 0; i < rule.getBody().size(); ++i) {
        auto& atom = rule.getBody()[i];
        if (!atom.containsAnonymous())continue;
        if (atom.getType() == AtomType::BUILTIN)
            ErrorHandler::errorParsing("Error while rewriting rule with VariablesRewriter. Anonymous in BINOP. Please report the issue.");
        set_term_variable_t vars;
        atom.getVariables(vars);
        if (vars.size() == 0) {
            // atom contains only anonymouse variable, remove it
            rule.getBody().erase(rule.getBody().begin() + i);
            --i; // decrement as we are removing one atom
        }
    }
}
}
