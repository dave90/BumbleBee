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

#include <gtest/gtest.h>

#include "bumblebee/ClientContext.hpp"
#include "bumblebee/catalog/Catalog.hpp"
#include "bumblebee/parser/ParserInputDirector.hpp"
#include "bumblebee/parser/statement/sql/SqlToDatalog.hpp"

using namespace bumblebee;

class SqlPredicateFromTest : public ::testing::Test {
protected:
    ClientContext context;

    // Helper: parse a SQL string and return the translated rules.
    // Pre-registers `predName/arity` in the schema before parsing.
    rules_vector_t translateSQL(const string& sql,
                                const string& predName, unsigned arity,
                                bool& foundError, string& errorMessage) {
        // Register the predicate in the schema so schema lookup works
        context.defaultSchema_.createPredicate(&context, predName.c_str(), arity);

        ParserInputDirector pid(TEXT, context);
        int res = pid.parse(sql);
        EXPECT_EQ(res, 0);
        EXPECT_TRUE(pid.getBuilder()->isSQL());

        auto stmt = std::move(pid.getBuilder()->getSqlStatement());
        SqlToDatalog rewriter(context);
        return rewriter.sqlToDatalog(stmt, foundError, errorMessage);
    }

    // Overload without predicate pre-registration (for error cases)
    rules_vector_t translateSQL(const string& sql,
                                bool& foundError, string& errorMessage) {
        ParserInputDirector pid(TEXT, context);
        int res = pid.parse(sql);
        EXPECT_EQ(res, 0);
        EXPECT_TRUE(pid.getBuilder()->isSQL());

        auto stmt = std::move(pid.getBuilder()->getSqlStatement());
        SqlToDatalog rewriter(context);
        return rewriter.sqlToDatalog(stmt, foundError, errorMessage);
    }
};

// ---------------------------------------------------------------------------
// Parsing tests: verify grammar accepts new syntax
// ---------------------------------------------------------------------------

TEST_F(SqlPredicateFromTest, ParseFromPredicateWithArity) {
    ParserInputDirector pid(TEXT, context);
    // Should parse without error
    EXPECT_EQ(pid.parse("%@sql\nSELECT V1, V2 FROM ancestor/2\n"), 0);
}

TEST_F(SqlPredicateFromTest, ParseFromPredicateWithCustomCols) {
    ParserInputDirector pid(TEXT, context);
    EXPECT_EQ(pid.parse("%@sql\nSELECT X, Y FROM ancestor(X, Y)\n"), 0);
}

TEST_F(SqlPredicateFromTest, ParseFromPredicateWithArityAndAlias) {
    ParserInputDirector pid(TEXT, context);
    EXPECT_EQ(pid.parse("%@sql\nSELECT a.V1 FROM ancestor/2 AS a\n"), 0);
}

TEST_F(SqlPredicateFromTest, ParseFromPredicateWithCustomColsAndAlias) {
    ParserInputDirector pid(TEXT, context);
    EXPECT_EQ(pid.parse("%@sql\nSELECT a.X FROM ancestor(X, Y) AS a\n"), 0);
}

// ---------------------------------------------------------------------------
// Translation tests: verify generated Datalog rules
// ---------------------------------------------------------------------------

TEST_F(SqlPredicateFromTest, TranslateFromPredicateWithArity) {
    // Register ancestor/2 in schema
    context.defaultSchema_.createPredicate(&context, "ancestor", 2);

    ParserInputDirector pid(TEXT, context);
    EXPECT_EQ(pid.parse("%@sql\nSELECT V1, V2 FROM ancestor/2\n"), 0);
    EXPECT_TRUE(pid.getBuilder()->isSQL());

    auto stmt = std::move(pid.getBuilder()->getSqlStatement());
    SqlToDatalog rewriter(context);
    bool foundError = false;
    string errorMessage;
    auto rules = rewriter.sqlToDatalog(stmt, foundError, errorMessage);

    EXPECT_FALSE(foundError) << errorMessage;
    EXPECT_FALSE(rules.empty());

    // The last rule should reference "ancestor" in its body
    auto lastRuleStr = rules.back().toString();
    EXPECT_NE(lastRuleStr.find("ancestor"), string::npos)
        << "Generated rule should reference 'ancestor': " << lastRuleStr;
}

TEST_F(SqlPredicateFromTest, TranslateFromPredicateWithCustomCols) {
    // Register mypred/3 in schema
    context.defaultSchema_.createPredicate(&context, "mypred", 3);

    ParserInputDirector pid(TEXT, context);
    EXPECT_EQ(pid.parse("%@sql\nSELECT A, B, C FROM mypred(A, B, C)\n"), 0);
    EXPECT_TRUE(pid.getBuilder()->isSQL());

    auto stmt = std::move(pid.getBuilder()->getSqlStatement());
    SqlToDatalog rewriter(context);
    bool foundError = false;
    string errorMessage;
    auto rules = rewriter.sqlToDatalog(stmt, foundError, errorMessage);

    EXPECT_FALSE(foundError) << errorMessage;
    EXPECT_FALSE(rules.empty());

    // The rule should reference "mypred" with 3 terms
    auto lastRuleStr = rules.back().toString();
    EXPECT_NE(lastRuleStr.find("mypred"), string::npos)
        << "Generated rule should reference 'mypred': " << lastRuleStr;
}

TEST_F(SqlPredicateFromTest, TranslateFromPredicateAutoArityLookup) {
    // Register edge/2 — arity lookup should find it automatically
    context.defaultSchema_.createPredicate(&context, "edge", 2);

    ParserInputDirector pid(TEXT, context);
    // No /N suffix — schema lookup should find arity=2
    EXPECT_EQ(pid.parse("%@sql\nSELECT V1, V2 FROM edge\n"), 0);
    EXPECT_TRUE(pid.getBuilder()->isSQL());

    auto stmt = std::move(pid.getBuilder()->getSqlStatement());
    SqlToDatalog rewriter(context);
    bool foundError = false;
    string errorMessage;
    auto rules = rewriter.sqlToDatalog(stmt, foundError, errorMessage);

    EXPECT_FALSE(foundError) << errorMessage;
    EXPECT_FALSE(rules.empty());

    auto lastRuleStr = rules.back().toString();
    EXPECT_NE(lastRuleStr.find("edge"), string::npos)
        << "Generated rule should reference 'edge': " << lastRuleStr;
}

// ---------------------------------------------------------------------------
// Error tests
// ---------------------------------------------------------------------------

TEST_F(SqlPredicateFromTest, ErrorWhenPredicateNotFoundInSchema) {
    // Do not register any predicate — should return error
    ParserInputDirector pid(TEXT, context);
    EXPECT_EQ(pid.parse("%@sql\nSELECT V1 FROM nosuchpred\n"), 0);
    EXPECT_TRUE(pid.getBuilder()->isSQL());

    auto stmt = std::move(pid.getBuilder()->getSqlStatement());
    SqlToDatalog rewriter(context);
    bool foundError = false;
    string errorMessage;
    auto rules = rewriter.sqlToDatalog(stmt, foundError, errorMessage);

    EXPECT_TRUE(foundError);
    EXPECT_NE(errorMessage.find("not found"), string::npos)
        << "Error message should mention 'not found': " << errorMessage;
}

TEST_F(SqlPredicateFromTest, ErrorWhenPredicateArityIsAmbiguous) {
    // Register same predicate with two different arities
    context.defaultSchema_.createPredicate(&context, "foo", 1);
    context.defaultSchema_.createPredicate(&context, "foo", 2);

    ParserInputDirector pid(TEXT, context);
    EXPECT_EQ(pid.parse("%@sql\nSELECT V1 FROM foo\n"), 0);
    EXPECT_TRUE(pid.getBuilder()->isSQL());

    auto stmt = std::move(pid.getBuilder()->getSqlStatement());
    SqlToDatalog rewriter(context);
    bool foundError = false;
    string errorMessage;
    auto rules = rewriter.sqlToDatalog(stmt, foundError, errorMessage);

    EXPECT_TRUE(foundError);
    EXPECT_NE(errorMessage.find("ambiguous"), string::npos)
        << "Error message should mention 'ambiguous': " << errorMessage;
}