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
#include "bumblebee/planner/rewriter/AggregatesRewriter.hpp"

#include "bumblebee/ClientContext.hpp"
#include "bumblebee/common/Helper.hpp"

namespace bumblebee{
AggInfo::AggInfo(vector<Atom> &atoms, set_term_variable_t &groups, set_term_variable_t &terms, bool distinct): atoms_(atoms), groups_(groups), terms_(std::move(terms)), distinct_(distinct) {
}

string AggInfo::createAggPredicateName(idx_t &suffixCounter, const vector<Term> &terms) {
    if (predName_.size() > 0)return predName_;
    vector<idx_t> groups, payloads;
    vector<string> aggFunctions;
    for (idx_t i=0;i<terms.size();++i) {
        auto& t = terms[i];
        BB_ASSERT(t.getType() == VARIABLE);
        if (this->groups_.contains(t.getVariable()))
            groups.push_back(i);
        if (this->payloadMap_.contains(t.getVariable())) {
            for (auto& fun:payloadMap_[t.getVariable()]) {
                payloads.push_back(i);
                aggFunctions.push_back(fun);
            }
        }
    }
    predName_ = Predicate::buildAggregateInternalPredicate({suffixCounter++, groups, payloads, aggFunctions, distinct_});
    return predName_;
}

AggregatesRewriter::AggregatesRewriter(ClientContext &clientContext): clientContext_(clientContext) {
}

void AggregatesRewriter::findAggregates(rules_vector_t &program) {
    rulesWithAggregates_.clear();
    for (idx_t i=0;i<program.size();++i) {
        auto& rule = program[i];
        for (auto& atom:rule.getBody()) {
            if (atom.getType() == AGGREGATE)
                rulesWithAggregates_.push_back(i);
        }
    }
}

void AggregatesRewriter::rewrite(rules_vector_t &program) {
    findAggregates(program);
    if (rulesWithAggregates_.empty())return;

    vector<AggInfo> aggInfos;
    // index of aggInfos to use for the ith aggregates
    vector<idx_t> aggInfosIndex;
    for (idx_t i=0;i<rulesWithAggregates_.size();i++) {
        auto& rule = program[rulesWithAggregates_[i]];
        set_term_variable_t ruleVariables;
        rule.getVariables(ruleVariables);
        for (auto& a: rule.getBody()) {
            if (a.getType() != AGGREGATE)continue;
            // calculate the groups
            set_term_variable_t groupVars;
            a.getAggSharedVariables(ruleVariables, groupVars);
            // now join the aggregate terms + the shared variables and the set of vars would be vars in head of new predicate
            set_term_variable_t sterms = groupVars;
            bool distinct = true;
            for (auto&t: a.getAggTerms()) {
                BB_ASSERT(t.getType() == VARIABLE);
                if (t.getVariable() == Predicate::INTERNAL_ID_VAR)
                    distinct = false;
                else
                    sterms.insert((t.getVariable()));
            }
            auto& payload = a.getAggTerms()[0]; // payload is the first term

            // now check if is present in the aggInfos ( avoiding creating duplicates aggregates tables)
            bool found = false;
            for (idx_t j=0;j<aggInfos.size() && !found;j++) {
                auto& info = aggInfos[j];
                if (compareVectorsNoSort(info.atoms_, a.getAggsAtoms())
                    && info.groups_ == groupVars
                    && info.terms_ == sterms
                    && info.distinct_ == distinct) {
                    // we can reuse this aggregate
                    aggInfosIndex.push_back(j);
                    found = true;
                    info.payloadMap_[payload.getVariable()].insert(a.getAggregateFunctionName());
                }
            }
            if (found) continue;
            // we need to create a new aggregate tables
            aggInfosIndex.push_back(aggInfos.size());
            aggInfos.emplace_back(a.getAggsAtoms(), groupVars, sterms, distinct);
            // add the information of function and payload
            aggInfos.back().payloadMap_[payload.getVariable()].insert(a.getAggregateFunctionName());
        }
    }

    // now for each aggregate we have the index to the aggregate table specs (aggInfosIndex)
    // and we can replace the aggregate atoms with the single atom
    idx_t counter = 0;
    for (idx_t i=0;i<rulesWithAggregates_.size();i++) {
        // because of the push back do not create rule variable
        auto ruleIndex = rulesWithAggregates_[i];
        set_term_variable_t ruleVariables;
        program[ruleIndex].getVariables(ruleVariables);
        for (idx_t j=0;j<program[ruleIndex].getBody().size();j++) {
            auto& a = program[ruleIndex].getBody()[j];
            if (a.getType() != AGGREGATE)continue;
            BB_ASSERT(aggInfosIndex[counter] < aggInfos.size());
            auto &info = aggInfos[aggInfosIndex[counter++]];
            vector<Term> terms;
            for (auto& v: info.terms_)
                terms.emplace_back(v.c_str(), true);
            bool createAuxRule = info.predName_.empty();
            string newPredName = info.createAggPredicateName(newPredCounter_, terms);
            Predicate *predicate = clientContext_.defaultSchema_.createPredicate(&clientContext_, newPredName.c_str(), info.terms_.size());
            if (clientContext_.printAll_) {
                predicate->setInternal(false);
            }
            if (createAuxRule) {
                Atom head = Atom::createClassicalAtom(predicate, std::move(vector(terms)));
                Rule newRule;
                newRule.addAtomInHead(std::move(head));
                newRule.setBody(a.getAggsAtoms());
                program.push_back(std::move(newRule));
            }
            auto newAtom =  Atom::createClassicalAtom(predicate, std::move(vector(terms)));
            a.getAggsAtoms().clear();
            a.getAggsAtoms().push_back(std::move(newAtom));
        }
    }
}
}
