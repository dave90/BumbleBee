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

#include "CLI11.hpp"
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

void SqlQueryNormalizer::normalize() {
    assignAliasesAndCollectColumns(query_.statement_);
    expandSelectStars(query_.statement_);
    validateGroupBy(query_.statement_);
}


vector<string> getColsFromExtTable(sql::FromItem &item, ClientContext& context_) {
    BB_ASSERT(item.getType() == sql::EXTERNAL);
    vector<ConstantType> types;
    for (auto& param: item.getInputValues())
        types.push_back(param.getConstantType());
    auto pfunc = (PredFunction*) context_.functionRegister_.getFunction(item.getExtTableName(), types ).get();
    vector<Expression> filters;
    vector<ConstantType> returnTypes;
    vector<string> names = {"*"};
    pfunc->bindFunction_(context_, item.getInputValues(), types, item.getNamedParameters(), returnTypes, names, filters);
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

void SqlQueryNormalizer::assignAliasesAndCollectColumns(sql::SQLStatement& statement) {
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

    if (statement.getAlias().empty())
        // generate alias
        statement.setAlias(query_.generatePredicateName());

    // fill the alias and table information
    for (auto& ve: statement.getSelect().getItems()) {
        if (ve.getAlias().empty()) {
            auto alias = query_.generateVarName();
            if (ve.getValues().size() == 1) {
                // take as alias the name of the variable if is not constant
                auto& vp = ve.getValues()[0];
                if (!vp.isIsConstant()) {
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
    // fill the where values
    for (auto& p: statement.getWhere().getItems()) {
        auto& vi1 = p.getValue1();
        auto& vi2 = p.getValue2();

        // se the table information if empty
        for (auto& vi: vi1.getValues())
            fillQTable(vi, colsTableMap, result_.errorMessage_);
        for (auto& vi: vi2.getValues())
            fillQTable(vi, colsTableMap, result_.errorMessage_);
    }
    // fill the groupo by values
    for (auto& ve: statement.getGroupby().getItems()) {
        // se the table information if empty
        for (auto& vi: ve.getValues())
            fillQTable(vi, colsTableMap, result_.errorMessage_);
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




void DatalogGenerator::generate() {
    generateRules(query_.statement_);
    if (result_.foundAnError()) return;
    // The last rule contains the answer so set the predicate as non internal
    if (!query_.statement_.getExportPath().empty()) {
        generateExportRule(query_.statement_);
    }else if (!result_.rules_.empty()) {
        auto& program = result_.rules_;
        BB_ASSERT(program.back().getHead().size() == 1);
        program.back().getHead()[0].getPredicate()->setInternal(false);
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

void generateRuleForExtAtom(sql::FromItem& item, SQLQuery& query, TranslationResult& result) {
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
        if (!vp.isIsConstant()) {
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
    return Atom::createBuiltinAtom(std::move(terms), predicate.getOp());
}

rules_vector_t generateRulesFromConditions( Atom& head, vector<Atom>& body, vector<vector<Atom>>& conditions ) {
    rules_vector_t program;
    if (conditions.size() == 1) {
        // we have only one block of conditions so merge with the body
        for (auto& a:conditions.back())
            body.push_back(std::move(a));
    }
    if (conditions.size() < 2) {
        Rule rule(head, body);
        program.push_back(std::move(rule));
        return program;
    }

    // set the predicate as distinct because we will generate multiple rules
    head.getPredicate()->setDistinct();
    // for each condition let's generate a rule
    for (auto& condition: conditions) {
        Atom h = head.clone();
        for (auto& a:body) {
            Atom ca = a.clone();
            condition.insert(condition.begin(), std::move(ca));
        }
        Rule r(h, condition);
        program.push_back(std::move(r));
    }

    return program;
}


void generateAggRules(const std::unordered_set<string>& groupVars, const std::unordered_map<idx_t, vector<string>>& aggVars,
        SQLQuery& query, sql::SQLStatement& statement, Rule& rule, string& headPredicateName,
        rules_vector_t& aggRules, rules_vector_t& additionalRules,  string& errorMessage) {

    auto& select = statement.getSelect();
    vector<Atom> body;
    BB_ASSERT(rule.getHead().size() == 1);
    // head atom of the last rule that contains the aggregation predicate
    auto& headLastRule =  rule.getHead()[0];

    if (!groupVars.empty()) {
        // generate aggregation rule
        auto terms = headLastRule.getTerms();
        vector<Term> headGroupTerms;
        for (idx_t i=0;i<terms.size();++i) {
            if ( groupVars.contains( terms[i].getVariable())) {
                headGroupTerms.push_back(terms[i]);
                continue;
            }
            // is not a group var so set as anonymous
            terms[i] = Term::createVariable(Term::anonymous_variable);
        }
        Atom groupAtom = Atom::createClassicalAtom(headLastRule.getPredicate(), std::move(terms));
        auto newPredName = query.generatePredicateName();
        auto newPred = query.context_.defaultSchema_.createPredicate(&query.context_, newPredName.c_str(), headGroupTerms.size());
        newPred->setDistinct(); // set distinct to decrease the scan
        Atom head = Atom::createClassicalAtom(newPred, std::move(headGroupTerms));
        Rule newRule(head.clone(), groupAtom);
        additionalRules.push_back(std::move(newRule));
        body.push_back(std::move(head));
    }
    // collect all the aggregation variables
    std::unordered_set<string> allAggVars;
    for (auto& [_, vars]:aggVars) {
        allAggVars.insert(vars.begin(), vars.end());
    }

    // now generate the aggregation atoms
    for (auto& [agg, vars]:aggVars) {
        auto terms = headLastRule.getTerms();
        vector<Term> aggTerms;
        std::unordered_set<string> aggTermVars;
        // first add in agg terms the aggregation column (must be the first term)
        aggTerms.push_back(Term::createVariable(vars[0].c_str()));
        aggTermVars.insert(vars[0].c_str());

        // let's insert all the agg vars to optimize the aggregation aux
        for (idx_t i=0;i<terms.size();++i) {
            auto var = terms[i].getVariable();
            if ( groupVars.contains( var) || allAggVars.contains(var)  ) {
                if (!aggTermVars.contains(var)) {
                    aggTerms.push_back(terms[i]);
                    aggTermVars.insert(var);
                }
                continue;
            }
            // is not a group or agg var so set as anonymous
            auto t = Term::createVariable(Term::anonymous_variable);
            terms[i] = std::move(t);
        }
        Term lt,ut;
        lt = Term::createVariable(select.getItems()[agg].getAlias());
        auto aggInternalAtom = Atom::createClassicalAtom(headLastRule.getPredicate(), std::move(terms));
        vector<Atom> aggBodyAtoms;
        aggBodyAtoms.push_back(std::move(aggInternalAtom));
        Atom aggregateAtom = Atom::createAggregateAtom(select.getAggFunctions()[agg],ASSIGNMENT, NONE_OP,lt,ut, std::move(aggTerms), std::move(aggBodyAtoms) );
        body.push_back(std::move(aggregateAtom));
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
    Rule newRule(head, body);
    aggRules.push_back(std::move(newRule));
}


void generateOrderRule(sql::SQLStatement &statement, SQLQuery& query, rules_vector_t &rules, string& errorMessage) {
    if (!errorMessage.empty()) return;

    auto& group = statement.getOrderby();
    if (group.empty())return;

    if (rules.size() == 1) {
        // only one rules related to the order by
        auto& lastRule = rules.back();
        lastRule.setLimit(statement.getLimit());

        lastRule.setModifiers(group.getColModifiers());
        if (lastRule.getLimit() == 0) {
            errorMessage =  "Unsupported order by directive without a limit. Please add a #limit directive.";
        }
        return;
    }
    // multiple rules with order by, so we need to create an additional rule that gather all
    // the values for the rule and then apply the order by
    auto pred = rules[0].getHead()[0].getPredicate();
    // expect same predicate in the head
    for (auto& rule: rules) {
        BB_ASSERT(rule.getHead().size() == 1);
        BB_ASSERT(rule.getHead()[0].getPredicate() == pred);
    }
    Atom body = rules[0].getHead()[0].clone();
    vector<Term> headTerms = body.getTerms();
    string alias = query.generatePredicateName();
    auto newPred = query.context_.defaultSchema_.createPredicate(&query.context_, alias.c_str(), headTerms.size());
    newPred->setDistinct();
    Atom head = Atom::createClassicalAtom(newPred, std::move(headTerms));
    Rule newRule(head, body);
    newRule.setLimit(statement.getLimit());

    newRule.setModifiers(group.getColModifiers());
    if (newRule.getLimit() == 0) {
        errorMessage =  "Unsupported order by directive without a limit. Please add a #limit directive.";
    }
    rules.push_back(std::move(newRule));
}


void getAggregatesInformation(sql::SQLStatement &statement, SQLQuery& query,
    string& errorMessage, vector<Atom>& body, vector<Term>& headTerms, std::unordered_set<string>& headVars,
    std::unordered_set<string>& groupVars, std::unordered_map<idx_t, vector<string>>& aggVars) {
    // if contains aggregation add gen id atom
    if (statement.containsAggregations()) {
        auto atom = generateAtomGenId(query.ID_VAR);
        body.push_back(std::move(atom));
        headTerms.push_back(Term::createVariable(query.ID_VAR));
    }

    for (idx_t i = 0;i< statement.getSelect().getItems().size();++i) {
        auto si = statement.getSelect().getItems()[i];
        if (si.toString(false) == "*" && statement.getSelect().getAggFunctions()[i] == COUNT) {
            // handle count(*), add the ID as counter and another column if only count is present
            auto t = Term::createVariable(query.ID_VAR);
            aggVars[i].push_back(t.getVariable());
            if (statement.getNumAggregations() == 1) {
                // add another var as aggregation because we have only count(*) as aggregation
                // otherwise optimizer we will keep only the ID in the body
                auto qNames = getAllQualifiedNames(statement, query.tableColumnsMap_);
                BB_ASSERT(!qNames.empty());
                auto var = query.getVariableName(qNames[0].table_, qNames[0].name_);
                aggVars[i].push_back(var);
                if (!headVars.contains(var)) {
                    headVars.insert(var);
                    headTerms.push_back(Term::createVariable(var.c_str()));
                }
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
            // put also ID var for duplicates values
            aggVars[i].emplace_back(query.ID_VAR);
        }

        if (headVars.contains(t.getVariable()))
            continue;
        headVars.insert(t.getVariable());
        headTerms.push_back(std::move(t));
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

void DatalogGenerator::generateRules(sql::SQLStatement &statement) {
    auto& defaultSchema = query_.context_.defaultSchema_;
    vector<Atom> body;

    // first generate rules from subqueries
    for (auto& sq: statement.getFrom().getSubQueries()) {
        generateRules(sq);
        if (result_.foundAnError()) return;
        // construct the atom from subquery
        string alias = sq.getAlias();
        BB_ASSERT(!alias.empty());
        BB_ASSERT(query_.tableColumnsMap_.contains(alias));
        auto atom = generateAtomFromTable(alias, query_, result_.errorMessage_);
        if (result_.foundAnError()) return;
        body.push_back(std::move(atom));
    }

    // generate body atoms
    idx_t i=0;
    for (auto& fi: statement.getFrom().getItems()) {
        switch (fi.getType()) {
            case sql::FromItemType::EXTERNAL:
                generateRuleForExtAtom(fi, query_, result_);
                break;
            default:
                result_.errorMessage_ = "Type '" + fi.toString() + "' is not supported.";
        }

        string alias = fi.getAlias();
        auto atom = generateAtomFromTable(alias,query_, result_.errorMessage_);
        if (result_.foundAnError()) return;
        body.push_back(std::move(atom));
        ++i;
    }

    string alias = statement.getAlias();
    // generate head atom from the select
    vector<Term> headTerms; // head terms
    std::unordered_set<string> headVars; // variables in head
    std::unordered_set<string> groupVars; // variables in group by
    std::unordered_map<idx_t, vector<string>> aggVars;
    // for each agg function index the aggregation vars. note: is possible to aggregate on multiple columns.
    getAggregatesInformation(statement, query_, result_.errorMessage_, body, headTerms, headVars, groupVars, aggVars);

    // as the OR condition split the rules
    // generate a vector of conditions for each OR
    vector<vector<Atom>> binopAtoms;
    auto& predicates = statement.getWhere().getItems();
    auto& ops = statement.getWhere().getOps();
    BB_ASSERT(predicates.empty() || predicates.size() == ops.size() +1);
    i = 0;
    binopAtoms.emplace_back();
    for (auto& p: predicates) {
        auto batom = generateBuiltinFromPredCondition(p, query_, result_.errorMessage_);
        binopAtoms.back().push_back(std::move(batom));
        if (i < ops.size() && ops[i] == sql::SQL_OR) {
            // we have an or so we need to generate another rule
            binopAtoms.emplace_back();
        }
        ++i;
    }

    // generates the final rules
    auto pred = defaultSchema.createPredicate(&query_.context_, alias.c_str(), headTerms.size());
    auto head = Atom::createClassicalAtom(pred, std::move(headTerms));
    auto rules = generateRulesFromConditions(head, body, binopAtoms);

    if (statement.getSelect().containsAggregations()){
        vector<Rule> aggregatedRules;
        vector<Rule> additionalRules;
        // generate the aggregated rules
        string headPredName = query_.generatePredicateName();
        for (auto& rule: rules)
            generateAggRules(groupVars, aggVars, query_, statement, rule, headPredName, aggregatedRules, additionalRules, result_.errorMessage_);
        // call the order generator only on aggregates rules
        if (!statement.getOrderby().empty())
            generateOrderRule(statement, query_, aggregatedRules, result_.errorMessage_);

        for (auto& rule: rules)
            result_.rules_.push_back(std::move(rule));
        for (auto& rule: additionalRules)
            result_.rules_.push_back(std::move(rule));
        for (auto& rule: aggregatedRules)
            result_.rules_.push_back(std::move(rule));

        return;
    }

    if (!statement.getOrderby().empty())
        generateOrderRule(statement, query_, rules, result_.errorMessage_);

    for (auto& rule: rules)
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
