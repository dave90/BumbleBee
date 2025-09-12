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
#include "bumblebee/planner/rewriter/VariablesRewriter.hpp"

#include "bumblebee/common/types/Assert.hpp"

namespace bumblebee{


void VariablesRewriter::pushAnonymous(Rule &rule) {
    set_term_variable_t usedVariables;
    rule.getVariablesInHead(usedVariables);
    set_term_variable_t allVariables;

    std::unordered_map<string,idx_t> variableCounter;
    for (idx_t i = 0; i < rule.getBody().size(); ++i) {
        rule.getBody()[i].getVariables(allVariables);
        // for loop of variables inside the term to catch the shared variables (i.e. a(X,X))
        for (auto& term: rule.getBody()[i].getTerms()) {
            set_term_variable_t termVariables;
            term.getVariables(termVariables);
            for (auto& var : termVariables) {
                if (!variableCounter.contains(var))
                    variableCounter[var] = 0;
                ++variableCounter[var];
            }
        }
    }

    // check the aggregate shared variables
    for (idx_t i = 0; i < rule.getBody().size(); ++i) {
        if (rule.getBody()[i].getType() != AGGREGATE)continue;
        set_term_variable_t sharedVars;
        rule.getBody()[i].getAggSharedVariables(allVariables, sharedVars);
        for (auto& var : sharedVars) {
            BB_ASSERT(variableCounter.contains(var));
            ++variableCounter[var];
        }
    }


    for (auto& [var, count]: variableCounter) {
        if (count > 1) {
            // count > 1 variables is shared with other atoms
            usedVariables.insert(var);
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
