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

#include <filesystem>
#include <fstream>

#include "bumblebee/ClientContext.hpp"
#include "bumblebee/parser/ParserInputDirector.hpp"

using namespace bumblebee;

// End-to-end driver: lexer + grammar + ParserInputBuilder + catalog populate.
// Parses an ASP program containing a NULL literal and verifies the resulting
// catalog facts carry a typeless-null Term.
class ParserNullTest : public ::testing::Test {
protected:
    // Per-test owned schema: the default ClientContext() constructor wires
    // defaultSchema_ to the singleton Catalog instance — predicates accumulate
    // across tests, double-counting facts. Owning the schema isolates each test.
    ClientContext context{std::make_unique<Schema>("default")};
    std::vector<std::filesystem::path> tempFiles_;

    std::filesystem::path writeProgram(const std::string &name, const std::string &content) {
        auto path = std::filesystem::temp_directory_path() / name;
        std::ofstream ofs(path);
        ofs << content;
        ofs.close();
        tempFiles_.push_back(path);
        return path;
    }

    void TearDown() override {
        for (auto &f : tempFiles_) std::filesystem::remove(f);
    }

    // Find the predicate by `name/arity` in the default schema, returning nullptr if absent.
    Predicate *findPredicate(const std::string &name, unsigned arity) {
        for (auto p : context.defaultSchema_.getPredicates()) {
            if (p->getName() == name && p->getArity() == arity) return p;
        }
        return nullptr;
    }
};

TEST_F(ParserNullTest, ParsesNullLiteralIntoTypelessNullTerm) {
    auto path = writeProgram("null_lit.dl", "a(1,2).\na(3,NULL).\n");
    ParserInputDirector pid(TEXT, context);
    std::vector<std::string> files = {path.string()};
    ASSERT_EQ(pid.parse(files), 0);

    auto *pred = findPredicate("a", 2);
    ASSERT_NE(pred, nullptr);
    auto &tbl = context.defaultSchema_.getPredicateTable(pred);
    const auto &facts = tbl->getFacts();
    ASSERT_EQ(facts.size(), 2u);

    // first fact a(1,2): neither term is NULL
    EXPECT_FALSE(facts[0].getTerms()[0].isNull());
    EXPECT_FALSE(facts[0].getTerms()[1].isNull());

    // second fact a(3,NULL): only the second term is NULL, and it is a CONSTANT term
    EXPECT_FALSE(facts[1].getTerms()[0].isNull());
    EXPECT_TRUE(facts[1].getTerms()[1].isNull());
    EXPECT_EQ(facts[1].getTerms()[1].getType(), TermType::CONSTANT);
}

// Lower-case `null` is also accepted (both forms map to NULLKW in the lexer).
TEST_F(ParserNullTest, LowerCaseNullParses) {
    auto path = writeProgram("null_lower.dl", "a(1,2).\na(3,null).\n");
    ParserInputDirector pid(TEXT, context);
    std::vector<std::string> files = {path.string()};
    ASSERT_EQ(pid.parse(files), 0);

    auto *pred = findPredicate("a", 2);
    ASSERT_NE(pred, nullptr);
    const auto &facts = context.defaultSchema_.getPredicateTable(pred)->getFacts();
    ASSERT_EQ(facts.size(), 2u);
    EXPECT_TRUE(facts[1].getTerms()[1].isNull());
}

// `IS NULL` parses into a unary builtin atom with Binop::IS_NULL.
TEST_F(ParserNullTest, IsNullParses) {
    auto path = writeProgram("is_null.dl",
        "a(1,2).\na(3,NULL).\nmissing(X,Y) :- a(X,Y), Y IS NULL.\nmissing(X,Y)?\n");
    ParserInputDirector pid(TEXT, context);
    std::vector<std::string> files = {path.string()};
    ASSERT_EQ(pid.parse(files), 0);
    // The rule produced a head predicate `missing/2`.
    auto *p = findPredicate("missing", 2);
    ASSERT_NE(p, nullptr);
}

// Both `IS NOT NULL` (uppercase) and `is not` (lowercase) parse.
TEST_F(ParserNullTest, IsNotNullParsesBothCases) {
    auto path = writeProgram("is_not_null.dl",
        "a(1,2).\na(3,NULL).\np(X,Y) :- a(X,Y), Y IS NOT NULL.\n"
        "q(X,Y) :- a(X,Y), Y is not NULL.\np(X,Y)?\n");
    ParserInputDirector pid(TEXT, context);
    std::vector<std::string> files = {path.string()};
    ASSERT_EQ(pid.parse(files), 0);
    EXPECT_NE(findPredicate("p", 2), nullptr);
    EXPECT_NE(findPredicate("q", 2), nullptr);
}

// SQL `IS NULL` parses end-to-end: the SQL grammar's IS_KW NULLKW production +
// onSQLIsNullPredicate hook + SqlToDatalog::generateIsNullAtom all wire up.
// Mirrors sql_predicate_from_test: pre-register the predicate in the schema so
// SQL column resolution works, then parse the SQL string and check translation.
TEST_F(ParserNullTest, SqlIsNullParsesAndTranslates) {
    context.defaultSchema_.createPredicate(&context, "a", 2);
    ParserInputDirector pid(TEXT, context);
    ASSERT_EQ(pid.parse("%@sql\nSELECT X, Y FROM a(X, Y) WHERE Y IS NULL\n"), 0);
    EXPECT_TRUE(pid.getBuilder()->isSQL());
}

TEST_F(ParserNullTest, SqlIsNotNullParsesAndTranslates) {
    context.defaultSchema_.createPredicate(&context, "a", 2);
    ParserInputDirector pid(TEXT, context);
    ASSERT_EQ(pid.parse("%@sql\nSELECT X, Y FROM a(X, Y) WHERE Y IS NOT NULL\n"), 0);
    EXPECT_TRUE(pid.getBuilder()->isSQL());
}