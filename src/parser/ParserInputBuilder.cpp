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

#include "bumblebee/common/Log.h"

namespace bumblebee {


ParserInputBuilder::ParserInputBuilder() {
}

ParserInputBuilder::~ParserInputBuilder() {
}

void ParserInputBuilder::onDirective(char *directiveName, char *directiveValue) {
}

void ParserInputBuilder::onRule() {
}

void ParserInputBuilder::onConstraint() {
}

void ParserInputBuilder::onWeakConstraint() {
}

void ParserInputBuilder::onQuery() {
}

void ParserInputBuilder::onHeadAtom() {
}

void ParserInputBuilder::onHead() {
}

void ParserInputBuilder::onBodyLiteral() {
}

void ParserInputBuilder::onBody() {
}

void ParserInputBuilder::onNafLiteral(bool naf) {
}

void ParserInputBuilder::onAtom(bool isStrongNeg) {

}

void ParserInputBuilder::onExistentialAtom() {
}

void ParserInputBuilder::onPredicateName(char *name) {
    // TODO keep track of the type of the terms and keep the largest one in predicate tables to calculate the type of a column

    std::cout << "Parsed terms: ";
    for (auto&t : terms_parsered) {
        std::cout<< "( " << t.toString() <<" , "<< t.getType() << " , " << t.getConstantType() << " , " << t.isAnonymous() << " ) " ;
    }
    std::cout<<std::endl;
    terms_parsered.clear();
}

void ParserInputBuilder::onExistentialVariable(char *var) {
}

void ParserInputBuilder::onEqualOperator() {
}

void ParserInputBuilder::onUnequalOperator() {
}

void ParserInputBuilder::onLessOperator() {
}

void ParserInputBuilder::onLessOrEqualOperator() {
}

void ParserInputBuilder::onGreaterOperator() {
}

void ParserInputBuilder::onGreaterOrEqualOperator() {
}

void ParserInputBuilder::onTerm(char *value) {
    newTerm(value);
}


void ParserInputBuilder::onUnknownVariable() {
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
    Term& term = terms_parsered.back();
    term.setNegative(true);
    if (term.getType() == CONSTANT && term.getConstantType() != ConstantType::STRING) {
        // multiply the constant numeric value to -1
        Term::setConstantNumericTerm(term, term.getNumericValue<int64_t>() * -1);
        return;
    }
    if (term.getType() == ARITH) {
        // multiply the arith term by -1
        int8_t mone = -1;
        Term moneTerm = Term::createConstantTerm(mone);
        term.addInArithTermBegin(std::move(moneTerm), Operator::TIMES);
        return;
    }
    if (term.getType() == VARIABLE) {
        // transform the variable term in arith and multiply by -1
        int8_t mone = -1;
        Term moneTerm = Term::createConstantTerm(mone);
        Term newArith = Term::createArith(std::move(term), std::move(moneTerm), '*');
        terms_parsered.pop_back();
        terms_parsered.push_back(std::move(newArith));
    }
}

void ParserInputBuilder::onTermParams() {
}

void ParserInputBuilder::onTermRange(char *lowerBound, char *upperBound) {
    Term t = Term::createRange(atoi(lowerBound), atoi(upperBound));
    terms_parsered.push_back(std::move(t));
}

void ParserInputBuilder::onArithmeticOperation(char arithOperator) {
    auto lt = std::move(terms_parsered.back());
    terms_parsered.pop_back();
    auto slt = std::move(terms_parsered.back());
    terms_parsered.pop_back();

    if (lt.getType() != ARITH && slt.getType() != ARITH) {
        auto t = Term::createArith(std::move(slt), std::move(lt), arithOperator);
        terms_parsered.push_back(std::move(t));
        LOG_DEBUG("SUM TERM ARITH");

        return;
    }
    if (lt.getType() == ARITH ) {
        // only last term is arith second last is normal term
        lt.addInArithTerm(std::move(slt), arithOperator);
        terms_parsered.push_back(std::move(lt));
        LOG_DEBUG("MERGE ARITH");
        return;
    }
    // the second last is arith and the last is normal term
    LOG_DEBUG("NONE TERM ARITH");

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
}

void ParserInputBuilder::onAggregateLowerGuard() {
}

void ParserInputBuilder::onAggregateUpperGuard() {
}

void ParserInputBuilder::onAggregateFunction(char *functionSymbol) {
}

void ParserInputBuilder::onAggregateGroundTerm(char *value, bool dash) {
}

void ParserInputBuilder::onAggregateVariableTerm(char *value) {
}

void ParserInputBuilder::onAggregateUnknownVariable() {
}

void ParserInputBuilder::onAggregateFunctionalTerm(char *value, int nTerms) {
}

void ParserInputBuilder::onAggregateNafLiteral() {
}

void ParserInputBuilder::onAggregateElement() {
}

void ParserInputBuilder::onAggregate(bool naf) {
}

void ParserInputBuilder::onEnd() {
}

void ParserInputBuilder::newTerm(char * value) {
    LOG_DEBUG("NEW TERM %s",value);
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
        Term term = Term::createConstantTerm(std::move(s));
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
    return safetyErrorMessage_;
}

}
