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

#include <climits>

#include "bumblebee/parser/ParserInputBuilder.h"

#include "CLI11.hpp"
#include "bumblebee/common/Helper.h"
#include "bumblebee/common/Log.h"

namespace bumblebee {


ParserInputBuilder::ParserInputBuilder(OutputType type,bool hiddenNewPredicates):
    currentSchema_(Catalog::instance().getDefaultSchema()),
    hiddenNewPredicate(hiddenNewPredicates),
    output_builder_(type) {
}

ParserInputBuilder::~ParserInputBuilder() {
}

void ParserInputBuilder::onDirective(char *directiveName, char *directiveValue) {
}

bool ParserInputBuilder::checkRuleSafety() {
    return checkRuleSafety(currentRule);
}

bool ParserInputBuilder::checkRuleSafety(Rule& currentRule) {
    // check if rule is safe
    set_term_variable_t toCheckVars, bodyVars;
    for (auto&a : currentRule.getHead())a.getVariables(toCheckVars);
    for (auto&a : currentRule.getBody())
        if (a.isNegative())
            a.getVariables(toCheckVars);
    for (auto&a : currentRule.getBody())
        if (!a.isNegative())
            a.getVariables(bodyVars);
    // Check all the vars in head and in negative atoms are present in body
    for (auto& v : toCheckVars)
        if (bodyVars.find(v) == bodyVars.end()) {
            currentRuleIsUnsafe_ = true;
            break;
        }
    // check that range are not in the rule
    for (auto&a : currentRule.getHead())
        if (a.containsRange())
            currentRuleIsUnsafe_ = true;
    for (auto&a : currentRule.getBody())
        if (a.containsRange())
            currentRuleIsUnsafe_ = true;

    if(currentRuleIsUnsafe_){
        safetyErrorMessage="--> Safety Error: "+currentRule.toString();
        foundASafetyError_=true;
        return false;
    }
    return true;
}

void ParserInputBuilder::onRule() {
    if(foundASafetyError_) return;
    if (currentRule.isFact()) {
        Atom fact = std::move(currentRule.getHead()[0]);
        // print if is not internal atom
        auto& pt = currentSchema_.get().getPredicateTable(fact.getPredicate());
        pt->addFact(fact);
        currentRule = {};
        return;
    }
    if (!checkRuleSafety()) return;
    for (auto& a: currentRule.getBody())
        if (a.getType() == AGGREGATE) {
            rulesWithAggregates_.push_back(program_.size());
            break;
        }

    program_.push_back(std::move(currentRule));
    currentRule = {};
}


void ParserInputBuilder::onConstraint() {
    if (!checkRuleSafety()) return;
    program_.push_back(std::move(currentRule));
    currentRule = {};
}

void ParserInputBuilder::onWeakConstraint() {
}

void ParserInputBuilder::onQuery() {
    currentAtom.getPredicate()->setInternal(false);
}

void ParserInputBuilder::onHeadAtom() {
    if(foundASafetyError_) return;
    if (currentAtom.containsAnonymous())currentRuleIsUnsafe_=true;
    currentRule.addAtomInHead(std::move(currentAtom));
}

void ParserInputBuilder::onHead() {
}

void ParserInputBuilder::onBodyLiteral() {
    if(foundASafetyError_) return;
    currentRule.addAtomInBody(std::move(currentAtom));
}

void ParserInputBuilder::onBody() {
}

void ParserInputBuilder::onNafLiteral(bool naf) {
    if(foundASafetyError_) return;
    currentAtom.setNegative(naf);
    if (naf && currentAtom.containsAnonymous())currentRuleIsUnsafe_=true;
}

void ParserInputBuilder::onAtom(bool isStrongNeg) {
}

void ParserInputBuilder::onExistentialAtom() {
}

void ParserInputBuilder::onPredicateName(char *name) {
    if(foundASafetyError_) return;

    Predicate *predicate = currentSchema_.get().createPredicate(name, terms_parsered.size());
    if (!hiddenNewPredicate) {
        predicate->setInternal(false);
    }
    currentAtom = Atom::createClassicalAtom(predicate, std::move(terms_parsered));
    terms_parsered.clear();
}

void ParserInputBuilder::onExistentialVariable(char *var) {
}

void ParserInputBuilder::onEqualOperator() {
    binop_ = Binop::EQUAL;
}

void ParserInputBuilder::onUnequalOperator() {
    binop_ = Binop::UNEQUAL;
}

void ParserInputBuilder::onLessOperator() {
    binop_ = Binop::LESS;
}

void ParserInputBuilder::onLessOrEqualOperator() {
    binop_ = Binop::LESS_OR_EQ;
}

void ParserInputBuilder::onGreaterOperator() {
    binop_ = Binop::GREATER;
}

void ParserInputBuilder::onGreaterOrEqualOperator() {
    binop_ = Binop::GREATER_OR_EQ;
}

void ParserInputBuilder::onTerm(char *value) {
    if(foundASafetyError_) return;
    newTerm(value);
}


void ParserInputBuilder::onUnknownVariable() {
    if(foundASafetyError_) return;
    std::string s("_");
    Term term = Term::createVariable(std::move(s));
    terms_parsered.push_back(std::move(term));
}

void ParserInputBuilder::onFunction(char *functionSymbol, int nTerms) {
}

void ParserInputBuilder::onHeadTailList() {
}

void ParserInputBuilder::onListTerm(int nTerms) {
}

void ParserInputBuilder::onTermDash() {
    if(foundASafetyError_) return;

    Term& term = terms_parsered.back();
    term.setNegative(true);
    if (term.getType() == CONSTANT && term.getConstantType() != ConstantType::STRING) {
        // multiply the constant numeric value to -1
        auto newValue = term.getValue().cast(BIGINT).getNumericValue<int64_t>() * -1;
        Term::setConstantNumericTerm(term, newValue);
        return;
    }
    if (term.getType() == ARITH) {
        // multiply the arith term by -1
        int8_t mone = -1;
        Term moneTerm = Term(mone);
        term.addInArithTermBegin(std::move(moneTerm), Operator::TIMES);
        return;
    }
    if (term.getType() == VARIABLE) {
        // transform the variable term in arith and multiply by -1
        int8_t mone = -1;
        Term moneTerm = Term(mone);
        Term newArith = Term::createArith(std::move(term), std::move(moneTerm), '*');
        terms_parsered.pop_back();
        terms_parsered.push_back(std::move(newArith));
    }
    if (term.getType() == RANGE) {
        auto& interval = term.getInterval();
        interval.from = interval.from*-1;
    }
}

void ParserInputBuilder::onTermParams() {
}

void ParserInputBuilder::onTermRange(char *lowerBound, char *upperBound) {
    if(foundASafetyError_) return;

    Term t = Term::createRange(atoi(lowerBound), atoi(upperBound));
    terms_parsered.push_back(std::move(t));
}

void ParserInputBuilder::onArithmeticOperation(char arithOperator) {
    if(foundASafetyError_) return;

    auto lt = std::move(terms_parsered.back());
    terms_parsered.pop_back();
    auto slt = std::move(terms_parsered.back());
    terms_parsered.pop_back();

    if (lt.getType() != ARITH && slt.getType() != ARITH) {
        auto t = Term::createArith(std::move(slt), std::move(lt), arithOperator);
        terms_parsered.push_back(std::move(t));

        return;
    }
    if (lt.getType() == ARITH ) {
        // only last term is arith second last is normal term
        lt.addInArithTerm(std::move(slt), arithOperator);
        terms_parsered.push_back(std::move(lt));
        return;
    }
    // the second last is arith and the last is normal term
    slt.addInArithTerm(std::move(lt), arithOperator);
    terms_parsered.push_back(std::move(slt));
}

void ParserInputBuilder::onWeightAtLevels(int nWeight, int nLevel, int nTerm) {
}

void ParserInputBuilder::onChoiceLowerGuard() {
}

void ParserInputBuilder::onChoiceUpperGuard() {
}

void ParserInputBuilder::onChoiceElementAtom() {
}

void ParserInputBuilder::onChoiceElementLiteral() {
}

void ParserInputBuilder::onChoiceElement() {
}

void ParserInputBuilder::onChoiceAtom() {
}

void ParserInputBuilder::onBuiltinAtom() {
    if(foundASafetyError_) return;

    currentAtom = Atom::createBuiltinAtom(std::move(terms_parsered), binop_);
    terms_parsered.clear();
}

void ParserInputBuilder::onAggregateLowerGuard() {
    if(foundASafetyError_) return;
    if (guard_terms.size() != 2)
        guard_terms.resize(2);
    if (terms_parsered.back().containsAnonymous())
        currentRuleIsUnsafe_ = true;
    guard_terms[0] = std::move(terms_parsered.back());
    terms_parsered.pop_back();
}

void ParserInputBuilder::onAggregateUpperGuard() {
    if(foundASafetyError_) return;
    if (guard_terms.size() != 2)
        guard_terms.resize(2);
    if (terms_parsered.back().containsAnonymous())
        currentRuleIsUnsafe_ = true;
    secondBinop_ = binop_;
    binop_ = NONE_OP;
    guard_terms[1] = std::move(terms_parsered.back());
    terms_parsered.pop_back();
}

void ParserInputBuilder::onAggregateFunction(char *functionSymbol) {
    if (functionSymbol[0] == '#')
        aggregateFunction_ = Atom::getAggFunction(++functionSymbol); // skip #
    else
        aggregateFunction_ = Atom::getAggFunction(functionSymbol);
}

void ParserInputBuilder::onAggregateGroundTerm(char *value, bool dash) {
    if(foundASafetyError_) return;
    newTerm(value);
    agg_terms_parsered.push_back(std::move(terms_parsered.back()));
    terms_parsered.pop_back();
}

void ParserInputBuilder::onAggregateVariableTerm(char *value) {
    if(foundASafetyError_) return;
    string var(value);
    agg_terms_parsered.emplace_back(std::move(var), true);
}

void ParserInputBuilder::onAggregateUnknownVariable() {
    currentRuleIsUnsafe_ = true;
}

void ParserInputBuilder::onAggregateFunctionalTerm(char *value, int nTerms) {
}

void ParserInputBuilder::onAggregateNafLiteral() {
    if(foundASafetyError_) return;
    agg_atoms.push_back(std::move(currentAtom));
}

void ParserInputBuilder::onAggregateElement() {
    if(foundASafetyError_) return;
}

void ParserInputBuilder::onAggregate(bool naf) {
    if(foundASafetyError_) return;

    currentAtom = Atom::createAggregateAtom(aggregateFunction_, binop_, secondBinop_, guard_terms[0], guard_terms[1], std::move(agg_terms_parsered), std::move(agg_atoms));
    binop_ = NONE_OP;
    secondBinop_ = NONE_OP;
    guard_terms.clear();
    agg_atoms.clear();
    agg_terms_parsered.clear();
}


void ParserInputBuilder::rewriteAggregates() {
    //TODO move the rewriting in the optimizer (maybe in the optimzier v2)

    // Transform aggregate bodies (which may contain multiple atoms) into a single atom.
    // Example transformation:
    //
    //   a(X), #sum{Y : b(X), c(X,Y)} = T.
    //     -> a(X), #sum{Y : #AGG1_SUM_GROUP_0_PAYLOADS_1(X,Y)} = T.
    //        #SUM_GROUP_0_PAYLOADS_1(X,Y) :- b(X), c(X,Y).
    // The new predicate name encodes:
    //   - the aggregates function (e.g., #SUM),
    //   - the grouping variables,
    //   - and the payload
    // Details:
    // - Groups are the variables shared between the aggregate and the rest of the rule body.
    // - Payloads are the columns being aggregated. Note: payloads.size() == functions.size().
    // - Since aggregate tables can apply multiple functions on the same groups, we can
    //   reuse aggregates across rules when possible.
    //
    // Two aggregates can be reused if they share:
    //   1. the same groups,
    //   2. the same body atoms,
    //   3. the same aggregation terms (group variables + variables defined inside the aggregate).

    if (rulesWithAggregates_.size() == 0)return;
    // information to create an aggregate table
    struct AggInfo {
        AggInfo(vector<Atom> &atoms, set_term_variable_t &groups, set_term_variable_t  &terms)
            : atoms(atoms), groups(groups), terms(std::move(terms)) {
        }
        string createAggPredicateName(idx_t& suffixCounter, const vector<Term>& terms) {
            if (predName.size() > 0)return predName;
            vector<idx_t> groups, payloads;
            vector<string> aggFunctions;
            for (idx_t i=0;i<terms.size();++i) {
                auto& t = terms[i];
                BB_ASSERT(t.getType() == VARIABLE);
                if (this->groups.contains(t.getVariable()))
                    groups.push_back(i);
                if (this->payloadMap.contains(t.getVariable())) {
                    for (auto& fun:payloadMap[t.getVariable()]) {
                        payloads.push_back(i);
                        aggFunctions.push_back(fun);
                    }
                }
            }
            predName = Predicate::buildAggregateInternalPredicate(suffixCounter++, groups, payloads, aggFunctions);
            return predName;
        }
        // atoms in the body of the aggregate
        vector<Atom>& atoms;
        // aggregation terms (group variables + distinct aggregation variable)
        set_term_variable_t terms;
        // group variable in terms
        set_term_variable_t groups;
        // map of payload variable and agg function
        std::unordered_map<string, std::unordered_set<string>> payloadMap;
        // name of the predicate
        string predName;

    };

    vector<AggInfo> aggInfos;
    // index of aggInfos to use for the ith aggregates
    vector<idx_t> aggInfosIndex;
    for (idx_t i=0;i<rulesWithAggregates_.size();i++) {
        auto& rule = program_[rulesWithAggregates_[i]];
        set_term_variable_t ruleVariables;
        rule.getVariables(ruleVariables);
        for (auto& a: rule.getBody()) {
            if (a.getType() != AGGREGATE)continue;
            // calculate the groups
            set_term_variable_t groupVars;
            a.getAggSharedVariables(ruleVariables, groupVars);
            // now join the aggregate terms + the shared variables and the set of vars would be vars in head of new predicate
            set_term_variable_t sterms = groupVars;
            for (auto&t: a.getAggTerms()) sterms.insert((t.getVariable()));
            auto& payload = a.getAggTerms()[0]; // payload is the first term

            // now check if is present in the aggInfos ( avoiding creating duplicates aggregates tables)
            bool found = false;
            for (idx_t j=0;j<aggInfos.size() && !found;j++) {
                auto& info = aggInfos[j];
                if (compareVectorsNoSort(info.atoms, a.getAggsAtoms())
                    && info.groups == groupVars
                    && info.terms == sterms) {
                    // we can reuse this aggregate
                    aggInfosIndex.push_back(j);
                    found = true;
                    info.payloadMap[payload.getVariable()].insert(a.getAggregateFunctionName());
                }
            }
            if (found) continue;
            // we need to create a new aggregate tables
            aggInfosIndex.push_back(aggInfos.size());
            aggInfos.emplace_back(a.getAggsAtoms(), groupVars, sterms);
            // add the information of function and payload
            aggInfos.back().payloadMap[payload.getVariable()].insert(a.getAggregateFunctionName());
        }
    }

    // now for each aggregate we have the index to the aggregate table specs (aggInfosIndex)
    // and we can replace the aggregate atoms with the single atom
    idx_t counter = 0;
    for (idx_t i=0;i<rulesWithAggregates_.size();i++) {
        // because of the push back do not create rule variable
        auto ruleIndex = rulesWithAggregates_[i];
        set_term_variable_t ruleVariables;
        program_[ruleIndex].getVariables(ruleVariables);
        for (idx_t j=0;j<program_[ruleIndex].getBody().size();j++) {
            auto& a = program_[ruleIndex].getBody()[j];
            if (a.getType() != AGGREGATE)continue;
            BB_ASSERT(aggInfosIndex[counter] < aggInfos.size());
            auto &info = aggInfos[aggInfosIndex[counter++]];
            vector<Term> terms;
            for (auto& v: info.terms)
                terms.emplace_back(v.c_str(), true);
            bool createAuxRule = info.predName.empty();
            string newPredName = info.createAggPredicateName(newPredCounter_, terms);
            Predicate *predicate = currentSchema_.get().createPredicate(newPredName.c_str(), info.terms.size());
            if (!hiddenNewPredicate) {
                predicate->setInternal(false);
            }
            if (createAuxRule) {
                Atom head = Atom::createClassicalAtom(predicate, std::move(vector(terms)));
                Rule newRule;
                newRule.addAtomInHead(std::move(head));
                newRule.setBody(a.getAggsAtoms());
                checkRuleSafety(newRule);
                program_.push_back(std::move(newRule));
            }
            auto newAtom =  Atom::createClassicalAtom(predicate, std::move(vector(terms)));
            a.getAggsAtoms().clear();
            a.getAggsAtoms().push_back(std::move(newAtom));
        }
    }
}

void ParserInputBuilder::onEnd() {
    rewriteAggregates();
}

rules_vector_t & ParserInputBuilder::getProgram() {
    return program_;
}

void ParserInputBuilder::newTerm(char * value) {
    if(foundASafetyError_) return;

    if( value[0] >= 'A' && value[0] <='Z' ) // Variable
    {
        std::string s(value);
        Term term = Term::createVariable(std::move(s));
        terms_parsered.push_back(std::move(term));
        return;
    }
    if( (value[0] == '\"' && value[strlen(value)-1] == '\"') ||
            (value[0] >= 'a' && value[0] <='z') )   // String constant
    {
        std::string s(value);
        if (value[0] == '\"')
            // remove the quote
            s = s.substr(1, s.size() - 2);

        Term term = Term(std::move(s));
        terms_parsered.push_back(std::move(term));
        return;
    }
    // Numeric constant
    long long num = atoll(value);
    if (num >= 0) {
        // Positive number
        unsigned long long unum = strtoull(value, nullptr, 10);
        Term term  = Term::createSmallestConstantNumericTerm(unum);
        terms_parsered.push_back(std::move(term));
        return;
    }
    Term term  = Term::createSmallestConstantNumericTerm(num);
    terms_parsered.push_back(std::move(term));
}

bool ParserInputBuilder::isFoundASafetyError() {
    return foundASafetyError_;
}

const std::string & ParserInputBuilder::getSafetyErrorMessage() {
    return safetyErrorMessage;
}

}
