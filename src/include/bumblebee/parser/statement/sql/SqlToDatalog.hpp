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

struct TranslationResult {
    rules_vector_t rules_;
    std::string errorMessage_;

    bool foundAnError() const {
        return !errorMessage_.empty();
    }
};

struct SQLQuery {
    static constexpr char PRED_PREFIX[] = "#p";
    static constexpr char VAR_PREFIX[] = "#V";
    static constexpr char ID_VAR[] = "#ID";

    SQLQuery(sql::SQLStatement &statement, ClientContext &context)
        : statement_(statement),
          context_(context) {
    }

    sql::SQLStatement& statement_;
    ClientContext& context_;
    // Map of table -> list of columns alias of the table
    std::unordered_map<string, std::unordered_set<string>> tableColumnsMap_;
    idx_t counter_{0};




    string generatePredicateName();
    string generateVarName();
    static string getVariableName(const string& table, const string& col);
};

class SqlQueryNormalizer {
public:
    TranslationResult result_;

    explicit SqlQueryNormalizer(SQLQuery &query);

    void normalize();
private:
    void assignAliasesAndCollectColumns(sql::SQLStatement& statement);
    void expandSelectStars(sql::SQLStatement& statement);
    void validateGroupBy(sql::SQLStatement& statement);

    SQLQuery& query_;
};

class DatalogGenerator {
public:
    TranslationResult result_;

    explicit DatalogGenerator(SQLQuery &query)
        : query_(query) {
    }

    void generate();
private:

    void generateRules(sql::SQLStatement& statement);
    void generateExportRule(sql::SQLStatement& statement);

    SQLQuery& query_;
};

class SqlToDatalog {
public:

    explicit SqlToDatalog(ClientContext &context);


    rules_vector_t sqlToDatalog(sql::SQLStatement& statement, bool& foundAnError, string& errorMessage);

private:

    ClientContext& context_;
};


}
