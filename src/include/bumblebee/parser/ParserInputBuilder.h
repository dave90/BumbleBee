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
#pragma once

#include <string>

#include "bumblebee/catalog/Catalog.h"
#include "bumblebee/catalog/Schema.h"
#include "bumblebee/parser/statement/Term.h"
#include "statement/Atom.h"
#include "statement/Rule.h"

namespace bumblebee {
	class Term;

	class ParserInputBuilder {
public:
	ParserInputBuilder();
	virtual ~ParserInputBuilder();

	virtual void onDirective( char* directiveName, char* directiveValue );

	bool checkRuleSafety();

	virtual void onRule();
	virtual void onConstraint();
	virtual void onWeakConstraint();
	virtual void onQuery();
	virtual void onHeadAtom();
	virtual void onHead();
	virtual void onBodyLiteral();
	virtual void onBody();
	virtual void onNafLiteral( bool naf = false );
	virtual void onAtom( bool isStrongNeg = false );
	virtual void onExistentialAtom();
	virtual void onPredicateName( char* name );
	virtual void onExistentialVariable( char* var );
	virtual void onEqualOperator();
	virtual void onUnequalOperator();
	virtual void onLessOperator();
	virtual void onLessOrEqualOperator();
	virtual void onGreaterOperator();
	virtual void onGreaterOrEqualOperator();
	virtual void onTerm( char* value );
	virtual void onUnknownVariable();
	virtual void onFunction( char* functionSymbol, int nTerms );
	virtual void onHeadTailList();
	virtual void onListTerm( int nTerms );
	virtual void onTermDash();
	virtual void onTermParams();
	virtual void onTermRange( char* lowerBound, char* upperBound );
	virtual void onArithmeticOperation( char arithOperator );
	virtual void onWeightAtLevels( int nWeight, int nLevel, int nTerm );
	virtual void onChoiceLowerGuard();
	virtual void onChoiceUpperGuard();
	virtual void onChoiceElementAtom();
	virtual void onChoiceElementLiteral();
	virtual void onChoiceElement();
	virtual void onChoiceAtom();
	virtual void onBuiltinAtom();
	virtual void onAggregateLowerGuard();
	virtual void onAggregateUpperGuard();
	virtual void onAggregateFunction( char* functionSymbol );
	virtual void onAggregateGroundTerm( char* value, bool dash = false );
	virtual void onAggregateVariableTerm( char* value );
	virtual void onAggregateUnknownVariable();
	virtual void onAggregateFunctionalTerm( char* value, int nTerms );
	virtual void onAggregateNafLiteral();
	virtual void onAggregateElement();
	virtual void onAggregate( bool naf = false );
	virtual void onEnd();


	void newTerm(char*);
	bool isFoundASafetyError();
	const std::string& getSafetyErrorMessage();
	rules_vector_t& getProgram();

protected:
	std::reference_wrapper<Schema> currentSchema_;
	bool foundARangeAtomInCurrentRule_{false};
	bool currentRuleIsUnsafe_{false};
	bool foundASafetyError_{false};
	Binop binop_;

	std::vector<Term> terms_parsered;
	Atom currentAtom{};
	Rule currentRule{};

	bool foundARangeAtomInCurrentRule{false};
	bool hiddenNewPredicate{false};
	bool currentRewriteArith{false};

	std::string safetyErrorMessage;
	rules_vector_t program_;

};

}
