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

    Atom bodyAggAtom = extractRuleFromAgg(agg_terms_parsered, agg_atoms);
    agg_atoms.clear();
    agg_atoms.push_back(std::move(bodyAggAtom));

    currentAtom = Atom::createAggregateAtom(aggregateFunction_, binop_, secondBinop_, guard_terms[0], guard_terms[1], std::move(agg_terms_parsered), std::move(agg_atoms));
    binop_ = NONE_OP;
    secondBinop_ = NONE_OP;
    guard_terms.clear();
    agg_atoms.clear();
    agg_terms_parsered.clear();
}

Predicate* ParserInputBuilder::getAuxHeadAtomAggRule(std::vector<Term>& aggTerms, std::vector<Atom>& atoms) {
    for (idx_t i = 0; i <auxAggRulesCreated_.size(); i++) {
        auto& rule = program_[ auxAggRulesCreated_[i]];
        // compare terms in head
        BB_ASSERT(rule.getHead().size() == 1);
        auto &headTerms = rule.getHead()[0].getTerms();
        if (headTerms != aggTerms)continue;
        // compare the body
        auto& bodyAtoms = rule.getBody();
        if (bodyAtoms.size() != atoms.size())continue;
        std::vector<bool> used(atoms.size(), false);
        bool matched = false;
        for (const auto& x : bodyAtoms) {
            matched = false;
            for (std::size_t j = 0; j < atoms.size(); ++j) {
                if (!used[j] && x == atoms[j]) {
                    used[j] = true;   // consume this occurrence
                    matched = true;
                    break;
                }
            }
            if (!matched) break; // x has no unused match in atoms
        }
        if (!matched) continue;
        // match found we can resuse this aux
        return rule.getHead()[0].getPredicate();
    }
    return nullptr;
}

Atom ParserInputBuilder::extractRuleFromAgg(std::vector<Term>& aggTerms, std::vector<Atom>& atoms) {
    // check if the aux rule with similar body and terms was alredy created
    Predicate* auxPredAtom = getAuxHeadAtomAggRule(aggTerms, atoms);
    if (auxPredAtom) {
        // aux rule alredy create, return the atom
        return Atom::createClassicalAtom(auxPredAtom, std::move(std::vector(aggTerms)));
    }

    // extract a new rule from the aggregate atoms
    // and return the atom in head
    string newPred = NEW_PREDICATE_AGG_PREFIX.c_str() + std::to_string(newPredCounter_++);
    Predicate *predicate = currentSchema_.get().createPredicate(newPred.c_str(), aggTerms.size());
    if (!hiddenNewPredicate) {
        predicate->setInternal(false);
    }
    Atom head = Atom::createClassicalAtom(predicate, std::move(std::vector(aggTerms)));
    Rule newRule;
    newRule.addAtomInHead(std::move(head));
    newRule.setBody(atoms);
    checkRuleSafety(newRule);
    auxAggRulesCreated_.push_back(program_.size());
    program_.push_back(std::move(newRule));
    return Atom::createClassicalAtom(predicate, std::move(std::vector(aggTerms)));
}

void ParserInputBuilder::onEnd() {
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
