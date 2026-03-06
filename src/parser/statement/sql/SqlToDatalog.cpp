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
#include "bumblebee/parser/statement/sql/SqlToDatalog.hpp"

#include <set>
#include "CLI11.hpp"
#include "bumblebee/common/Constants.hpp"
#include "bumblebee/common/StringUtils.hpp"
#include "bumblebee/function/PredFunction.hpp"

namespace bumblebee{

string SQLQuery::generatePredicateName() {
    string result = PRED_PREFIX;
    result += "_";
    return result + std::to_string(counter_++);
}

string SQLQuery::generateVarName() {
    string result = VAR_PREFIX;
    return result + std::to_string(counter_++);
}

string SQLQuery::getVariableName(const string &table, const string &col) {
    return col+"."+table;
}

SqlQueryNormalizer::SqlQueryNormalizer(SQLQuery &query): query_(query) {
}

void SqlQueryNormalizer::setOuterColumnsMap(
    const std::unordered_map<string, std::unordered_set<string>>& outerMap) {
    outerColumnsMap_ = &outerMap;
}

void SqlQueryNormalizer::normalize() {
    assignAliasesAndCollectColumns(query_.statement_, true);
    expandSelectStars(query_.statement_);
    validateGroupBy(query_.statement_);
    removeUnusedCols(query_.statement_);
}


vector<string> getColsFromExtTable(sql::FromItem &item, ClientContext& context_) {
    BB_ASSERT(item.getType() == sql::EXTERNAL);
    vector<LogicalType> types;
    for (auto& param: item.getInputValues())
        types.emplace_back(param.getPhysicalType());
    auto pfunc = (PredFunction*) context_.functionRegister_.getFunction(item.getExtTableName(), types ).get();
    TableFilterSet filters;
    vector<LogicalType> returnTypes;
    vector<string> names = {"*"};
    std::unordered_map<string, idx_t> bindVarName;
    pfunc->bindFunction_(context_, item.getInputValues(), types, item.getNamedParameters(), bindVarName, returnTypes, names, filters);
    return names;
}


vector<sql::QualifiedName> getAllQualifiedNames(sql::SQLStatement& statement, std::unordered_map<string, std::unordered_set<string>> tableColumnsMap) {
    std::unordered_set<string> aliasNames;
    for (auto& f: statement.getFrom().getSubQueries())
        aliasNames.insert(f.getAlias());
    for (auto& f: statement.getFrom().getItems())
        aliasNames.insert(f.getAlias());

    vector<sql::QualifiedName> result;
    for (auto& alias: aliasNames) {
        BB_ASSERT(!alias.empty());
        if (!tableColumnsMap.contains(alias)) continue;
        for (auto&col : tableColumnsMap[alias]) {
            result.push_back({.name_ = col, .table_ = alias});
        }
    }
    return result;
}

void fillQTable(sql::ValuePrimary& vi, std::unordered_map<string, vector<string>>& colsTableMap, string& errorMessage) {
    if (vi.isSubExpr()) {
        for (auto& inner: vi.getSubExpr().getValues())
            fillQTable(inner, colsTableMap, errorMessage);
        return;
    }
    if (vi.isIsConstant())return;
    auto& q = vi.getQualifier();
    if (!q.name_.empty() && q.table_.empty()) {
        if (colsTableMap[q.name_].size() != 1) {
            errorMessage = "Column " + q.name_ + " does not exist or is in conflict with other tables.";
            return;
        }
        q.table_ = colsTableMap[q.name_][0];
    }
}

// Recursively fill table qualifiers for all predicates inside a WhereItem.
static void fillWhereQTable(sql::WhereItem& item,
    std::unordered_map<string, vector<string>>& colsTableMap, string& errorMessage) {
    std::visit([&](auto& wi) {
        using T = std::decay_t<decltype(wi)>;
        if constexpr (std::is_same_v<T, sql::Predicate>) {
            for (auto& vi: wi.getValue1().getValues())
                fillQTable(vi, colsTableMap, errorMessage);
            for (auto& vi: wi.getValue2().getValues())
                fillQTable(vi, colsTableMap, errorMessage);
        } else if constexpr (std::is_same_v<T, sql::SubqueryPredicate>) {
            // Fill table qualifier for outer LHS column only.
            // The subquery's internal columns are handled during inner normalization.
            for (auto& vi: wi.value_.getValues())
                fillQTable(vi, colsTableMap, errorMessage);
        } else if constexpr (std::is_same_v<T, sql::InPredicate>) {
            // Fill table qualifier for outer LHS column and constant list.
            // The subquery's internal columns are handled during inner normalization.
            for (auto& vi: wi.value_.getValues())
                fillQTable(vi, colsTableMap, errorMessage);
            for (auto& ve: wi.values_)
                for (auto& vi: const_cast<sql::ValueExpr&>(ve).getValues())
                    fillQTable(vi, colsTableMap, errorMessage);
        } else { // sql::WhereGroup
            for (auto& inner: wi.getWhere().getItems())
                fillWhereQTable(inner, colsTableMap, errorMessage);
        }
    }, item);
}

void SqlQueryNormalizer::assignAliasesAndCollectColumns(sql::SQLStatement& statement, bool isTopLevel) {
    // process from items
    // register the columns of the tables
    // and create alias if is empty
    // register also the select itesm as columns of statment

    if (result_.foundAnError())return;
    // first process sub queries
    for (auto& sq: statement.getFrom().getSubQueries()) {
        assignAliasesAndCollectColumns(sq);
    }

    std::unordered_map<string, vector<string>> colsTableMap;
    for (auto& fi: statement.getFrom().getItems()) {
        if (fi.getType() == sql::FromItemType::PREDICATE_TABLE) {
            // Determine columns in order
            vector<string> cols;
            if (!fi.getPredicateColumns().empty()) {
                cols = fi.getPredicateColumns();
            } else {
                int arity = fi.getPredicateArity();
                if (arity < 0) {
                    // Look up in schema — collect all predicates matching name
                    string predName = fi.getTableName();
                    std::set<unsigned> arities;
                    for (auto* p : query_.context_.defaultSchema_.getPredicates())
                        if (string(p->getName()) == predName)
                            arities.insert(p->getArity());
                    if (arities.empty()) {
                        result_.errorMessage_ = "Predicate '" + predName + "' not found in schema.";
                        return;
                    }
                    if (arities.size() > 1) {
                        result_.errorMessage_ = "Predicate '" + predName + "' is ambiguous: multiple arities exist. Use '" + predName + "/N' to specify arity.";
                        return;
                    }
                    arity = (int)*arities.begin();
                }
                // Store the resolved arity back so generatePredicateTableAtom
                // can produce the correct number of terms even after column pruning.
                fi.setPredicateArity(arity);
                for (int j = 1; j <= arity; ++j)
                    cols.push_back("V" + std::to_string(j));
            }
            // Assign alias if empty
            if (fi.getAlias().empty())
                fi.setAlias(query_.generatePredicateName());
            if (query_.tableColumnsMap_.contains(fi.getAlias())) {
                result_.errorMessage_ = "Table " + fi.getAlias() + " already exists.";
                return;
            }
            for (auto& col : cols) {
                colsTableMap[col].push_back(fi.getAlias());
                query_.tableColumnsMap_[fi.getAlias()].insert(col);
            }
            continue;
        }
        if (fi.getType() != sql::FromItemType::EXTERNAL) {
            result_.errorMessage_ = "Only  ext table are supported :(";
            return;
        }
        if (fi.getAlias().empty())
            // generate alias
            fi.setAlias(query_.generatePredicateName());

        auto columns = getColsFromExtTable(fi, query_.context_);
        if (query_.tableColumnsMap_.contains(fi.getAlias())) {
            result_.errorMessage_ = "Table "+ fi.getAlias() + " already exists.";
            return;
        }
        for (auto& col: columns) {
            colsTableMap[col].push_back(fi.getAlias());
            query_.tableColumnsMap_[fi.getAlias()].insert(col);
        }
    }
    for (auto& sq: statement.getFrom().getSubQueries())
        for (auto& col: query_.tableColumnsMap_[sq.getAlias()])
            colsTableMap[col].push_back(sq.getAlias());

    // If an outer-scope column map is provided (correlated subquery), add outer columns
    // as fallback for any column name not already resolved from the inner FROM tables.
    // This is only applied at the root statement level of this normalizer to avoid
    // polluting nested sub-query resolution.
    if (outerColumnsMap_ && &statement == &query_.statement_) {
        for (auto& [alias, cols]: *outerColumnsMap_)
            for (auto& col: cols)
                if (!colsTableMap.count(col))
                    colsTableMap[col].push_back(alias);
    }

    if (statement.getAlias().empty()) {
        if (isTopLevel)
            statement.setAlias(QUERY_RESULT_PRED_NAME);
        else
            statement.setAlias(query_.generatePredicateName());
    }

    // fill the alias and table information
    for (auto& ve: statement.getSelect().getItems()) {
        if (ve.getAlias().empty()) {
            auto alias = query_.generateVarName();
            if (ve.getValues().size() == 1) {
                // take as alias the name of the variable if is not constant
                auto& vp = ve.getValues()[0];
                if (!vp.isSubExpr() && !vp.isIsConstant()) {
                    auto& name = vp.getQualifier().name_;
                    if (colsTableMap.contains(name) && colsTableMap[name].size() == 1)
                        alias = name;
                    else
                        alias = vp.toString();
                }
            }
            ve.setAlias(alias);
        }
        // se the table information if empty
        for (auto& vi: ve.getValues())
            fillQTable(vi, colsTableMap, result_.errorMessage_);

        query_.tableColumnsMap_[statement.getAlias()].insert(ve.getAlias());
    }
    // fill the where values (recurse into WhereGroups)
    for (auto& item: statement.getWhere().getItems()) {
        fillWhereQTable(item, colsTableMap, result_.errorMessage_);
    }

    // Build a map from SELECT alias -> actual column qualifier so that GROUP BY
    // clauses can reference SELECT aliases (e.g. GROUP BY SUPPLIER_NO where
    // SUPPLIER_NO is an alias for L_SUPPKEY).
    std::unordered_map<string, sql::QualifiedName> selectAliasMap;
    for (auto& ve: statement.getSelect().getItems()) {
        if (!ve.getAlias().empty() && ve.getValues().size() == 1
                && !ve.getValues()[0].isSubExpr() && !ve.getValues()[0].isIsConstant()) {
            auto& q = ve.getValues()[0].getQualifier();
            if (!q.table_.empty() && ve.getAlias() != q.name_)
                selectAliasMap[ve.getAlias()] = q;
        }
    }

    // fill the groupo by values
    for (auto& ve: statement.getGroupby().getItems()) {
        // se the table information if empty
        for (auto& vi: ve.getValues()) {
            auto& q = vi.getQualifier();
            if (!vi.isSubExpr() && !vi.isIsConstant() && q.table_.empty()
                    && selectAliasMap.count(q.name_)) {
                // GROUP BY references a SELECT alias; replace with the actual column qualifier
                vi.setQualifier(selectAliasMap[q.name_]);
            } else {
                fillQTable(vi, colsTableMap, result_.errorMessage_);
            }
        }
    }

}

void SqlQueryNormalizer::expandSelectStars(sql::SQLStatement& statement) {
    if (result_.foundAnError())return;
    // replace star with all the columns of tables

    // first process sub queries
    for (auto& sq: statement.getFrom().getSubQueries()) {
        expandSelectStars(sq);
    }
    // check if contains * and is only one *
    idx_t star = 0;
    auto& items = statement.getSelect().getItems();
    auto &aggs = statement.getSelect().getAggFunctions();
    for (idx_t i = 0; i < items.size(); i++) {
        if (aggs[i] == NONE && items[i].toString(false) == "*")
            ++star;
    }

    if (!star)
        return;
    if (star > 1) {
        // error we found 2 star
        result_.errorMessage_ = "Error, found a start with other select items, if you want to use * please do not specify additional select items.";
        return;
    }

    // we found a star, find all the cols of the tables/subqueries and add as select items
    items.clear();
    auto qualifiedNames = getAllQualifiedNames(statement, query_.tableColumnsMap_);
    for (auto& qualified: qualifiedNames) {
        sql::ValueExpr ve(qualified);
        ve.setAlias(qualified.table_+"."+qualified.name_);
        statement.getSelect().addItem(ve, NONE);
    }

}

void SqlQueryNormalizer::validateGroupBy(sql::SQLStatement& statement) {
    // checks if the select items the not aggregate columns are present in the group by
    if (result_.foundAnError())return;
    if (statement.getGroupby().isEmpty()) return;
    for (auto& sq: statement.getFrom().getSubQueries()) {
        validateGroupBy(sq);
    }
    std::unordered_set<string> groups;
    idx_t i = 0;
    // if group table information of group col replace with correct table name
    for (auto& q: statement.getGroupby().getQualifiedNames()) {
        if (q.table_.empty()) {

            statement.getGroupby().setQualifiedNames(q, i);
        }
        groups.insert(q.table_+"."+q.name_);
        ++i;
    }
    auto& aggs = statement.getSelect().getAggFunctions();
    for (i=0;i<aggs.size();++i) {
        if (aggs[i] != NONE) continue;
        // non aggregate column, check if exist in the groups
        auto &ve = statement.getSelect().getItems()[i];
        BB_ASSERT(!ve.getAlias().empty());

        if (ve.getValues().size() > 1) {
            result_.errorMessage_ = "Group by with expression not supported: " + ve.toString();
            return;
        }
        auto& vp = ve.getValues()[0];
        if (vp.isIsConstant()) continue;

        auto& q= vp.getQualifier();
        BB_ASSERT(!q.table_.empty());

        if (!groups.contains(q.table_+"."+q.name_)) {
            result_.errorMessage_ = "Column " + q.name_ + " does not belong in groups column.";
            return;
        }
    }
}

// Collect all bare column names referenced inside a subquery's WHERE clause
// (recursing into nested SubqueryPredicates).  These are the candidate correlated
// references that must be kept in the outer tableColumnsMap_ even though they do
// not appear at the outer query level.
static void collectInnerSubqueryColNames(sql::SQLStatement& stmt, std::unordered_set<string>& names) {
    for (auto& item: stmt.getWhere().getItems()) {
        std::visit([&](auto& wi) {
            using T = std::decay_t<decltype(wi)>;
            if constexpr (std::is_same_v<T, sql::Predicate>) {
                for (auto& vp: wi.getValue1().getValues())
                    if (!vp.isIsConstant() && !vp.isSubExpr())
                        names.insert(vp.getQualifier().name_);
                for (auto& vp: wi.getValue2().getValues())
                    if (!vp.isIsConstant() && !vp.isSubExpr())
                        names.insert(vp.getQualifier().name_);
            } else if constexpr (std::is_same_v<T, sql::SubqueryPredicate>) {
                collectInnerSubqueryColNames(*wi.subquery_, names);
            } else if constexpr (std::is_same_v<T, sql::InPredicate>) {
                // Collect LHS column name (may be a correlated outer reference).
                for (auto& vp: wi.value_.getValues())
                    if (!vp.isIsConstant() && !vp.isSubExpr())
                        names.insert(vp.getQualifier().name_);
                if (wi.subquery_)
                    collectInnerSubqueryColNames(*wi.subquery_, names);
            }
            // WhereGroup: groups are only at the outer level; ignore for now.
        }, item);
    }
}

void SqlQueryNormalizer::removeUnusedCols(sql::SQLStatement &statement) {
    // Recursively prune sub-queries first (bottom-up) so each level only sees
    // columns relevant to its own SELECT/WHERE/GROUP BY.
    for (auto& sq: statement.getFrom().getSubQueries()) {
        removeUnusedCols(sq);
    }

    auto queryQualifiedNames = statement.getQualifiedNames();
    std::unordered_set<string> usedCols;
    for (auto& q: queryQualifiedNames)
        usedCols.insert(q.table_+"."+q.name_);

    // Collect bare column names from inner subquery WHEREs so that correlated
    // outer-scope columns are not erroneously pruned (they appear only inside
    // the subquery at this point, not at the outer predicate level).
    std::unordered_set<string> innerSubqueryColNames;
    for (auto& item: statement.getWhere().getItems()) {
        if (auto* sp = std::get_if<sql::SubqueryPredicate>(&item))
            collectInnerSubqueryColNames(*sp->subquery_, innerSubqueryColNames);
        else if (auto* ip = std::get_if<sql::InPredicate>(&item))
            if (ip->subquery_)
                collectInnerSubqueryColNames(*ip->subquery_, innerSubqueryColNames);
    }

    // Only prune tables that are directly in THIS statement's FROM clause.
    // Sub-query result aliases (e.g. SHIPPING) and inner tables (N1, N2, lineitem,
    // etc.) that belong to sub-queries are handled by the recursive calls above.
    std::unordered_set<string> directTables;
    for (auto& fi: statement.getFrom().getItems())
        directTables.insert(fi.getAlias());
    for (auto& sq: statement.getFrom().getSubQueries())
        directTables.insert(sq.getAlias());

    for (auto& [table, cols]: query_.tableColumnsMap_) {
        if (!directTables.contains(table)) continue;
        for (auto it = cols.begin(); it != cols.end(); ) {
            if (cols.size() <= 1) break; // keep at least one column
            if (usedCols.contains(table + "." + *it) || innerSubqueryColNames.contains(*it)) {
                ++it;
            } else {
                it = cols.erase(it); // erase returns the next valid iterator
            }
        }
    }
}


void DatalogGenerator::generate() {
    generateRules(query_.statement_);
    if (result_.foundAnError()) return;

    if (!result_.rules_.empty()) {
        // Mark all intermediate predicates as internal
        for (size_t i = 0; i + 1 < result_.rules_.size(); ++i) {
            for (auto& headAtom : result_.rules_[i].getHead()) {
                headAtom.getPredicate()->setInternal(true);
            }
        }

        // The last rule contains the answer so set the predicate as non internal
        if (!query_.statement_.getExportPath().empty()) {
            generateExportRule(query_.statement_);
        } else {
            auto& program = result_.rules_;
            BB_ASSERT(program.back().getHead().size() == 1);
            program.back().getHead()[0].getPredicate()->setInternal(false);
        }
    }
}

Atom generateAtomFromTable(string& alias, SQLQuery& query, string& errorMessage ) {
    // generate an atom with all the variables as columns in the table
    BB_ASSERT(!alias.empty());
    BB_ASSERT(query.tableColumnsMap_.contains(alias));
    vector<Term> terms;
    for (auto& col: query.tableColumnsMap_[alias]) {
        auto t = Term::createVariable(query.getVariableName(alias, col));
        terms.push_back(std::move(t));
    }
    auto pred = query.context_.defaultSchema_.createPredicate(&query.context_, alias.c_str(), terms.size());
    return Atom::createClassicalAtom(pred, std::move(terms));
}

Atom generateFirstExtAtomFromTable(sql::FromItem& fi, SQLQuery& query, string& errorMessage ) {
    // do not generate aux rule for first atom
    vector<Term> terms;
    string columnMappingValue;
    for (auto& col: query.tableColumnsMap_[fi.getAlias()]) {
        BB_ASSERT(!col.empty());
        auto t = Term::createVariable(query.getVariableName(fi.getAlias(), col));
        columnMappingValue += t.toString()+":" + col+ ";";
        terms.push_back(std::move(t));
    }
    if (!columnMappingValue.empty())
        columnMappingValue.pop_back();
    // add the columns_mapping parameter
    auto& namedParams = fi.getNamedParameters();
    namedParams["columns_mapping"] = columnMappingValue;
    auto extAtom = Atom::createExternalAtom(fi.getNamedParameters(), fi.getInputValues(), fi.getExtTableName(), std::move(terms));
    return extAtom;
}

void generateRuleForExtAtom( sql::FromItem& item, SQLQuery& query, TranslationResult& result) {
    BB_ASSERT(item.getType() == sql::FromItemType::EXTERNAL);

    vector<Term> terms;
    for (auto& col: query.tableColumnsMap_[item.getAlias()]) {
        // do not change the var name otherwise the external atom does not match with the var name
        // for example with csv reader var match the column name
        BB_ASSERT(!col.empty());
        auto t = Term::createVariable(col.c_str());
        terms.push_back(std::move(t));
    }
    auto extAtom = Atom::createExternalAtom(item.getNamedParameters(), item.getInputValues(), item.getExtTableName(), std::move(terms));

    terms = extAtom.getTerms();
    string alias = item.getAlias();
    BB_ASSERT(!alias.empty());
    auto pred = query.context_.defaultSchema_.createPredicate(&query.context_, alias.c_str(), terms.size());
    auto head = Atom::createClassicalAtom(pred, std::move(terms));
    Rule rule(head, extAtom);
    result.rules_.push_back(std::move(rule));
}

Atom generateAtomGenId(const char* idVar) {
    vector<Term> idTerms;
    auto t = Term::createVariable(idVar);
    idTerms.push_back(std::move(t));
    string func = "&gen_id";
    std::unordered_map<string, Value> namedParams;
    vector<Value> inputValues;
    return Atom::createExternalAtom(namedParams, inputValues, func, std::move(idTerms));
}


Term generateTermFromValueExpr(sql::ValueExpr& ve, const string& sqlAlias, SQLQuery &query, string& errorMessage) {

    vector<Term> terms;
    for (auto& vp: ve.getValues()) {
        if (vp.isSubExpr()) {
            Term subTerm = generateTermFromValueExpr(vp.getSubExpr(), sqlAlias, query, errorMessage);
            if (subTerm.getType() == ARITH)
                subTerm.setParenthesized(true);
            terms.push_back(std::move(subTerm));
        } else if (!vp.isIsConstant()) {
            BB_ASSERT(!vp.getQualifier().table_.empty());
            // generate the variable using the table and name
            string var = query.getVariableName(vp.getQualifier().table_, vp.getQualifier().name_);
            terms.push_back(Term::createVariable(std::move(var)));
        }else
            terms.emplace_back(vp.getValue());
    }

    Term result;
    if (terms.size() == 1) {
        result = std::move( terms.front());
    }else {
        // arith term
        BB_ASSERT(!ve.getValues().empty());
        BB_ASSERT(ve.getValues().size() == ve.getOperators().size() +1);
        auto t = Term::createArith(std::move(terms[0]),std::move(terms[1]), ve.getOperators()[0] );
        for (idx_t i = 1;i < ve.getOperators().size();i++) {
            t.addInArithTerm(std::move(terms[i+1]), ve.getOperators()[i] );
        }
        result = std::move(t);
    }

    return result;
}

Atom generateBuiltinFromPredCondition(sql::Predicate &predicate, SQLQuery& query, string& errorMessage) {
    vector<Term> terms;
    auto t1 = generateTermFromValueExpr(predicate.getValue1(),"", query, errorMessage);
    auto t2 = generateTermFromValueExpr(predicate.getValue2(),"",query, errorMessage);
    terms.push_back(std::move(t1));
    terms.push_back(std::move(t2));
    return Atom::createBuiltinAtom(std::move(terms), sql::toCoreBinop(predicate.getOp()));
}

Atom generateLikeAtom(sql::Predicate& predicate, SQLQuery& query, string& errorMessage) {
    auto colTerm = generateTermFromValueExpr(predicate.getValue1(), "", query, errorMessage);

    auto& patternVals = predicate.getValue2().getValues();
    if (patternVals.size() != 1 || !patternVals.front().isIsConstant()) {
        errorMessage = "LIKE operator requires a constant string pattern on the right side";
        return Atom{};
    }

    vector<Value> inputValues;
    inputValues.push_back(patternVals.front().getValue().clone());
    std::unordered_map<string, Value> namedParams;
    terms_vector_t terms;
    terms.push_back(std::move(colTerm));
    string funcName = "&like";
    return Atom::createExternalAtom(namedParams, inputValues, funcName, std::move(terms));
}

Rule generateAggRules(const std::unordered_set<string>& groupVars, const std::unordered_map<idx_t, vector<string>>& aggVars,
        SQLQuery& query, sql::SQLStatement& statement, Rule& rule, string& headPredicateName,
        rules_vector_t& additionalRules,  string& errorMessage) {

    auto& select = statement.getSelect();
    auto& headLastRule =  rule.getHead()[0];
    vector<Term> assignmentTerms;
    vector<AggregateFunctionType> aggFunctions;
    vector<Term> aggTerms;
    for (auto& [agg, vars]: aggVars) {
        assignmentTerms.push_back(Term::createVariable(select.getItems()[agg].getAlias()));
        aggFunctions.push_back(select.getAggFunctions()[agg]);
        // add all vars as aggregate terms (multiple for COUNT(*) with cross products)
        for (auto& v : vars)
            aggTerms.push_back(Term::createVariable(v.c_str()));
    }
    // let's add the ID to avoid the distinct calculation
    auto t = Term::createVariable(query.ID_VAR);
    aggTerms.push_back(std::move(t));

    vector<Atom> aggBodyAtoms;
    for (auto& atom:rule.getBody())
        aggBodyAtoms.push_back(atom.clone());

    vector<Term> aggGroupTerms;
    for (auto& var: groupVars)
        aggGroupTerms.push_back(Term::createVariable(var.c_str()));

    BB_ASSERT(aggFunctions.size() > 0);
    BB_ASSERT(aggFunctions.size() == assignmentTerms.size());

    Atom aggregateAtom ;
    if (aggFunctions.size() > 1)
        aggregateAtom = Atom::createMultiAggregateAtom(std::move(aggFunctions), std::move(assignmentTerms),
        std::move(aggTerms), std::move(aggBodyAtoms), std::move(aggGroupTerms));
    else  {
        Term ug;
        aggregateAtom = Atom::createAggregateAtom(aggFunctions[0], Binop::ASSIGNMENT, Binop::NONE_OP,
            assignmentTerms[0], ug, std::move(aggTerms),
            std::move(aggBodyAtoms), std::move(aggGroupTerms));
    }

    // calculate the terms in the head
    vector<Term> headTerms;
    idx_t i = 0;
    for (auto& ve: statement.getSelect().getItems()) {
        if (statement.getSelect().getAggFunctions()[i] != NONE) {
            // insert the alias for aggregations
            auto var = ve.getAlias();
            BB_ASSERT(!var.empty());
            headTerms.push_back(Term::createVariable(std::move(var)));
        }else {
            // create the term from value
            auto t = generateTermFromValueExpr(ve, statement.getAlias(), query, errorMessage);
            headTerms.push_back(std::move(t));
        }
        ++i;
    }
    auto newPred = query.context_.defaultSchema_.createPredicate(&query.context_, headPredicateName.c_str(), headTerms.size());
    Atom head = Atom::createClassicalAtom(newPred, std::move(headTerms));

    Rule newRule(head, aggregateAtom);
    return newRule;

}


void generateOrderRule(sql::SQLStatement &statement,Rule& rule, string& errorMessage) {
    if (!errorMessage.empty()) return;

    auto& group = statement.getOrderby();
    if (group.empty())return;

    rule.setLimit(statement.getLimit());

    rule.setModifiers(group.getColModifiers());
    if (rule.getLimit() == 0) {
        errorMessage =  "Unsupported order by directive without a limit. Please add a #limit directive.";
    }
}


void getAggregatesInformation(sql::SQLStatement &statement, SQLQuery& query,
    string& errorMessage, vector<Atom>& body, vector<Term>& headTerms, std::unordered_set<string>& headVars,
    std::unordered_set<string>& groupVars, std::unordered_map<idx_t, vector<string>>& aggVars) {


    for (idx_t i = 0;i< statement.getSelect().getItems().size();++i) {
        auto si = statement.getSelect().getItems()[i];
        if (si.toString(false) == "*" && statement.getSelect().getAggFunctions()[i] == COUNT) {

            // we can count on all the columns, so just pick a column used in the select
            string var = "";
            for (idx_t idx=0;idx< statement.getSelect().getItems().size();++idx) {
                auto ve = statement.getSelect().getItems()[idx];
                if (ve.toString(false) != "*") {
                    for (auto& vp: ve.getValues()) {
                        if (vp.isSubExpr() || vp.isIsConstant()) continue;
                        BB_ASSERT(!vp.getQualifier().table_.empty());
                        var = query.getVariableName(vp.getQualifier().table_, vp.getQualifier().name_);
                        break;
                    }
                }
            }

            if (var.empty()) {
                // COUNT(*) with no other select items: pick one variable per FROM table
                // so the aggregate auxiliary rule head includes all tables, preserving
                // cross products (otherwise the VariablesRewriter removes unused tables).
                auto qNames = getAllQualifiedNames(statement, query.tableColumnsMap_);
                BB_ASSERT(!qNames.empty());
                std::unordered_set<string> seenTables;
                for (auto& qn : qNames) {
                    if (seenTables.contains(qn.table_)) continue;
                    seenTables.insert(qn.table_);
                    auto v = query.getVariableName(qn.table_, qn.name_);
                    if (!headVars.contains(v)) {
                        headVars.insert(v);
                        headTerms.push_back(Term::createVariable(v.c_str()));
                    }
                    aggVars[i].push_back(v);
                }
            } else {
                aggVars[i].push_back(var);
            }
            continue;
        }
        auto t = generateTermFromValueExpr( si, statement.getAlias(), query, errorMessage);
        if (t.getType() == ARITH) {
            // arith term, put in the head the variable and in the body a builtin atom
            auto varTerm = Term::createVariable(query.generateVarName());
            vector builtInTerms = {varTerm, t};
            auto atom = Atom::createBuiltinAtom(std::move(builtInTerms), Binop::ASSIGNMENT);
            body.push_back(std::move(atom));
            t = varTerm;
        }
        BB_ASSERT(t.getType() == VARIABLE);

        if (statement.getSelect().getAggFunctions()[i] != NONE) {
            // aggregation
            aggVars[i].push_back(t.getVariable());
        }

        if (headVars.contains(t.getVariable()))
            continue;
        headVars.insert(t.getVariable());
        headTerms.push_back(std::move(t));
    }

    // Ensure all FROM tables have at least one variable in aggregate terms
    // to preserve cross products in the auxiliary rule.
    if (!aggVars.empty() && statement.getFrom().getItems().size() > 1) {
        // Collect tables already represented in aggVars
        std::unordered_set<string> representedTables;
        for (auto& [_, vars] : aggVars)
            for (auto& v : vars)
                for (auto& [table, cols] : query.tableColumnsMap_)
                    for (auto& col : cols)
                        if (query.getVariableName(table, col) == v)
                            representedTables.insert(table);

        // Add one variable from each missing FROM table
        auto qNames = getAllQualifiedNames(statement, query.tableColumnsMap_);
        for (auto& qn : qNames) {
            if (representedTables.contains(qn.table_)) continue;
            representedTables.insert(qn.table_);
            auto v = query.getVariableName(qn.table_, qn.name_);
            aggVars.begin()->second.push_back(v);
        }
    }

    // add the group by cols in head terms
    for (auto& q: statement.getGroupby().getQualifiedNames()) {
        auto var = query.getVariableName(q.table_, q.name_);
        groupVars.insert(var);
        if (headVars.contains(var)) continue;
        auto t = Term::createVariable(std::move(var));
        headVars.insert(t.getVariable());
        headTerms.push_back(std::move(t));
    }
}

vector<vector<Atom>> toBinopAtomsCnf(vector<Atom>& atoms) {
    vector<vector<Atom>> result;
    for (auto& a: atoms) {
        result.emplace_back();
        result.back().push_back(std::move(a));
    }
    return result;
}

vector<vector<Atom>> distributeBinopAtoms(vector<vector<Atom>>& cnf1,vector<vector<Atom>>& cnf2) {
    vector<vector<Atom>> result;
    for (auto& c1: cnf1) {
        for (auto& c2: cnf2) {
            result.emplace_back();
            for (auto& a:c1) result.back().push_back(a.clone());
            for (auto& a:c2) result.back().push_back(a.clone());
        }
    }
    return result;
}

using CNF = vector<vector<Atom>>;

// ---- Forward declarations ----

Atom generatePredicateTableAtom(sql::FromItem& fi, SQLQuery& query, string& errorMessage);

// ---- Forward declarations for mutually recursive subquery helpers ----

static vector<Atom> generateInnerBodyFromSubquery(
    sql::SQLStatement& inner, SQLQuery& query, TranslationResult& result, string& errorMessage);

static vector<Atom> generateWhereSubqueryAtoms(
    sql::SubqueryPredicate& sp, SQLQuery& query, TranslationResult& result, string& errorMessage);

static vector<Atom> generateWhereInSubqueryAtoms(
    sql::InPredicate& ip, SQLQuery& query, TranslationResult& result, string& errorMessage);

static CNF generateWhereListCNF(sql::predicate_vector_t& items, vector<sql::SQLOperator>& ops,
    bool likeForbidden, vector<Atom>& likeBody, SQLQuery& query, TranslationResult& result, string& errorMessage);

void generateBinopAtoms(vector<Atom>& body, sql::predicate_vector_t& items, vector<sql::SQLOperator>& ops,
    SQLQuery& query, TranslationResult& result, string& errorMessage);

static CNF generateInListCNF(sql::InPredicate& ip, SQLQuery& query, string& errorMessage);

// ---- Subquery validation ----

static void validateWhereSubquery(sql::SQLStatement& inner, string& errorMessage) {
    if (inner.getSelect().getItems().size() != 1) {
        errorMessage = "Scalar subquery in WHERE must return exactly one column";
        return;
    }
    if (inner.getSelect().getAggFunctions()[0] == NONE) {
        errorMessage = "Scalar subquery in WHERE must use an aggregate function (MAX, MIN, AVG, SUM, COUNT)";
        return;
    }
}

// ---- Inner body generation (normalizes inner subquery and builds FROM+WHERE body atoms) ----

static vector<Atom> generateInnerBodyFromSubquery(
    sql::SQLStatement& inner, SQLQuery& outerQuery, TranslationResult& result, string& errorMessage) {

    // Create an inner SQLQuery sharing the outer counter to avoid name conflicts.
    SQLQuery innerQuery(inner, outerQuery.context_);
    innerQuery.counter_ = outerQuery.counter_;

    SqlQueryNormalizer normalizer(innerQuery);
    // Provide the outer scope so that correlated column references in the inner WHERE
    // (e.g. P_PARTKEY from an outer "part" table) are resolved correctly.
    // Use only FROM table aliases — NOT the outer SELECT result alias — to prevent
    // ambiguity when a SELECT column has the same name as a FROM table column.
    std::unordered_map<string, std::unordered_set<string>> outerFromMap;
    for (auto& fi: outerQuery.statement_.getFrom().getItems()) {
        const auto& alias = fi.getAlias();
        auto it = outerQuery.tableColumnsMap_.find(alias);
        if (it != outerQuery.tableColumnsMap_.end())
            outerFromMap[alias] = it->second;
    }
    for (auto& sq: outerQuery.statement_.getFrom().getSubQueries()) {
        const auto& alias = sq.getAlias();
        auto it = outerQuery.tableColumnsMap_.find(alias);
        if (it != outerQuery.tableColumnsMap_.end())
            outerFromMap[alias] = it->second;
    }
    normalizer.setOuterColumnsMap(outerFromMap);
    normalizer.normalize();
    if (normalizer.result_.foundAnError()) {
        errorMessage = normalizer.result_.errorMessage_;
        return {};
    }

    // Sync counter back and merge tableColumnsMap so outer can use inner alias names.
    outerQuery.counter_ = innerQuery.counter_;
    for (auto& [alias, cols]: innerQuery.tableColumnsMap_)
        outerQuery.tableColumnsMap_[alias] = cols;

    // Generate FROM body atoms.
    vector<Atom> body;

    // Handle derived tables (subqueries) in the inner FROM clause,
    // mirroring the logic in DatalogGenerator::generateRules.
    for (auto& sq: inner.getFrom().getSubQueries()) {
        DatalogGenerator subqGen(outerQuery);
        subqGen.generateRules(sq);
        if (subqGen.result_.foundAnError()) {
            errorMessage = subqGen.result_.errorMessage_;
            return {};
        }
        for (auto& r: subqGen.result_.rules_)
            result.rules_.push_back(std::move(r));

        string sqAlias = sq.getAlias();
        BB_ASSERT(outerQuery.tableColumnsMap_.contains(sqAlias));
        vector<Term> sqTerms;
        for (auto& item: sq.getSelect().getItems()) {
            const auto& colAlias = item.getAlias();
            auto t = Term::createVariable(outerQuery.getVariableName(sqAlias, colAlias));
            sqTerms.push_back(std::move(t));
        }
        auto sqPred = outerQuery.context_.defaultSchema_.createPredicate(
            &outerQuery.context_, sqAlias.c_str(), sqTerms.size());
        body.push_back(Atom::createClassicalAtom(sqPred, std::move(sqTerms)));
    }

    idx_t tableIdx = 0;
    for (auto& fi: inner.getFrom().getItems()) {
        if (fi.getType() == sql::FromItemType::PREDICATE_TABLE) {
            auto atom = generatePredicateTableAtom(fi, outerQuery, errorMessage);
            if (!errorMessage.empty()) return {};
            body.push_back(std::move(atom));
        } else if (tableIdx == 0 && fi.getType() == sql::FromItemType::EXTERNAL) {
            auto extAtom = generateFirstExtAtomFromTable(fi, outerQuery, errorMessage);
            if (!errorMessage.empty()) return {};
            body.push_back(std::move(extAtom));
        } else if (fi.getType() == sql::FromItemType::EXTERNAL) {
            generateRuleForExtAtom(fi, outerQuery, result);
            if (result.foundAnError()) { errorMessage = result.errorMessage_; return {}; }
            string fiAlias = fi.getAlias();
            auto atom = generateAtomFromTable(fiAlias, outerQuery, errorMessage);
            if (!errorMessage.empty()) return {};
            body.push_back(std::move(atom));
        } else {
            errorMessage = "Only external tables are supported in subquery FROM clause";
            return {};
        }
        ++tableIdx;
    }

    // Process inner WHERE items.
    auto& whereItems = inner.getWhere().getItems();
    auto& whereOps   = inner.getWhere().getOps();
    for (auto& item: whereItems) {
        if (auto* pred = std::get_if<sql::Predicate>(&item)) {
            if (pred->getOp() == sql::SQL_LIKE) {
                auto atom = generateLikeAtom(*pred, outerQuery, errorMessage);
                if (!errorMessage.empty()) return {};
                body.push_back(std::move(atom));
            } else {
                auto atom = generateBuiltinFromPredCondition(*pred, outerQuery, errorMessage);
                if (!errorMessage.empty()) return {};
                body.push_back(std::move(atom));
            }
        } else if (auto* grp = std::get_if<sql::WhereGroup>(&item)) {
            // Delegate grouped conditions to the CNF machinery (without subquery support
            // inside groups — subqueries inside grouped conditions would need auxiliary rules,
            // which is not supported here).
            generateBinopAtoms(body, grp->getWhere().getItems(), grp->getWhere().getOps(),
                outerQuery, result, errorMessage);
            if (!errorMessage.empty()) return {};
        } else if (auto* ip2 = std::get_if<sql::InPredicate>(&item)) {
            if (ip2->subquery_) {
                auto atoms = generateWhereInSubqueryAtoms(*ip2, outerQuery, result, errorMessage);
                if (!errorMessage.empty()) return {};
                for (auto& atom: atoms)
                    body.push_back(std::move(atom));
            } else {
                auto cnf = generateInListCNF(*ip2, outerQuery, errorMessage);
                if (!errorMessage.empty()) return {};
                for (auto& clause: cnf) {
                    if (clause.empty()) continue;
                    if (clause.size() == 1)
                        body.push_back(std::move(clause[0]));
                    else
                        body.push_back(Atom::createOrBuiltinAtom(clause));
                }
            }
        } else if (auto* sp2 = std::get_if<sql::SubqueryPredicate>(&item)) {
            // Nested subquery: generate auxiliary predicate rule.
            validateWhereSubquery(*sp2->subquery_, errorMessage);
            if (!errorMessage.empty()) return {};

            // Recursively build the innermost body atoms.
            auto innermostBody = generateInnerBodyFromSubquery(*sp2->subquery_, outerQuery, result, errorMessage);
            if (!errorMessage.empty()) return {};

            // Build aggregate info from innermost select.
            auto& innermostSelect = sp2->subquery_->getSelect();
            AggregateFunctionType aggFunc = innermostSelect.getAggFunctions()[0];

            Term aggTerm;
            if (innermostSelect.getItems()[0].toString(false) == "*" && aggFunc == COUNT) {
                // COUNT(*): pick any column from the innermost table.
                auto qNames = getAllQualifiedNames(*sp2->subquery_, outerQuery.tableColumnsMap_);
                BB_ASSERT(!qNames.empty());
                auto var = outerQuery.getVariableName(qNames[0].table_, qNames[0].name_);
                aggTerm = Term::createVariable(std::move(var));
            } else {
                auto& si = innermostSelect.getItems()[0];
                aggTerm = generateTermFromValueExpr(const_cast<sql::ValueExpr&>(si),
                    sp2->subquery_->getAlias(), outerQuery, errorMessage);
                if (!errorMessage.empty()) return {};
                // Handle arithmetic-wrapped aggregate (e.g. 0.2 * avg(col))
                if (aggTerm.getType() == ARITH) {
                    auto arithVar = Term::createVariable(outerQuery.generateVarName());
                    terms_vector_t builtinTerms = {arithVar, std::move(aggTerm)};
                    innermostBody.push_back(Atom::createBuiltinAtom(std::move(builtinTerms), Binop::ASSIGNMENT));
                    aggTerm = arithVar;
                }
            }

            // Fresh guard variable for assignment in the auxiliary rule.
            string guardVarName = outerQuery.generateVarName();
            Term guardTerm = Term::createVariable(guardVarName.c_str());
            Term emptyGuard;
            terms_vector_t aggTerms;
            aggTerms.push_back(std::move(aggTerm));
            aggTerms.push_back(Term::createVariable(SQLQuery::ID_VAR));

            // Aggregate atom: guardVar = #agg{aggTerm, #ID : innermostBody}
            auto aggAtom = Atom::createAggregateAtom(aggFunc, Binop::EQUAL, Binop::NONE_OP,
                guardTerm, emptyGuard, std::move(aggTerms), std::move(innermostBody), {});

            // Create auxiliary predicate rule: #aux(guardVar) :- aggAtom
            string auxPredName = outerQuery.generatePredicateName();
            auto auxPred = outerQuery.context_.defaultSchema_.createPredicate(
                &outerQuery.context_, auxPredName.c_str(), 1);
            terms_vector_t auxHeadTerms = { Term::createVariable(guardVarName.c_str()) };
            auto auxHead = Atom::createClassicalAtom(auxPred, std::move(auxHeadTerms));
            Rule auxRule(auxHead, aggAtom);
            result.rules_.push_back(std::move(auxRule));

            // Add classical atom #aux(guardVar) to inner body (binds guardVar).
            terms_vector_t classTerms = { Term::createVariable(guardVarName.c_str()) };
            body.push_back(Atom::createClassicalAtom(auxPred, std::move(classTerms)));

            // Add builtin: LHS BINOP guardVar to enforce the comparison.
            auto lhsTerm = generateTermFromValueExpr(sp2->value_, "", outerQuery, errorMessage);
            if (!errorMessage.empty()) return {};
            terms_vector_t builtinTerms = { std::move(lhsTerm), Term::createVariable(guardVarName.c_str()) };
            body.push_back(Atom::createBuiltinAtom(std::move(builtinTerms), sql::toCoreBinop(sp2->op_)));
        }
    }

    // Validate that no top-level OR connects WHERE items in the inner subquery.
    // Each item is currently treated independently (equivalent to AND). OR at
    // this level would produce wrong results silently, so we guard it explicitly.
    for (auto& op: whereOps) {
        if (op == sql::SQL_OR) {
            errorMessage = "OR in scalar subquery WHERE clause is not yet supported";
            return {};
        }
    }

    return body;
}

// ---- Collect outer-scope (correlated) variables from inner WHERE predicates ----
// After inner normalization, correlated columns carry an outer-table alias suffix
// (e.g. "O_ORDERKEY.#p_0"). This function returns those variable names so they
// can be used as explicit group variables and auxiliary-predicate head arguments.
// outerAliases must be a snapshot of the outer tableColumnsMap_ taken BEFORE
// generateInnerBodyFromSubquery merges inner aliases into it.
static vector<string> findCorrelatedVarNamesFromWhere(
    sql::SQLStatement& inner,
    const std::unordered_map<string, std::unordered_set<string>>& outerAliases) {
    std::unordered_set<string> seen;
    vector<string> corrVarNames;
    auto inspect = [&](sql::ValueExpr& ve) {
        for (auto& vp: ve.getValues()) {
            if (vp.isIsConstant() || vp.isSubExpr()) continue;
            auto& q = vp.getQualifier();
            if (!q.table_.empty() && outerAliases.count(q.table_)) {
                string varName = q.name_ + "." + q.table_;
                if (!seen.count(varName)) {
                    seen.insert(varName);
                    corrVarNames.push_back(varName);
                }
            }
        }
    };
    for (auto& item: inner.getWhere().getItems()) {
        if (auto* pred = std::get_if<sql::Predicate>(&item)) {
            inspect(pred->getValue1());
            inspect(pred->getValue2());
        } else if (auto* grp = std::get_if<sql::WhereGroup>(&item)) {
            // Recurse into parenthesized groups to find correlated references.
            for (auto& inner_item: grp->getWhere().getItems()) {
                if (auto* inner_pred = std::get_if<sql::Predicate>(&inner_item)) {
                    inspect(inner_pred->getValue1());
                    inspect(inner_pred->getValue2());
                }
            }
        } else if (auto* ip = std::get_if<sql::InPredicate>(&item)) {
            // The LHS of IN may be a correlated outer reference.
            inspect(ip->value_);
            // Don't recurse into ip->subquery_ (handled separately during inner normalization).
        }
    }
    return corrVarNames;
}

// ---- Aggregate atom construction for a scalar WHERE subquery ----
// Always generates an auxiliary predicate rule to avoid nested aggregates (which
// occur when the outer query also has GROUP BY aggregates).  Returns a classical
// atom that references the auxiliary predicate.
//
// Two sub-cases:
//   (a) Constant LHS  (e.g.  0 < COUNT(*)):
//       #aux(corrVars...) :- lhs OP #agg{...; corrVars: inner_body}
//       Returns: #aux(corrVars...)
//
//   (b) Variable LHS  (e.g.  col == MIN(x)):
//       guard = fresh var
//       #aux(corrVars..., guard) :- guard == #agg{...; corrVars: inner_body}
//       Returns: #aux(corrVars..., lhs_var)
//       (when OP == EQUAL the classical atom unification handles the check;
//        other operators are handled with the guard mechanism inside the agg atom)
static vector<Atom> generateWhereSubqueryAtoms(
    sql::SubqueryPredicate& sp, SQLQuery& query, TranslationResult& result, string& errorMessage) {

    auto& inner = *sp.subquery_;

    validateWhereSubquery(inner, errorMessage);
    if (!errorMessage.empty()) return {};

    // Snapshot outer aliases BEFORE generateInnerBodyFromSubquery merges inner
    // table aliases into query.tableColumnsMap_ — used to identify correlated vars.
    auto outerAliasesSnapshot = query.tableColumnsMap_;

    auto innerBodyAtoms = generateInnerBodyFromSubquery(inner, query, result, errorMessage);
    if (!errorMessage.empty()) return {};

    auto& select = inner.getSelect();
    AggregateFunctionType aggFunc = select.getAggFunctions()[0];

    // Build the aggregation term.
    Term aggTerm;
    if (select.getItems()[0].toString(false) == "*" && aggFunc == COUNT) {
        // COUNT(*): pick any inner column as the aggregate term.
        auto qNames = getAllQualifiedNames(inner, query.tableColumnsMap_);
        BB_ASSERT(!qNames.empty());
        auto var = query.getVariableName(qNames[0].table_, qNames[0].name_);
        aggTerm = Term::createVariable(std::move(var));
    } else {
        auto& si = select.getItems()[0];
        aggTerm = generateTermFromValueExpr(const_cast<sql::ValueExpr&>(si),
            inner.getAlias(), query, errorMessage);
        if (!errorMessage.empty()) return {};
        // Handle arithmetic-wrapped aggregate (e.g. 0.2 * avg(col))
        if (aggTerm.getType() == ARITH) {
            auto arithVar = Term::createVariable(query.generateVarName());
            terms_vector_t builtinTerms = {arithVar, std::move(aggTerm)};
            innerBodyAtoms.push_back(Atom::createBuiltinAtom(std::move(builtinTerms), Binop::ASSIGNMENT));
            aggTerm = arithVar;
        }
    }

    terms_vector_t aggTerms;
    aggTerms.push_back(std::move(aggTerm));
    aggTerms.push_back(Term::createVariable(SQLQuery::ID_VAR));

    // Collect correlated (outer-scope) variable names from the inner WHERE using
    // the pre-merge snapshot (after the merge, inner aliases are also in the map).
    auto corrVarNames = findCorrelatedVarNamesFromWhere(inner, outerAliasesSnapshot);

    // Build explicit group terms from the correlated variable names.
    terms_vector_t explicitGroups;
    for (auto& name: corrVarNames) explicitGroups.push_back(Term::createVariable(name.c_str()));

    // Determine whether the LHS is a constant (e.g. 0) or a column reference.
    bool lhsIsConstant = sp.value_.getValues().size() == 1
                      && sp.value_.getValues()[0].isIsConstant();

    Atom aggAtom;
    Term emptyGuard;
    string guardVarName;  // only used in case (b)

    if (lhsIsConstant) {
        // (a) Constant LHS: embed guard and binop directly in the aggregate atom.
        auto lhsTerm = generateTermFromValueExpr(sp.value_, "", query, errorMessage);
        if (!errorMessage.empty()) return {};
        aggAtom = Atom::createAggregateAtom(aggFunc, sql::toCoreBinop(sp.op_), Binop::NONE_OP,
            lhsTerm, emptyGuard, std::move(aggTerms), std::move(innerBodyAtoms),
            std::move(explicitGroups));
    } else {
        // (b) Variable LHS: use a fresh guard variable assigned by the aggregate.
        guardVarName = query.generateVarName();
        Term guardTerm = Term::createVariable(guardVarName.c_str());
        aggAtom = Atom::createAggregateAtom(aggFunc, Binop::EQUAL, Binop::NONE_OP,
            guardTerm, emptyGuard, std::move(aggTerms), std::move(innerBodyAtoms),
            std::move(explicitGroups));
    }

    // Build auxiliary predicate head terms.
    terms_vector_t auxHeadTerms;
    for (auto& name: corrVarNames) auxHeadTerms.push_back(Term::createVariable(name.c_str()));
    if (!lhsIsConstant)
        auxHeadTerms.push_back(Term::createVariable(guardVarName.c_str()));

    // Create the auxiliary predicate and rule.
    string auxPredName = query.generatePredicateName();
    auto auxPred = query.context_.defaultSchema_.createPredicate(
        &query.context_, auxPredName.c_str(), auxHeadTerms.size());
    auto auxHead = Atom::createClassicalAtom(auxPred, std::move(auxHeadTerms));
    Rule auxRule(auxHead, aggAtom);
    result.rules_.push_back(std::move(auxRule));

    // Build the classical atom to return (used in the outer rule body).
    // For case (a): use corrVars directly — planner uses hash join (equality).
    // For case (b) with EQUAL: use outer column directly — planner uses hash join.
    // For case (b) with non-EQUAL (>, <, >=, <=, !=): use fresh var — planner does
    //   cross-product (broadcasts scalar), then a separate builtin enforces the actual binop.
    terms_vector_t classTerms;
    for (auto& name: corrVarNames) classTerms.push_back(Term::createVariable(name.c_str()));

    vector<Atom> resultAtoms;

    if (!lhsIsConstant) {
        auto lhsTerm = generateTermFromValueExpr(sp.value_, "", query, errorMessage);
        if (!errorMessage.empty()) return {};

        Binop coreBinop = sql::toCoreBinop(sp.op_);
        if (coreBinop == Binop::EQUAL) {
            // Equality: place outer column in classical atom → planner uses hash join.
            classTerms.push_back(std::move(lhsTerm));
            resultAtoms.push_back(Atom::createClassicalAtom(auxPred, std::move(classTerms)));
        } else {
            // Non-equality: use fresh var in classical atom → cross-product assigns scalar.
            // Then add a builtin to enforce the actual comparison.
            string freshVarName = query.generateVarName();
            classTerms.push_back(Term::createVariable(freshVarName.c_str()));
            resultAtoms.push_back(Atom::createClassicalAtom(auxPred, std::move(classTerms)));
            // Builtin: lhsTerm binop freshVar  (e.g. INDEX.#p_0 <= #Vfresh)
            terms_vector_t filterTerms;
            filterTerms.push_back(std::move(lhsTerm));
            filterTerms.push_back(Term::createVariable(freshVarName.c_str()));
            resultAtoms.push_back(Atom::createBuiltinAtom(std::move(filterTerms), coreBinop));
        }
    } else {
        resultAtoms.push_back(Atom::createClassicalAtom(auxPred, std::move(classTerms)));
    }

    return resultAtoms;
}

// ---- IN / NOT IN constant list CNF generation ----
// IN (c1, c2, ...): one clause with one OR-builtin atom (col=c1 OR col=c2 OR ...).
// NOT IN (c1, c2, ...): one clause per value with UNEQUAL atom (col!=c1 AND col!=c2 AND ...).
static CNF generateInListCNF(sql::InPredicate& ip, SQLQuery& query, string& errorMessage) {
    auto lhsTerm = generateTermFromValueExpr(ip.value_, "", query, errorMessage);
    if (!errorMessage.empty()) return {};

    if (ip.isNotIn_) {
        // NOT IN: one UNEQUAL atom per value (each becomes its own CNF clause → AND)
        CNF cnf;
        for (auto& valExpr: ip.values_) {
            auto rhsTerm = generateTermFromValueExpr(const_cast<sql::ValueExpr&>(valExpr),
                "", query, errorMessage);
            if (!errorMessage.empty()) return {};
            Term lhsCopy = lhsTerm;
            terms_vector_t terms = {std::move(lhsCopy), std::move(rhsTerm)};
            auto atom = Atom::createBuiltinAtom(std::move(terms), Binop::UNEQUAL);
            cnf.emplace_back();
            cnf.back().push_back(std::move(atom));
        }
        return cnf;
    } else {
        // IN: one OR-builtin atom covering all values
        if (ip.values_.size() == 1) {
            auto rhsTerm = generateTermFromValueExpr(const_cast<sql::ValueExpr&>(ip.values_[0]),
                "", query, errorMessage);
            if (!errorMessage.empty()) return {};
            terms_vector_t terms = {std::move(lhsTerm), std::move(rhsTerm)};
            auto atom = Atom::createBuiltinAtom(std::move(terms), Binop::EQUAL);
            CNF cnf;
            cnf.emplace_back();
            cnf.back().push_back(std::move(atom));
            return cnf;
        }
        vector<Atom> builtinAtoms;
        for (auto& valExpr: ip.values_) {
            auto rhsTerm = generateTermFromValueExpr(const_cast<sql::ValueExpr&>(valExpr),
                "", query, errorMessage);
            if (!errorMessage.empty()) return {};
            Term lhsCopy = lhsTerm;
            terms_vector_t terms = {std::move(lhsCopy), std::move(rhsTerm)};
            builtinAtoms.push_back(Atom::createBuiltinAtom(std::move(terms), Binop::EQUAL));
        }
        auto orAtom = Atom::createOrBuiltinAtom(builtinAtoms);
        CNF cnf;
        cnf.emplace_back();
        cnf.back().push_back(std::move(orAtom));
        return cnf;
    }
}

// ---- IN / NOT IN subquery translation ----
// Sub-case A (no aggregation): auxiliary predicate for set membership.
// Sub-case B (aggregation): reuse scalar subquery logic via SubqueryPredicate.
static vector<Atom> generateWhereInSubqueryAtoms(
    sql::InPredicate& ip, SQLQuery& query, TranslationResult& result, string& errorMessage) {

    BB_ASSERT(ip.subquery_);
    auto& inner = *ip.subquery_;

    if (inner.getSelect().getItems().size() != 1) {
        errorMessage = "IN/NOT IN subquery must return exactly one column";
        return {};
    }

    AggregateFunctionType aggFunc = inner.getSelect().getAggFunctions()[0];

    if (aggFunc != NONE) {
        // Sub-case B: aggregation → reuse scalar subquery logic.
        // Temporarily transfer subquery ownership to build a synthetic SubqueryPredicate.
        sql::SubqueryPredicate syntheticSp;
        syntheticSp.op_      = ip.isNotIn_ ? sql::SQL_UNEQUAL : sql::SQL_EQUAL;
        syntheticSp.value_   = ip.value_;
        syntheticSp.subquery_ = std::move(ip.subquery_);

        auto atoms = generateWhereSubqueryAtoms(syntheticSp, query, result, errorMessage);

        ip.subquery_ = std::move(syntheticSp.subquery_);
        return atoms;
    }

    // Sub-case A: no aggregation → set membership via auxiliary predicate.

    // Snapshot outer aliases BEFORE generateInnerBodyFromSubquery merges inner aliases.
    auto outerAliasesSnapshot = query.tableColumnsMap_;

    auto innerBodyAtoms = generateInnerBodyFromSubquery(inner, query, result, errorMessage);
    if (!errorMessage.empty()) return {};

    // Get the inner SELECT column variable (what is being checked for membership).
    auto& innerSelectItem = inner.getSelect().getItems()[0];
    auto innerColTerm = generateTermFromValueExpr(
        const_cast<sql::ValueExpr&>(innerSelectItem), "", query, errorMessage);
    if (!errorMessage.empty()) return {};

    // Collect correlated (outer-scope) variable names.
    auto corrVarNames = findCorrelatedVarNamesFromWhere(inner, outerAliasesSnapshot);

    // Build aux head terms: corrVars... + innerCol
    terms_vector_t auxHeadTerms;
    for (auto& name: corrVarNames)
        auxHeadTerms.push_back(Term::createVariable(name.c_str()));
    auxHeadTerms.push_back(std::move(innerColTerm));

    // Create auxiliary predicate and rule: #aux(corrVars..., innerCol) :- innerBodyAtoms
    string auxPredName = query.generatePredicateName();
    auto auxPred = query.context_.defaultSchema_.createPredicate(
        &query.context_, auxPredName.c_str(), auxHeadTerms.size());
    auto auxHead = Atom::createClassicalAtom(auxPred, std::move(auxHeadTerms));
    Rule auxRule(auxHead, innerBodyAtoms);
    result.rules_.push_back(std::move(auxRule));

    // Build LHS term for the outer classical atom (the column being tested).
    auto lhsTerm = generateTermFromValueExpr(ip.value_, "", query, errorMessage);
    if (!errorMessage.empty()) return {};

    // Build outer classical atom: #aux(corrVars..., lhsTerm)
    terms_vector_t classTerms;
    for (auto& name: corrVarNames)
        classTerms.push_back(Term::createVariable(name.c_str()));
    classTerms.push_back(std::move(lhsTerm));

    auto classAtom = Atom::createClassicalAtom(auxPred, std::move(classTerms));
    if (ip.isNotIn_)
        classAtom.setNegative(true);

    vector<Atom> resultAtoms;
    resultAtoms.push_back(std::move(classAtom));
    return resultAtoms;
}

// ---- WHERE CNF generation (updated to thread TranslationResult through) ----

// Generate a CNF for a single WhereItem.
// LIKE predicates: add to likeBody and return {{}} (OR-identity, skipped in final output).
// Predicates: return {{atom}}.
// WhereGroups: recurse.
// SubqueryPredicates: return {{aggregate_atom}}.
static CNF generateWhereItemCNF(sql::WhereItem& item, bool likeForbidden,
    vector<Atom>& likeBody, SQLQuery& query, TranslationResult& result, string& errorMessage) {
    if (auto* pred = std::get_if<sql::Predicate>(&item)) {
        if (pred->getOp() == sql::SQL_LIKE) {
            if (likeForbidden) {
                errorMessage = "LIKE operator inside grouped WHERE conditions is not supported";
                return {};
            }
            auto atom = generateLikeAtom(*pred, query, errorMessage);
            if (!errorMessage.empty()) return {};
            likeBody.push_back(std::move(atom));
            // Return a CNF with one empty clause: acts as OR-identity in distributeBinopAtoms
            CNF cnf;
            cnf.emplace_back();
            return cnf;
        }
        auto atom = generateBuiltinFromPredCondition(*pred, query, errorMessage);
        if (!errorMessage.empty()) return {};
        CNF cnf;
        cnf.emplace_back();
        cnf.back().push_back(std::move(atom));
        return cnf;
    } else if (auto* sp = std::get_if<sql::SubqueryPredicate>(&item)) {
        if (likeForbidden) {
            errorMessage = "Subquery predicate inside grouped WHERE conditions is not supported";
            return {};
        }
        auto atoms = generateWhereSubqueryAtoms(*sp, query, result, errorMessage);
        if (!errorMessage.empty()) return {};
        CNF cnf;
        for (auto& atom: atoms) {
            cnf.emplace_back();
            cnf.back().push_back(std::move(atom));
        }
        return cnf;
    } else if (auto* ip = std::get_if<sql::InPredicate>(&item)) {
        if (ip->subquery_) {
            if (likeForbidden) {
                errorMessage = "IN/NOT IN subquery inside grouped WHERE conditions is not supported";
                return {};
            }
            auto atoms = generateWhereInSubqueryAtoms(*ip, query, result, errorMessage);
            if (!errorMessage.empty()) return {};
            CNF cnf;
            for (auto& atom: atoms) {
                cnf.emplace_back();
                cnf.back().push_back(std::move(atom));
            }
            return cnf;
        } else {
            return generateInListCNF(*ip, query, errorMessage);
        }
    } else { // sql::WhereGroup
        auto& group = std::get<sql::WhereGroup>(item);
        auto& w = group.getWhere();
        return generateWhereListCNF(w.getItems(), w.getOps(), true, likeBody, query, result, errorMessage);
    }
}

// Generate a CNF from a list of WhereItems connected by ops.
// Implements SQL-standard AND > OR precedence:
//   items between consecutive ORs form AND-groups; the AND-groups are then OR-distributed.
// AND-groups: concatenate item CNFs; OR of groups: distribute (Cartesian product).
//
// NOTE: CNF conversion via OR-distribution can produce exponential clause counts.
// N OR-separated AND-groups each with M items yield up to M^N clauses. For queries
// with many OR-groups this may generate very large rule bodies. Consider simplifying
// such queries if performance degrades.
static CNF generateWhereListCNF(sql::predicate_vector_t& items, vector<sql::SQLOperator>& ops,
    bool likeForbidden, vector<Atom>& likeBody, SQLQuery& query, TranslationResult& result, string& errorMessage) {
    if (items.empty()) return {};
    BB_ASSERT(items.size() == ops.size() + 1);

    // Pre-check: LIKE in OR context is not supported.
    // If any OR operator exists at this level, LIKE cannot appear as a top-level item,
    // because it belongs to an AND-group that would be OR-distributed, giving wrong semantics.
    // (LIKE inside a nested WhereGroup is caught by the likeForbidden flag in generateWhereItemCNF.)
    bool hasOrAtThisLevel = false;
    for (auto& op : ops) {
        if (op == sql::SQL_OR) { hasOrAtThisLevel = true; break; }
    }
    for (idx_t i = 0; i < items.size(); ++i) {
        auto* pred = std::get_if<sql::Predicate>(&items[i]);
        if (pred && pred->getOp() == sql::SQL_LIKE) {
            if (likeForbidden) {
                errorMessage = "LIKE operator inside grouped WHERE conditions is not supported";
                return {};
            }
            if (hasOrAtThisLevel) {
                errorMessage = "LIKE operator inside OR conditions is not supported";
                return {};
            }
        }
    }

    // Split items into OR-separated AND-groups.
    // Each AND-group is a range [start, end] of item indices connected by AND.
    vector<std::pair<idx_t, idx_t>> andGroups; // (start, end) inclusive ranges
    idx_t groupStart = 0;
    for (idx_t i = 0; i < ops.size(); ++i) {
        if (ops[i] == sql::SQL_OR) {
            andGroups.emplace_back(groupStart, i);
            groupStart = i + 1;
        }
    }
    andGroups.emplace_back(groupStart, (idx_t)items.size() - 1);

    // For each AND-group, conjoin the item CNFs (AND = concatenate clauses).
    // Then distribute (OR) the AND-group CNFs.
    CNF cnfResult;
    for (idx_t gi = 0; gi < andGroups.size(); ++gi) {
        auto [start, end] = andGroups[gi];
        CNF groupCnf = generateWhereItemCNF(items[start], likeForbidden, likeBody, query, result, errorMessage);
        if (!errorMessage.empty()) return {};
        for (idx_t j = start + 1; j <= end; ++j) {
            CNF next = generateWhereItemCNF(items[j], likeForbidden, likeBody, query, result, errorMessage);
            if (!errorMessage.empty()) return {};
            for (auto& clause: next) groupCnf.push_back(std::move(clause));
        }
        if (gi == 0) {
            cnfResult = std::move(groupCnf);
        } else {
            cnfResult = distributeBinopAtoms(cnfResult, groupCnf);
        }
    }
    return cnfResult;
}

void generateBinopAtoms(vector<Atom>& body, sql::predicate_vector_t& items, vector<sql::SQLOperator>& ops,
    SQLQuery& query, TranslationResult& result, string& errorMessage) {
    if (items.empty()) return;
    vector<Atom> likeBody;
    auto cnf = generateWhereListCNF(items, ops, false, likeBody, query, result, errorMessage);
    if (!errorMessage.empty()) return;
    for (auto& likeAtom: likeBody) body.push_back(std::move(likeAtom));
    for (auto& atoms: cnf) {
        if (atoms.empty()) continue; // skip empty clauses (OR-identity placeholder from LIKE)
        if (atoms.size() == 1) {
            // Single atom: push directly (handles both BUILTIN and AGGREGATE atoms)
            body.push_back(std::move(atoms[0]));
        } else {
            // Multiple atoms in an OR clause: all are BUILTINs
            body.push_back(Atom::createOrBuiltinAtom(atoms));
        }
    }
}

// Determines which non-first table a WHERE filter atom can be pushed to.
// A filter is pushable if ALL its variables belong to a single non-first table.
// Variables follow the naming convention "col.alias" (e.g., "O_ORDERDATE.#p_1").
// Returns the table alias if pushable, empty string otherwise (e.g., cross-table
// joins, first-table filters, or constants without variables).
// Aggregate atoms are never pushed: they may contain correlated outer-scope variables
// and must remain in the main rule body where all outer bindings are visible.
string getAtomPushdownTarget(Atom& atom,
    const std::unordered_map<string, std::unordered_set<string>>& nonFirstTableCols) {
    if (atom.getType() == AGGREGATE) return "";
    vector<string> variables;
    atom.getVariablesList(variables);
    if (variables.empty()) return "";

    string targetAlias;
    for (auto& var : variables) {
        // For each variable, find which non-first table it belongs to
        // by checking if var matches "col.alias" for any (alias, cols) pair
        string foundAlias;
        for (auto& [alias, cols] : nonFirstTableCols) {
            string suffix = "." + alias;
            if (var.size() > suffix.size() && var.substr(var.size() - suffix.size()) == suffix) {
                string colPart = var.substr(0, var.size() - suffix.size());
                if (cols.contains(colPart)) {
                    foundAlias = alias;
                    break;
                }
            }
        }
        // Variable not found in any non-first table (belongs to first table or unknown)
        if (foundAlias.empty()) return "";
        // All variables must map to the same table
        if (targetAlias.empty())
            targetAlias = foundAlias;
        else if (targetAlias != foundAlias)
            return ""; // Cross-table filter, not pushable
    }
    return targetAlias;
}

// Converts qualified variable names in a filter atom from main-rule naming
// (e.g., "O_ORDERDATE.#p_1") to sourcing-rule naming (e.g., "O_ORDERDATE").
// This is needed because sourcing rules use simple column names as variables,
// while the main rule qualifies them with the table alias.
void convertToSourcingRuleVars(Atom& atom, const string& tableAlias,
    const std::unordered_set<string>& tableCols) {
    for (auto& col : tableCols) {
        string qualifiedName = col + "." + tableAlias;
        atom.replaceVariable(qualifiedName, col);
    }
}

Atom generatePredicateTableAtom(sql::FromItem& fi, SQLQuery& query, string& errorMessage) {
    string predName = fi.getTableName();
    string alias = fi.getAlias();
    BB_ASSERT(!alias.empty());
    BB_ASSERT(query.tableColumnsMap_.contains(alias));

    const auto& cols = fi.getPredicateColumns();
    vector<Term> terms;
    if (!cols.empty()) {
        // Custom column names: iterate in declaration order.
        // Columns that were pruned by removeUnusedCols are replaced by anonymous variables.
        for (auto& col : cols) {
            Term t = query.tableColumnsMap_[alias].count(col)
                ? Term::createVariable(query.getVariableName(alias, col))
                : Term::createVariable(Term::anonymous_variable);
            terms.push_back(std::move(t));
        }
    } else {
        // Default V1..VN columns: iterate over the full arity in numeric order.
        // fi.getPredicateArity() holds the resolved arity (set during assignAliasesAndCollectColumns).
        // Columns pruned by removeUnusedCols are replaced by anonymous variables.
        idx_t arity = static_cast<idx_t>(fi.getPredicateArity());
        for (idx_t j = 1; j <= arity; ++j) {
            string col = "V" + std::to_string(j);
            Term t = query.tableColumnsMap_[alias].count(col)
                ? Term::createVariable(query.getVariableName(alias, col))
                : Term::createVariable(Term::anonymous_variable);
            terms.push_back(std::move(t));
        }
    }
    auto pred = query.context_.defaultSchema_.createPredicate(
        &query.context_, predName.c_str(), terms.size());
    return Atom::createClassicalAtom(pred, std::move(terms));
}

void DatalogGenerator::generateRules(sql::SQLStatement &statement) {
    auto& defaultSchema = query_.context_.defaultSchema_;
    vector<Atom> body;

    // External FROM items (file tables) must appear first in the body so the
    // physical optimizer uses them as the pipeline source.  Derived-table subqueries
    // come second and are hash-joined against the source via the WHERE equality
    // predicates that follow.  Reversing this order would make a derived table the
    // source and leave the file scan as a broken pipeline atom that doesn't propagate
    // columns from earlier atoms.

    // Step 1: generate external (file-scan) body atoms first.
    idx_t i=0;
    for (auto& fi: statement.getFrom().getItems()) {
        // Predicate table references generate a classical atom directly (no auxiliary rule)
        if (fi.getType() == sql::FromItemType::PREDICATE_TABLE) {
            auto atom = generatePredicateTableAtom(fi, query_, result_.errorMessage_);
            body.push_back(std::move(atom));
            if (result_.foundAnError()) return;
            ++i;
            continue;
        }

        // for the first table do not create additional rule
        if (i == 0 && fi.getType() == sql::FromItemType::EXTERNAL) {
            auto extAtom = generateFirstExtAtomFromTable(fi, query_, result_.errorMessage_);
            body.push_back(std::move(extAtom));
            if (result_.foundAnError()) return;
            ++i;
            continue;
        }

        switch (fi.getType()) {
            case sql::FromItemType::EXTERNAL: {
                if (i > 0)
                    generateRuleForExtAtom(fi, query_, result_);
                break;
            }
            default:
                result_.errorMessage_ = "Type '" + fi.toString() + "' is not supported.";
        }
        string alias = fi.getAlias();
        auto atom = generateAtomFromTable(alias,query_, result_.errorMessage_);
        body.push_back(std::move(atom));
        if (result_.foundAnError()) return;
        ++i;
    }

    // Step 2: generate classical body atoms from derived-table subqueries.
    for (auto& sq: statement.getFrom().getSubQueries()) {
        generateRules(sq);
        if (result_.foundAnError()) return;
        // Construct the body atom for the sub-query alias using SELECT item order.
        // The inner rule's head is built in SELECT item order (all SELECT items),
        // so the body atom must match those positions without filtering by keptCols.
        // Filtering by keptCols would reduce arity below the inner rule's head arity,
        // causing a predicate mismatch (no results). Extra variables in the body that
        // are not projected to the outer head are safe in Datalog (existentially bound).
        string alias = sq.getAlias();
        BB_ASSERT(!alias.empty());
        BB_ASSERT(query_.tableColumnsMap_.contains(alias));
        vector<Term> sqTerms;
        for (auto& item: sq.getSelect().getItems()) {
            const auto& colAlias = item.getAlias();
            auto t = Term::createVariable(query_.getVariableName(alias, colAlias));
            sqTerms.push_back(std::move(t));
        }
        auto sqPred = query_.context_.defaultSchema_.createPredicate(
            &query_.context_, alias.c_str(), sqTerms.size());
        auto atom = Atom::createClassicalAtom(sqPred, std::move(sqTerms));
        if (result_.foundAnError()) return;
        body.push_back(std::move(atom));
    }

    // aggregates atoms information
    string alias = statement.getAlias();
    // generate head atom from the select
    vector<Term> headTerms; // head terms
    std::unordered_set<string> headVars; // variables in head
    std::unordered_set<string> groupVars; // variables in group by
    std::unordered_map<idx_t, vector<string>> aggVars;
    // for each agg function index the aggregation vars. note: is possible to aggregate on multiple columns.
    getAggregatesInformation(statement, query_, result_.errorMessage_, body, headTerms, headVars, groupVars, aggVars);
    if (result_.foundAnError()) return;

    // binops atoms — generate into a temporary vector, then push eligible filters
    // into sourcing rules so the PhysicalOptimizer can push them down to the readers.
    // This avoids loading unnecessary data from non-first tables.
    vector<Atom> binopAtoms;
    generateBinopAtoms(binopAtoms, statement.getWhere().getItems(), statement.getWhere().getOps(), query_, result_, result_.errorMessage_);
    if (result_.foundAnError()) return;

    // Build a map of non-first table aliases to their column sets.
    // The first table's external atom is already in the main rule body,
    // so the PhysicalOptimizer handles its filters directly.
    std::unordered_map<string, std::unordered_set<string>> nonFirstTableCols;
    idx_t tableIdx = 0;
    for (auto& fi : statement.getFrom().getItems()) {
        if (tableIdx > 0)
            nonFirstTableCols[fi.getAlias()] = query_.tableColumnsMap_[fi.getAlias()];
        ++tableIdx;
    }

    for (auto& atom : binopAtoms) {
        string target = getAtomPushdownTarget(atom, nonFirstTableCols);
        if (!target.empty()) {
            // Push this filter into the sourcing rule for 'target'
            auto clone = atom.clone();
            convertToSourcingRuleVars(clone, target, nonFirstTableCols[target]);
            // Find the sourcing rule whose head predicate matches the target alias
            for (auto& rule : result_.rules_) {
                BB_ASSERT(!rule.getHead().empty());
                if (rule.getHead()[0].getPredicate()->getName() == target) {
                    rule.addAtomInBody(std::move(clone));
                    break;
                }
            }
        } else {
            // Not pushable — keep in the main rule body
            body.push_back(std::move(atom));
        }
    }

    // generate the rule
    auto pred = defaultSchema.createPredicate(&query_.context_, alias.c_str(), headTerms.size());
    auto head = Atom::createClassicalAtom(pred, std::move(headTerms));
    Rule rule(head, body);


    if (statement.getSelect().containsAggregations()){
        vector<Rule> additionalRules;
        // Use the statement's own alias as the head predicate name so that outer
        // queries can reference the derived table by alias (e.g. REVENUE0(...)).
        string headPredName = alias;
        auto aggRule = generateAggRules(groupVars, aggVars, query_, statement, rule, headPredName, additionalRules, result_.errorMessage_);
        // call the order generator only on aggregates rules
        if (!statement.getOrderby().empty())
            generateOrderRule(statement, aggRule, result_.errorMessage_);

        for (auto& r: additionalRules)
            result_.rules_.push_back(std::move(r));
        result_.rules_.push_back(std::move(aggRule));

        return;
    }

    if (!statement.getOrderby().empty())
        generateOrderRule(statement, rule, result_.errorMessage_);

    result_.rules_.push_back(std::move(rule));
}



string getExternalFunction(const string& sqlExportFilePath, string& errorMessage) {
    if (FileSystem::isCSVFile(sqlExportFilePath)) {
        return "&write_csv";
    }
    errorMessage = "Unsupported file format: " + sqlExportFilePath+ ". The export path must end with '.csv'.";
    return "";
}

void DatalogGenerator::generateExportRule(sql::SQLStatement &statement) {
    if (result_.rules_.empty()) return;

    auto sqlExportFilePath = statement.getExportPath();
    auto externalFunction = getExternalFunction(sqlExportFilePath, result_.errorMessage_);
    if (result_.foundAnError()) {
        return;
    }
    // take the head predicate of the last rule as contains the answer of the query
    BB_ASSERT(result_.rules_.back().getHead().size() == 1);
    auto & si = statement.getSelect().getItems();
    auto headPredLastRule = result_.rules_.back().getHead()[0].getPredicate();
    BB_ASSERT(headPredLastRule->getArity() == si.size()); //
    auto newPredName = query_.generatePredicateName();

    auto& params = statement.getExportNamedParameters();
    vector<Value> values;
    values.emplace_back(sqlExportFilePath);
    vector<Term> t1;
    for (idx_t i = 0; i < headPredLastRule->getArity(); ++i) {
        string var = si[i].getAlias();
        if (var.empty()) {
            var = si[i].toString();
        }
        auto t = Term::createVariable(std::move(var));
        BB_ASSERT(t.getType() == VARIABLE);
        t1.push_back(std::move(t));
    }
    auto t2 = t1;
    // create a predicate to get execute by statement dependency
    auto head = Atom::createExternalAtom(params, values, externalFunction, std::move(t1));
    string name =  head.getExternalFunctionName() + "_" + query_.generatePredicateName();
    Predicate *predicate = query_.context_.defaultSchema_.createPredicate(&query_.context_ ,name.c_str(), headPredLastRule->getArity());
    predicate->setInternal(true);
    head.setPredicte(predicate);

    auto body = Atom::createClassicalAtom(headPredLastRule, std::move(t2));
    Rule rule(head, body);
    result_.rules_.push_back(std::move(rule));
}

SqlToDatalog::SqlToDatalog(ClientContext &context):context_(context) {
}

rules_vector_t SqlToDatalog::sqlToDatalog(sql::SQLStatement &statement, bool &foundAnError, string &errorMessage) {
    SQLQuery query(statement, context_);
    SqlQueryNormalizer normalizer(query);
    normalizer.normalize();
    if (normalizer.result_.foundAnError()) {
        foundAnError = true;
        errorMessage = normalizer.result_.errorMessage_;
        return {};
    }
    DatalogGenerator generator(query);
    generator.generate();
    if (generator.result_.foundAnError()) {
        foundAnError = true;
        errorMessage = generator.result_.errorMessage_;
        return {};
    }
    rules_vector_t result = std::move(generator.result_.rules_);
    return result;
}

}
