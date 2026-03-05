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

#include "bumblebee/catalog/Catalog.hpp"
#include "bumblebee/catalog/Schema.hpp"
#include "bumblebee/output/OutputBuilder.hpp"
#include "bumblebee/parser/statement/Term.hpp"
#include "statement/Atom.hpp"
#include "statement/Rule.hpp"
#include "statement/sql/SQLStatement.hpp"
#include "statement/sql/ValueExpr.hpp"

namespace bumblebee {


class Term;

class ParserInputBuilder {
public:
	// TODO refactor pass client context
	ParserInputBuilder(OutputType type, ClientContext& context);
	virtual ~ParserInputBuilder();

	virtual void onDirective( char* directiveName, char* directiveValue );

	bool checkRuleSafety(Rule& currentRule);
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
	virtual void onBuiltinOrList();
	virtual void onAggregateLowerGuard();
	virtual void onAggregateUpperGuard();
	virtual void onAggregateFunction( char* functionSymbol );
	virtual void onAggregateGroundTerm( char* value, bool dash = false );
	virtual void onAggregateVariableTerm( char* value );
	virtual void onAggregateUnknownVariable();
	virtual void onAggregateFunctionalTerm( char* value, int nTerms );
	virtual void onAggregateNafLiteral();
	virtual void onAggregateElement();
	virtual void onAggregateGroupSemicolon();
	virtual void onAggregate( bool naf = false );
	virtual void onMultiAssignVariable(char* var);
	virtual void onMultiAggregateAssignment();

	virtual void onExtAtom(bool naf = false);
	virtual void onSemicolon();
	virtual void onExternalPredicateName(char * name);
	virtual void onNamedParameter();


	virtual void onEnd();


	void newTerm(char*);
	bool isFoundASafetyError();
	const std::string& getSafetyErrorMessage();
	rules_vector_t& getProgram();
	void parseLimitDirective(const string& value);
	void parseOrderByDirective(const string& value);
	void setRuleDirective();

	bool isSQL();
	void onSQLValue(char*);
	void onSQLQualifiedName(char* name, char* table = nullptr);
	void onSQLMulTerm(char);
	void onSQLFinalizeMulExpr();
	void onSQLAddTerm(char);
	void onSQLParenOpen();
	void onSQLParenClose();
	void onSQLSelectItem();
	void onSQLAlias(char*);
	void onSQLTableRef(char*);
	void onSQLFromItem();
	void onSQLPredicate();
	void onSQLPredicateValueExpr();
	void onSQLPredicateValueExprOp();
	void onSQLLikePredicate();
	void onSQLOperatorCondition(const char *);
	void onSQLWhereGroupBegin();
	void onSQLWhereGroupEnd();

	void onSQLWhere();
	void onSQLFrom();
	void onSQLSelect();
	void onSQLSubQuery();
	void onSQLStart();
	void onSQLWhereSubqueryPredicate();
	void onSQLInListValue();
	void onSQLInListPredicate(bool isNotIn);
	void onSQLInSubqueryPredicate(bool isNotIn);
	void onSQLExtTable();
	void onSQLExtTableName(char*);
	void onSQLExtTableNameString(char*);
	sql::SQLStatement& getSqlStatement();
	void onSQLAggregateFunction(char*);
	void onSQLGroupByItem();
	void onSQLCopyTo(char*);
	void onSQLCopy();
	void onSqlOrderModifier(char*);
	void onSqlOrderCol();
	void onSQLLimit(char*);
	void onSQLTopLevelAlias();

protected:
	OutputBuilder output_builder_{NONE_OUTPUT};
	std::reference_wrapper<Schema> currentSchema_;
	bool foundARangeAtomInCurrentRule_{false};
	bool currentRuleIsUnsafe_{false};
	bool foundASafetyError_{false};
	Binop binop_{NONE_OP};
	Binop secondBinop_{NONE_OP};
	Binop aggBinop_{NONE_OP};
	Binop aggSecondBinop_{NONE_OP};
	vector<AggregateFunctionType> aggregateFunctions_;

	vector<Term> terms_parsered;
	vector<Term> guard_terms;
	vector<Term> agg_terms_parsered;
	vector<Term> agg_group_terms_parsered;
	vector<Term> multi_assign_terms_;
	vector<Atom> agg_atoms;
	vector<Atom> builtin_atoms;
	Atom currentAtom{};
	Rule currentRule{};

	bool foundARangeAtomInCurrentRule{false};
	bool hiddenNewPredicate{true};
	bool currentRewriteArith{false};

	std::string safetyErrorMessage;
	rules_vector_t program_;

	vector<idx_t> rulesWithAggregates_;

	idx_t newPredCounter_{0};
	bool distinctNewPredicate_;
	BufferManager &bufferManager_;
	ClientContext &clientContext_;

	idx_t externalSemicolumnCount_{0};
	std::unordered_map<string, Value> namedParameters_;
	vector<Value> inputValues_;
	string externalFunctionName_;
	idx_t extAtomCounter_{0};

	idx_t limit_{0};
	vector<OrderModifiers> orderModifiers_;
	vector<string> colsOrderModifiers_;


	// SQL parser
	bool isSql_{false};
	vector<sql::ValuePrimary> sqlValuePrimary_;
	vector<sql::ValueExpr> inListValues_;
	string alias_;
	sql::ValueExpr valueExpr_;
	sql::ValueExpr mulExpr_;
	struct SqlExprState {
		sql::ValueExpr addExpr;
		sql::ValueExpr mulExpr;
	};
	vector<SqlExprState> sqlExprStack_;
	sql::from_items_t fromItems_;
	vector<sql::Predicate> predicates_;
	sql::Predicate sqlPredicate_;
	vector<sql::SQLStatement> sqlStatements_;

	// Saved outer predicate context for WHERE scalar subquery parsing.
	// When entering a nested SELECT (inside a WHERE subquery), the inner WHERE
	// parsing would overwrite sqlPredicate_. We save and restore it here.
	struct SubqueryPredicateContext {
		sql::Predicate predicate;
		Binop binop;
	};
	vector<SubqueryPredicateContext> subqueryPredicateContextStack_;

	string sqlExportFilePath_;

	// Stack of outer WHERE contexts saved when entering a parenthesized WHERE group.
	// onSQLWhereGroupBegin() pushes the current WHERE onto this stack and starts a
	// fresh one; onSQLWhereGroupEnd() pops it back and wraps the inner WHERE as a group.
	vector<sql::Where> whereContextStack_;

};

}
