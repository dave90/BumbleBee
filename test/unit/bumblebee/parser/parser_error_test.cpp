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
#include <fstream>
#include <filesystem>

#include "bumblebee/ClientContext.hpp"
#include "bumblebee/output/OutputBuilder.hpp"
#include "bumblebee/parser/ParserInputDirector.hpp"
#include "bumblebee/common/Log.hpp"

using namespace bumblebee;

class ParserErrorTest : public ::testing::Test {
protected:
    ClientContext context;

    std::filesystem::path createTempFile(const std::string& name, const std::string& content) {
        auto path = std::filesystem::temp_directory_path() / name;
        std::ofstream ofs(path);
        ofs << content;
        ofs.close();
        tempFiles_.push_back(path);
        return path;
    }

    std::filesystem::path initTestLogger() {
        auto logPath = std::filesystem::temp_directory_path() / "bumble_test.log";
        tempFiles_.push_back(logPath);
        std::filesystem::remove(logPath);
        init_logger(logPath.c_str(), 0);
        return logPath;
    }

    std::string readLogFile(const std::filesystem::path& logPath) {
        close_logger();
        std::ifstream ifs(logPath);
        return std::string((std::istreambuf_iterator<char>(ifs)),
                            std::istreambuf_iterator<char>());
    }

    // Verify that parsing the given input produces an error whose log output
    // contains each of the expected substrings (source line, line number, etc.)
    void expectErrorContext(const std::string& fileName,
                           const std::string& input,
                           const std::vector<std::string>& expectedSubstrings) {
        auto logPath = initTestLogger();
        auto path = createTempFile(fileName, input);
        ParserInputDirector pid(TEXT, context);
        int result = pid.parse({path.string()});
        EXPECT_EQ(result, -1);

        std::string log = readLogFile(logPath);
        for (const auto& expected : expectedSubstrings) {
            EXPECT_NE(log.find(expected), std::string::npos)
                << "Log should contain: \"" << expected << "\"\nActual log:\n" << log;
        }
    }

    void TearDown() override {
        close_logger();
        for (auto& f : tempFiles_) {
            std::filesystem::remove(f);
        }
    }

    std::vector<std::filesystem::path> tempFiles_;
};

// ---------------------------------------------------------------------------
// Column tracking unit tests
// ---------------------------------------------------------------------------

TEST_F(ParserErrorTest, ColumnInitiallyZero) {
    ParserInputDirector pid(TEXT, context);
    EXPECT_EQ(pid.getColumn(), 0);
}

TEST_F(ParserErrorTest, AdvanceColumnIncrementsPosition) {
    ParserInputDirector pid(TEXT, context);
    pid.advanceColumn(5);
    EXPECT_EQ(pid.getColumn(), 5);
    pid.advanceColumn(3);
    EXPECT_EQ(pid.getColumn(), 8);
}

TEST_F(ParserErrorTest, SetTokenStartRecordsCurrentColumn) {
    ParserInputDirector pid(TEXT, context);
    pid.advanceColumn(10);
    pid.setTokenStart();
    pid.advanceColumn(5);
    EXPECT_EQ(pid.getColumn(), 15);
}

TEST_F(ParserErrorTest, OnNewLineResetsColumn) {
    ParserInputDirector pid(TEXT, context);
    pid.advanceColumn(10);
    EXPECT_EQ(pid.getColumn(), 10);
    pid.onNewLine();
    EXPECT_EQ(pid.getColumn(), 0);
}

TEST_F(ParserErrorTest, MultipleNewLinesResetColumn) {
    ParserInputDirector pid(TEXT, context);
    pid.advanceColumn(5);
    pid.onNewLine();
    EXPECT_EQ(pid.getColumn(), 0);
    pid.advanceColumn(3);
    EXPECT_EQ(pid.getColumn(), 3);
    pid.onNewLine();
    EXPECT_EQ(pid.getColumn(), 0);
}

// ---------------------------------------------------------------------------
// Valid Datalog parsing tests
// ---------------------------------------------------------------------------

TEST_F(ParserErrorTest, ParseSimpleFact) {
    auto path = createTempFile("valid_fact.dl", "a(1,2).\n");
    ParserInputDirector pid(TEXT, context);
    EXPECT_EQ(pid.parse({path.string()}), 0);
}

TEST_F(ParserErrorTest, ParseMultipleFacts) {
    auto path = createTempFile("multi_facts.dl", "a(1).\nb(2).\nc(3,4,5).\n");
    ParserInputDirector pid(TEXT, context);
    EXPECT_EQ(pid.parse({path.string()}), 0);
}

TEST_F(ParserErrorTest, ParseSimpleRule) {
    auto path = createTempFile("valid_rule.dl", "b(X,Y) :- a(X,Y).\n");
    ParserInputDirector pid(TEXT, context);
    EXPECT_EQ(pid.parse({path.string()}), 0);
}

TEST_F(ParserErrorTest, ParseRuleWithMultipleBodyAtoms) {
    auto path = createTempFile("multi_body.dl", "c(X,Z) :- a(X,Y), b(Y,Z).\n");
    ParserInputDirector pid(TEXT, context);
    EXPECT_EQ(pid.parse({path.string()}), 0);
}

TEST_F(ParserErrorTest, ParseRuleWithBuiltins) {
    auto path = createTempFile("builtins.dl", "b(X) :- a(X), X > 10.\n");
    ParserInputDirector pid(TEXT, context);
    EXPECT_EQ(pid.parse({path.string()}), 0);
}

TEST_F(ParserErrorTest, ParseRuleWithStringConstant) {
    auto path = createTempFile("string_const.dl", "a(\"hello\",1).\n");
    ParserInputDirector pid(TEXT, context);
    EXPECT_EQ(pid.parse({path.string()}), 0);
}

TEST_F(ParserErrorTest, ParseRuleWithNegation) {
    auto path = createTempFile("negation.dl", "b(X) :- a(X), not c(X).\n");
    ParserInputDirector pid(TEXT, context);
    EXPECT_EQ(pid.parse({path.string()}), 0);
}

TEST_F(ParserErrorTest, ParseRuleWithAnonymousVar) {
    auto path = createTempFile("anon_var.dl", "b(X) :- a(X,_).\n");
    ParserInputDirector pid(TEXT, context);
    EXPECT_EQ(pid.parse({path.string()}), 0);
}

TEST_F(ParserErrorTest, ParseWithComments) {
    auto path = createTempFile("comments.dl",
        "% this is a comment\na(1).\n%another comment\nb(2).\n");
    ParserInputDirector pid(TEXT, context);
    EXPECT_EQ(pid.parse({path.string()}), 0);
}

TEST_F(ParserErrorTest, ParseEmptyFileReturnsError) {
    auto path = createTempFile("empty.dl", "");
    ParserInputDirector pid(TEXT, context);
    EXPECT_EQ(pid.parse({path.string()}), -1);
}

TEST_F(ParserErrorTest, ParseRuleWithArithmetic) {
    auto path = createTempFile("arith.dl", "b(X,Z) :- a(X,Y), Z = X + Y.\n");
    ParserInputDirector pid(TEXT, context);
    EXPECT_EQ(pid.parse({path.string()}), 0);
}

TEST_F(ParserErrorTest, ParseMultipleRulesAndFacts) {
    auto path = createTempFile("mixed.dl",
        "a(1,2).\na(3,4).\nb(X,Y) :- a(X,Y).\nc(X) :- b(X,_), X > 1.\n");
    ParserInputDirector pid(TEXT, context);
    EXPECT_EQ(pid.parse({path.string()}), 0);
}

TEST_F(ParserErrorTest, ParseValidSql) {
    auto path = createTempFile("valid_sql.dl", "%@sql\nSELECT * FROM a\n");
    ParserInputDirector pid(TEXT, context);
    EXPECT_EQ(pid.parse({path.string()}), 0);
}

// ---------------------------------------------------------------------------
// Error detection tests
// ---------------------------------------------------------------------------

TEST_F(ParserErrorTest, MissingCommaReturnsError) {
    auto path = createTempFile("missing_comma.dl", "a(X Y).\n");
    ParserInputDirector pid(TEXT, context);
    EXPECT_EQ(pid.parse({path.string()}), -1);
}

TEST_F(ParserErrorTest, MissingDotReturnsError) {
    auto path = createTempFile("missing_dot.dl", "a(X,Y)\n");
    ParserInputDirector pid(TEXT, context);
    EXPECT_EQ(pid.parse({path.string()}), -1);
}

TEST_F(ParserErrorTest, MissingCommaInBodyReturnsError) {
    auto path = createTempFile("body_err.dl", "c(X) :- a(X Y).\n");
    ParserInputDirector pid(TEXT, context);
    EXPECT_EQ(pid.parse({path.string()}), -1);
}

TEST_F(ParserErrorTest, InvalidSqlReturnsError) {
    auto path = createTempFile("bad_sql.dl", "%@sql\nSELECT * FORM t\n");
    ParserInputDirector pid(TEXT, context);
    EXPECT_EQ(pid.parse({path.string()}), -1);
}

TEST_F(ParserErrorTest, NonExistentFileReturnsError) {
    ParserInputDirector pid(TEXT, context);
    EXPECT_EQ(pid.parse({"/tmp/nonexistent_bumble_test_xyz.dl"}), -1);
}

TEST_F(ParserErrorTest, ErrorOnLaterLine) {
    auto path = createTempFile("err_line3.dl", "a(1).\nb(2).\nc(X Y).\n");
    ParserInputDirector pid(TEXT, context);
    EXPECT_EQ(pid.parse({path.string()}), -1);
}

// ---------------------------------------------------------------------------
// Error context output tests (verify LOG_ERROR contains line + caret)
// ---------------------------------------------------------------------------

TEST_F(ParserErrorTest, ErrorContextMissingComma) {
    expectErrorContext("ctx_comma.dl", "a(X Y).\n",
        {"line 1", "a(X Y).", "^"});
}

TEST_F(ParserErrorTest, ErrorContextMissingDot) {
    expectErrorContext("ctx_dot.dl", "a(X,Y)\n",
        {"a(X,Y)", "^"});
}

TEST_F(ParserErrorTest, ErrorContextBodyError) {
    expectErrorContext("ctx_body.dl", "b(X) :- a(X Y).\n",
        {"line 1", "b(X) :- a(X Y).", "^"});
}

TEST_F(ParserErrorTest, ErrorContextSqlError) {
    expectErrorContext("ctx_sql.dl", "%@sql\nSELECT * FORM t\n",
        {"line 2", "SELECT * FORM t", "^"});
}

TEST_F(ParserErrorTest, ErrorContextThirdLine) {
    expectErrorContext("ctx_line3.dl", "a(1).\nb(2).\nc(X Y).\n",
        {"line 3", "c(X Y).", "^"});
}

TEST_F(ParserErrorTest, ErrorContextContainsFilename) {
    expectErrorContext("ctx_fname.dl", "a(X Y).\n",
        {"ctx_fname.dl"});
}
