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
#include "bumblebee/parser/ParserInputBuilder.h"

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
}

void ParserInputBuilder::onTerm(int value) {
}

void ParserInputBuilder::onUnknownVariable() {
}

void ParserInputBuilder::onFunction(char *functionSymbol, int nTerms) {
}

void ParserInputBuilder::onHeadTailList() {
}

void ParserInputBuilder::onListTerm(int nTerms) {
}

void ParserInputBuilder::onTermDash() {
}

void ParserInputBuilder::onTermParams() {
}

void ParserInputBuilder::onTermRange(char *lowerBound, char *upperBound) {
}

void ParserInputBuilder::onArithmeticOperation(char arithOperator) {
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

void ParserInputBuilder::newTerm(char *) {
}

bool ParserInputBuilder::isFoundASafetyError() {
    return foundASafetyError_;
}

const std::string & ParserInputBuilder::getSafetyErrorMessage() {
    return safetyErrorMessage_;
}

}