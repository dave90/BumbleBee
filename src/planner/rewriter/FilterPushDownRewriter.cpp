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
#include "bumblebee/planner/rewriter/FilterPushDownRewriter.h"

#include "bumblebee/common/types/Assert.h"

namespace bumblebee{
FilterPushDownRewriter::~FilterPushDownRewriter() {}

void FilterPushDownRewriter::rewrite(Rule &rule) {
    auto& body = rule.getBody();

    // construct the new body with only classical atoms
    atoms_vector_t newBody;

    // fetch all the builtin and aggregates and the vars
    // for the aggregate we store the shared variables
    // aggregates will be treated similar to assignment
    vector<Atom*> builtIntAtoms;
    vector<set_term_variable_t> builtInVariables;
    set_term_variable_t ruleVars;
    for (auto& atom : body) {
        atom.getVariables(ruleVars);
        if (atom.getType() == BUILTIN) {
            set_term_variable_t vars;
            atom.getVariables(vars);

            builtInVariables.push_back(vars);
            builtIntAtoms.push_back(&atom);
        }
        if (atom.getType() == AGGREGATE) {
            builtIntAtoms.push_back(&atom);
            builtInVariables.emplace_back();
        }
    }

    for (idx_t i = 0; i < builtIntAtoms.size(); i++) {
        auto aggAtom = builtIntAtoms[i];
        if (aggAtom->getType() != AGGREGATE) continue;
        BB_ASSERT(aggAtom->getBinop() == ASSIGNMENT);
        set_term_variable_t sharedVars;
        aggAtom->getAggSharedVariables(ruleVars, sharedVars);
        sharedVars.erase(aggAtom->getTerms()[0].getVariable()); // remove the assignment var
        builtInVariables[i] = std::move(sharedVars);
    }


    set_term_variable_t currentVariables;
    for (idx_t i=0; i< body.size(); i++) {
        if (body[i].getType() == BUILTIN || body[i].getType() == AGGREGATE) continue;
        // add the variables of this classical atoms in the current variables set
        // and push in the new body (not changing the order of classical atoms)
        body[i].getVariables(currentVariables);
        newBody.push_back(std::move(body[i]));

        // first add the constant assignment
        for (idx_t j=0; j< builtIntAtoms.size(); j++) {
            auto atom = builtIntAtoms[j];
            if (atom == nullptr)continue;
            if (!atom->isConstantAssignment())continue;
            atom->setBinop(ASSIGNMENT);
            atom->getVariables(currentVariables);
            newBody.push_back(std::move(*atom));
            builtIntAtoms[j] = nullptr;
        }

        // first check the possible assignment
        // we need to loop again and again for cascade assignment ( X = Y, Z = X, etc.)
        for (idx_t j=0; j< builtIntAtoms.size();) {
            auto ba = builtIntAtoms[j];
            // if is nullptr builtin already inserted in the new body
            if (ba == nullptr) {
                ++j;
                continue;
            }
            // if is equal or assignment and is possible to evaluate now (so only one variable is not bounded)
            // then evaluate it now

            if ((
                ba->getType() == BUILTIN
                 && ( ba->getBinop() == ASSIGNMENT || ba->getBinop() == EQUAL)
                 && isAssignmePossibleToEvaluate(currentVariables, builtInVariables[j], *ba))
                 ||(
                 ba->getType() == AGGREGATE
                 && Term::subset(currentVariables, builtInVariables[j])
                 )) {
                    ba->setBinop(ASSIGNMENT);
                    ba->getVariables(currentVariables);
                    newBody.push_back(std::move(*ba));
                    builtIntAtoms[j] = nullptr;
                    // new assignment found restart from 0
                    j = 0;
                    continue;
            }

            ++j;
        }


        // now check the builtin atoms if is possible to insert one
        // and while true until no more new atoms is possible to insert
        for (idx_t j=0; j< builtIntAtoms.size();++j) {
            auto ba = builtIntAtoms[j];
            // if is nullptr builtin already inserted in the new body
            if (ba == nullptr || ba->getType() == AGGREGATE)continue;
            // if all the variables are present, we can execute as filter
            if (Term::subset(currentVariables, builtInVariables[j])) {
                if (ba->getBinop() == ASSIGNMENT) {
                    // change to EQUAL as all the variables are present
                    ba->setBinop(EQUAL);
                }
                newBody.push_back(std::move(*ba));
                builtIntAtoms[j] = nullptr;
            }
        }

    }

    // check the remaining builtin (if any) and throw error if are present
    for (idx_t j=0; j< builtIntAtoms.size(); j++) {
        auto ba = builtIntAtoms[j];
        BB_ASSERT(ba == nullptr);
    }

    rule.setBody(newBody);
}

bool FilterPushDownRewriter::isAssignmePossibleToEvaluate(set_term_variable_t &currentVariables,set_term_variable_t &builtinVars, Atom &assignment) {
    BB_ASSERT(assignment.getType() == BUILTIN);
    BB_ASSERT(assignment.getBinop() == EQUAL || assignment.getBinop() == ASSIGNMENT);
    // TODO rewrite the arith; for now we do not rewrite the formula we work with assignment
    // only if left or right has one variable

    if (assignment.isConstantAssignment())return true;

    set_term_variable_t intersection;
    Term::intersetVariables(currentVariables, builtinVars, intersection );
    if (intersection.size() +1 !=  builtinVars.size()  )return false;
    // only one variable is not bounded, find it
    vector<string> varsNotBounded;
    for (auto& v:builtinVars)
        if (!intersection.contains(v)) {
            varsNotBounded.push_back(v);
        }
    BB_ASSERT(varsNotBounded.size() == 1);
    set_term_variable_t leftVariables, rightVariables ;
    assignment.getTerms()[0].getVariables(leftVariables);
    assignment.getTerms()[1].getVariables(rightVariables);
    if (leftVariables.size() == 1 && leftVariables.contains(varsNotBounded[0])) {
        // left side is the assignment
        assignment.setBinop(ASSIGNMENT);
        return true;
    }
    if (rightVariables.size() == 1 && rightVariables.contains(varsNotBounded[0])) {
        // right side is the assignment, swap it
        assignment.getTerms()[1] = std::move(assignment.getTerms()[0]);
        Term assignVar(varsNotBounded[0].c_str(), true);
        assignment.getTerms()[0] = std::move(assignVar);
        assignment.setBinop(ASSIGNMENT);
        return true;
    }
    // TODO rewrite arith if is not assignment, for now return false
    return false;

}
}
