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

#include "bumblebee/common/StringUtils.hpp"
#include "bumblebee/function/PredFunction.hpp"

namespace bumblebee{
SqlToDatalog::SqlToDatalog(ClientContext &context): context_(context) {
}

rules_vector_t SqlToDatalog::sqlToDatalog(sql::SQLStatement &statement, bool& foundAnError, string& errorMessage) {
    rules_vector_t program;
    visitFrom(statement);
    replaceStar(statement);
    visitGroup(statement);
    if (!errorMessage_.empty()) {
        foundAnError = true;
        errorMessage = errorMessage_;
        return program;
    }
    genRuleFromSql(statement, program);
    if (!errorMessage_.empty()) {
        foundAnError = true;
        errorMessage = errorMessage_;
    }
    // The last rule contains the answer so set the predicate as non internal
    if (!statement.getExportPath().empty()) {
        generateExportRule(statement, program);
    }else if (!program.empty()) {
        BB_ASSERT(program.back().getHead().size() == 1);
        program.back().getHead()[0].getPredicate()->setInternal(false);
    }

    errorMessage = errorMessage_;

    return program;
}


bool isCSVFile(const string& path) {
    // Check if path length is at least 4 characters (".csv")
    if (path.length() < 4) return false;

    // Convert to lowercase for case-insensitive comparison
    string lowerPath = StringUtils::lower(path);

    // Check if it ends with ".csv"
    return lowerPath.compare(lowerPath.length() - 4, 4, ".csv") == 0;
}

string getExternalFunction(const string& sqlExportFilePath, string& errorMessage) {
    if (isCSVFile(sqlExportFilePath)) {
        return "&write_csv";
    }
    errorMessage = "Unsupported file format: " + sqlExportFilePath+ ". The export path must end with '.csv'.";
    return "";
}

void SqlToDatalog::generateExportRule(sql::SQLStatement &statement, rules_vector_t &rules) {
    if (rules.empty()) return;

    auto sqlExportFilePath = statement.getExportPath();
    auto externalFunction = getExternalFunction(sqlExportFilePath,errorMessage_);
    if (!isCSVFile(sqlExportFilePath)) {
        errorMessage_ = "Unsupported file format: " + sqlExportFilePath+ ". The export path must end with '.csv'.";
        return;
    }
    // take the head predicate of the last rule as contains the answer of the query
    auto headPredLastRule = rules.back().getHead()[0].getPredicate();
    auto newPredName = generatePredicateName();
    auto newPred = context_.defaultSchema_.createPredicate(&context_, newPredName.c_str(), headPredLastRule->getArity());

    auto& params = statement.getExportNamedParameters();
    vector<Value> values;
    values.push_back(sqlExportFilePath);
    vector<Term> t1;
    for (idx_t i = 0; i < headPredLastRule->getArity(); ++i) {
        auto t = Term::createVariable("V"+std::to_string(i));
        t1.push_back(std::move(t));
    }
    auto t2 = t1;
    // create a predicate to get execute by statement dependency
    auto head = Atom::createExternalAtom(params, values, externalFunction, std::move(t1));
    string name =  head.getExternalFunctionName() + "_" + generatePredicateName();
    Predicate *predicate = context_.defaultSchema_.createPredicate(&context_ ,name.c_str(), headPredLastRule->getArity());
    predicate->setInternal(true);
    head.setPredicte(predicate);

    auto body = Atom::createClassicalAtom(headPredLastRule, std::move(t2));
    Rule rule;
    rule.addAtomInBody(std::move(body));
    rule.addAtomInHead(std::move(head));
    rules.push_back(std::move(rule));
}

void SqlToDatalog::replaceStar(sql::SQLStatement &statement) {
    // replace * from the sql with the cols of the tables
    if (!errorMessage_.empty()) return;

    // first process sub queries
    for (auto& sq: statement.getFrom().getSubQueries()) {
        replaceStar(sq);
    }
    // check if contains * and is only one *
    bool star = false;
    for (auto & ve: statement.getSelect().getItems()) {
        for (auto& vp: ve.getValues()) {
            if (star) {
                // error we found a start with other select items or expression
                errorMessage_ = "Error, found a start with other select items, if you want to use start please do not specify additional select items";
                return;
            }
            if (vp.getQualifier().name_ == "*")
                star = true;
        }
    }
    if (!star)
        return;
    // we found a star, find all the cols of the tables/subqueries and add as select items
    std::unordered_set<string> aliasNames;
    for (auto& f: statement.getFrom().getSubQueries())
        aliasNames.insert(f.getAlias());
    for (auto& f: statement.getFrom().getItems())
        aliasNames.insert(f.getAlias());

    statement.getSelect().getItems().clear();
    for (auto& alias: aliasNames) {
        BB_ASSERT(!alias.empty());
        if (!tableColumnsMap_.contains(alias)) continue;
        for (auto&col : tableColumnsMap_[alias]) {
            sql::QualifiedName qualified = {.name_ = col, .table_ = alias};
            sql::ValuePrimary vp(qualified);
            sql::ValueExpr ve;
            ve.addValuePrimary(vp);
            statement.getSelect().getItems().push_back(std::move(ve));
        }
    }

}



vector<string> getColsFromExtTable(sql::FromItem &item, ClientContext& context_) {
    BB_ASSERT(item.getType() == sql::EXTERNAL);
    vector<ConstantType> types;
    for (auto& param: item.getInputValues())
        types.push_back(param.getConstantType());
    auto pfunc = (PredFunction*) context_.functionRegister_.getFunction(item.getExtTableName(), types ).get();
    vector<Expression> filters;
    vector<ConstantType> returnTypes;
    vector<string> names;
    names.push_back("*"); // select all the columns
    pfunc->bindFunction_(context_, item.getInputValues(), types, item.getNamedParameters(), returnTypes, names, filters);
    return names;
}


void SqlToDatalog::visitFrom(sql::SQLStatement &statement) {
    if (!errorMessage_.empty()) return;

    // first process sub queries
    for (auto& sq: statement.getFrom().getSubQueries()) {
        visitFrom(sq);
    }
    // process from items
    // register the columns of the tables
    // and create alias if is empty
    for (auto& fi: statement.getFrom().getItems()) {
        if (fi.getType() != sql::FromItemType::EXTERNAL) {
            errorMessage_ = "Only  ext table are supported :(";
            return;
        }
        if (fi.getAlias().empty()) {
            // generate alias
            auto newAlias = generatePredicateName();
            fi.setAlias(newAlias);
        }
        auto columns = getColsFromExtTable(fi, context_);
        if (tableColumnsMap_.contains(fi.getAlias())) {
            errorMessage_ = "Table "+ fi.getAlias() + " already exists.";
            return;
        }
        for (auto& col: columns) {
            tableColumnsMap_[fi.getAlias()].insert(col);
            if (columnTableMap_.contains(col)) continue;
            columnTableMap_[col] = fi.getAlias();
        }
    }
    if (statement.getAlias().empty()) {
        // generate alias
        auto alias = generatePredicateName();
        statement.setAlias(alias);
    }
    // regist the select columns
    for (auto& ve: statement.getSelect().getItems()) {
        if (ve.getAlias().empty()) {
            auto alias = generateVarName();
            ve.setAlias(alias);
        }
        tableColumnsMap_[statement.getAlias()].insert(ve.getAlias());
    }
}

void SqlToDatalog::visitGroup(sql::SQLStatement &statement) {
    if (!errorMessage_.empty()) return;
    if (statement.getGroupby().isEmpty())return;
    for (auto& sq: statement.getFrom().getSubQueries()) {
        visitGroup(sq);
    }
    std::unordered_set<string> groups;
    for (auto q: statement.getGroupby().getQualifiedNames()) {
        if (q.table_.empty()) {
            if (!columnTableMap_.contains(q.name_)) {
                errorMessage_ = "Column " + q.name_ + " does not exist or is in conflict with other tables.";
                return;
            }
            q.table_ = columnTableMap_[q.name_];
        }
        groups.insert(q.table_+"."+q.name_);
    }
    for (idx_t i=0;i<statement.getSelect().getAggFunctions().size();++i) {
        if (statement.getSelect().getAggFunctions()[i] != NONE) continue;
        // not aggregate column, check if exist in the groups
        auto &ve = statement.getSelect().getItems()[i];
        if (ve.getAlias().empty()) {
            auto newVar = generateVarName();
            ve.setAlias(newVar);
        }
        if (ve.getValues().size() > 1) {
            errorMessage_ = "Group by with expression not supported: " + ve.toString();
            return;
        }
        auto& vp = ve.getValues()[0];
        if (vp.isIsConstant())continue;
        auto& q= vp.getQualifier();
        if (q.table_.empty()) {
            if (!columnTableMap_.contains(q.name_)) {
                errorMessage_ = "Column " + q.name_ + " does not exist or is in conflict with other tables.";
                return;
            }
            q.table_ = columnTableMap_[q.name_];
        }
        if (!groups.contains(q.table_+"."+q.name_)) {
            errorMessage_ = "Column " + q.name_ + " does not belong in groups column.";
            return;
        }
    }
}



void SqlToDatalog::genRuleFromSql(sql::SQLStatement &statement, rules_vector_t &program) {
    auto& defaultSchema = context_.defaultSchema_;
    vector<Atom> body;
    // first generate rules from subqueries
    for (auto& sq: statement.getFrom().getSubQueries()) {
        genRuleFromSql(sq, program);
        if (!errorMessage_.empty())
            return;
        // construct the atom from subquery
        string alias = sq.getAlias();
        BB_ASSERT(!alias.empty());
        BB_ASSERT(tableColumnsMap_.contains(alias));
        auto atom = getAtomFromTable(alias);
        if (!errorMessage_.empty())
            return;
        body.push_back(std::move(atom));
    }
    // generate body atoms
    idx_t i=0;
    for (auto& fi: statement.getFrom().getItems()) {
        if (fi.getType() == sql::FromItemType::EXTERNAL) {
            // we need to generate another rule
            generateRuleForExtAtom(fi, program);
        }
        string alias = fi.getAlias();
        BB_ASSERT(!alias.empty());
        BB_ASSERT(tableColumnsMap_.contains(alias));
        auto atom = getAtomFromTable(alias);
        if (!errorMessage_.empty())
            return;
        body.push_back(std::move(atom));
        ++i;
    }

    string alias = statement.getAlias();


    // generate head atom from the select
    vector<Term> headTerms;
    std::unordered_set<string> headVars;
    for (auto& si:statement.getSelect().getItems()) {
        auto t = getTermFromValueExpr( si, statement.getAlias());
        if (!errorMessage_.empty())
            return;
        if(t.getType() == VARIABLE) {
            headVars.insert(t.getVariable());
        }
        headTerms.push_back(std::move(t));
    }
    // add the group by cols in head terms
    for (auto& q: statement.getGroupby().getQualifiedNames()) {
        auto var = getVariableName(q.table_, q.name_);
        if (headVars.contains(var)) continue;
        auto t = Term::createVariable(std::move(var));
        headVars.insert(t.getVariable());
        headTerms.push_back(std::move(t));
    }


    // as the OR condition split the rules
    // generate a vector of conditions for each OR
    vector<vector<Atom>> binopAtoms;
    auto& predicates = statement.getWhere().getItems();
    auto& ops = statement.getWhere().getOps();
    BB_ASSERT(predicates.empty() || predicates.size() == ops.size() +1);
    i = 0;
    binopAtoms.emplace_back();
    for (auto& p: predicates) {
        auto batom = getBuiltinFromPredCondition(p);
        binopAtoms.back().push_back(std::move(batom));
        if (i < ops.size() && ops[i] == sql::SQL_OR) {
            // we have an or so we need to generate another rule
            binopAtoms.emplace_back();
        }
        ++i;
    }

    auto pred = defaultSchema.createPredicate(&context_, alias.c_str(), headTerms.size());
    auto head = Atom::createClassicalAtom(pred, std::move(headTerms));
    generateRules(head, body, binopAtoms, program);
    if (statement.getSelect().containsAggregations()) {
        generateAggRules(statement, pred, program);
    }
}

void SqlToDatalog::generateAggRules(sql::SQLStatement &statement, Predicate* headPred, rules_vector_t &rules) {
    auto& select = statement.getSelect();
    std::unordered_set<string> headVars;
    // grups vars for external atom and agg atoms
    vector<Term> atomGroupVars;
    // group vars for internal aggregates
    vector<Term> groupTerms;
    vector<idx_t> aggTermsIndex;
    vector<Term> headTerms;

    for (idx_t i=0;i<select.getItems().size();++i) {
        string var;
        Term t = getTermFromValueExpr(select.getItems()[i], statement.getAlias());
        if (select.getAggFunctions()[i] != NONE || t.getType() != VARIABLE) {
            if (select.getAggFunctions()[i] != NONE) {
                aggTermsIndex.push_back(i);
                t = Term::createVariable(select.getItems()[i].getAlias());
            }
            var = Term::anonymous_variable;
        }else {
            var = t.getVariable();
            Term vTerm = Term::createVariable(var.c_str());
            groupTerms.push_back(std::move(vTerm));
        }
        headTerms.push_back(std::move(t));
        headVars.insert(var);
        Term vTerm = Term::createVariable(std::move(var));
        atomGroupVars.push_back(std::move(vTerm));
    }
    // add the group by cols in head terms
    for (auto& q: statement.getGroupby().getQualifiedNames()) {
        auto var = getVariableName(q.table_, q.name_);
        if (headVars.contains(var)) continue;
        headVars.insert(var);
        auto t = Term::createVariable(var.c_str());
        atomGroupVars.push_back(std::move(t));
        t = Term::createVariable(std::move(var));
        groupTerms.push_back(std::move(t));

    }
    BB_ASSERT(headPred->getArity() == atomGroupVars.size());
    vector<Atom> body;
    // first add the group atom
    auto terms = atomGroupVars;
    Atom groupAtom = Atom::createClassicalAtom(headPred, std::move(terms));
    body.push_back(std::move(groupAtom));
    // generate agg atoms for each aggregation
    for (auto idx : aggTermsIndex) {
        Term lt,ut;
        lt = Term::createVariable(select.getItems()[idx].getAlias());
        Term aggVar = Term::createVariable(generateVarName());
        auto aggTerms = groupTerms;
        aggTerms.insert(aggTerms.begin(), aggVar);
        auto aggAtomTerms = atomGroupVars;
        aggAtomTerms[idx] = std::move(aggVar);
        auto aggInternalAtom = Atom::createClassicalAtom(headPred, std::move(aggAtomTerms));
        vector<Atom> aggBodyAtoms;
        aggBodyAtoms.push_back(std::move(aggInternalAtom));
        Atom aggregateAtom = Atom::createAggregateAtom(select.getAggFunctions()[idx],ASSIGNMENT, NONE_OP,lt,ut, std::move(aggTerms), std::move(aggBodyAtoms) );
        body.push_back(std::move(aggregateAtom));
    }
    string alias = generatePredicateName();
    auto newPred = context_.defaultSchema_.createPredicate(&context_, alias.c_str(), headTerms.size());
    Atom head = Atom::createClassicalAtom(newPred, std::move(headTerms));
    Rule rule;
    rule.addAtomInHead(std::move(head));
    rule.setBody(body);
    rules.push_back(std::move(rule));
}

void SqlToDatalog::generateRules( Atom& head, vector<Atom>& body, vector<vector<Atom>>& conditions, rules_vector_t& program ) {
    if (conditions.size() == 1) {
        // we have only one block of conditions so merge with the body
        for (auto& a:conditions.back())
            body.push_back(std::move(a));
    }
    if (conditions.size() < 2) {
        Rule rule;
        rule.setBody(body);
        rule.addAtomInHead(std::move(head));
        program.push_back(std::move(rule));
        return;
    }


    // for each condition let's generate a rule
    for (auto& condition: conditions) {
        Rule r;
        Atom h = head.clone();
        for (auto& a:body) {
            Atom ca = a.clone();
            condition.insert(condition.begin(), std::move(ca));
        }
        r.setBody(condition);
        r.addAtomInHead(std::move(h));
        program.push_back(std::move(r));
    }


}

Atom SqlToDatalog::getAtomFromTable(string &table) {
    // generate an atom with all the variables as columns in the table
    BB_ASSERT(!table.empty());
    BB_ASSERT(tableColumnsMap_.contains(table));
    vector<Term> terms;
    for (auto& col: tableColumnsMap_[table]) {
        string var = getVariableName(table, col);
        BB_ASSERT(!var.empty());
        auto t = Term::createVariable(std::move(var));
        terms.push_back(std::move(t));
    }
    auto pred = context_.defaultSchema_.createPredicate(&context_, table.c_str(), terms.size());
    return Atom::createClassicalAtom(pred, std::move(terms));
}

Atom SqlToDatalog::getExtAtomFromTable(sql::FromItem &item) {
    // generate external atom
    BB_ASSERT(item.getType() == sql::FromItemType::EXTERNAL);
    vector<Term> terms;
    for (auto& col: tableColumnsMap_[item.getAlias()]) {
        // do not change the var name otherwise the external atom does not match with the var name
        // for example with csv reader var match the column name
        string var = col;
        BB_ASSERT(!var.empty());
        auto t = Term::createVariable(std::move(var));
        terms.push_back(std::move(t));
    }
    return Atom::createExternalAtom(item.getNamedParameters(), item.getInputValues(), item.getExtTableName(), std::move(terms));
}

void SqlToDatalog::generateRuleForExtAtom(sql::FromItem &fi, rules_vector_t &program) {
    auto extAtom = getExtAtomFromTable(fi);
    vector<Term> terms = extAtom.getTerms();
    string alias = fi.getAlias();
    BB_ASSERT(!alias.empty());
    auto pred = context_.defaultSchema_.createPredicate(&context_, alias.c_str(), terms.size());
    auto head = Atom::createClassicalAtom(pred, std::move(terms));
    Rule rule;
    rule.addAtomInBody(std::move(extAtom));
    rule.addAtomInHead(std::move(head));
    program.push_back(std::move(rule));
}

Atom SqlToDatalog::getBuiltinFromPredCondition(sql::Predicate &predicate) {
    vector<Term> terms;
    auto t1 = getTermFromValueExpr(predicate.getValue1());
    auto t2 = getTermFromValueExpr(predicate.getValue2());
    terms.push_back(std::move(t1));
    terms.push_back(std::move(t2));
    return Atom::createBuiltinAtom(std::move(terms), predicate.getOp());
}

Term SqlToDatalog::getTermFromValueExpr(sql::ValueExpr ve, const string& sqlAlias) {
    // set the tables of the qualifiers if empty
    for (auto& vp: ve.getValues()) {
        if (!vp.isIsConstant() && vp.getQualifier().table_.empty()) {
            if (!columnTableMap_.contains(vp.getQualifier().name_)) {
                errorMessage_ = "Column " + vp.getQualifier().name_ + " does not exist or is in conflict with other tables.";
                return {};
            }
            vp.getQualifier().table_ = columnTableMap_[vp.getQualifier().name_];
        }
        if (!vp.isIsConstant() && !tableColumnsMap_.contains(vp.getQualifier().table_)) {
            errorMessage_ = "Table " + vp.getQualifier().table_ + " does not exist.";
            return {};
        }
    }

    vector<Term> terms;
    for (auto& vp: ve.getValues()) {
        if (!vp.isIsConstant()) {
            // generate the variable using the table and name
            string var = getVariableName(vp.getQualifier().table_, vp.getQualifier().name_);
            auto t = Term::createVariable(std::move(var));
            terms.push_back(std::move(t));
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
            t.addInArithTerm(i+1, ve.getOperators()[i] );
        }
        result = std::move(t);
    }

    if (sqlAlias.empty())
        return result;

    string alias = ve.getAlias();
    if (alias.empty())
        alias = generateVarName();

    if (!columnTableMap_.contains(alias))
        columnTableMap_[alias] = sqlAlias;
    tableColumnsMap_[sqlAlias].insert(alias);

    return result;
}

string SqlToDatalog::generatePredicateName() {
    string result = PRED_PREFIX;
    result += "_";
    return result + std::to_string(counter_++);
}

string SqlToDatalog::generateVarName() {
    string result = VAR_PREFIX;
    return result + std::to_string(counter_++);
}

string SqlToDatalog::getVariableName(const string &table, const string &col) {
    return col+"."+table;
}
}
