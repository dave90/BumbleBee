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
#include "SQLStatement.hpp"
#include "bumblebee/ClientContext.hpp"
#include "bumblebee/parser/statement/Rule.hpp"

namespace bumblebee{

// Transform SQL Statement to Datalog program
class SqlToDatalog {
public:
    static constexpr char PRED_PREFIX[] = "#p";
    static constexpr char VAR_PREFIX[] = "V";

    explicit SqlToDatalog(ClientContext &context);

    void replaceStar(sql::SQLStatement & statement);

    rules_vector_t sqlToDatalog(sql::SQLStatement& statement, bool& foundAnError, string& errorMessage);

private:
    // visit the from tables setting the alias (if not) and gather the columns of the table
    void visitFrom(sql::SQLStatement& statement);

    void generateRuleForExtAtom(sql::FromItem & fi, rules_vector_t & rules);

    void genRuleFromSql(sql::SQLStatement& statement, rules_vector_t& program);
    Atom getAtomFromTable(string& table);
    Atom getExtAtomFromTable(sql::FromItem& table);
    Atom getBuiltinFromPredCondition(sql::Predicate& predicate);
    Term getTermFromValueExpr( sql::ValueExpr ve, const string& alias = "");

    void generateRules( Atom& head, vector<Atom>& body, vector<vector<Atom>>& conditions, rules_vector_t& program );

    string generatePredicateName();
    string generateVarName();
    string getVariableName(const string& table, const string& col);

    idx_t counter_{0};
    // Map of column name -> table. If 2 table share the same column name will not be stored in the map
    std::unordered_map<string, string> columnTableMap_;
    // Map of table -> list of columns of the table
    std::unordered_map<string, std::unordered_set<string>> tableColumnsMap_;

    ClientContext& context_;
    string errorMessage_;
};

}
