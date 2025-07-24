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

#include "bumblebee/common/Constants.h"
#include "bumblebee/parser/ParserInputDirector.h"
#include "bumblebee/parser/statement/Atom.h"
#include "bumblebee/parser/statement/Predicate.h"
#include "bumblebee/parser/statement/Rule.h"
#include "bumblebee/planner/rewriter/ArithRewriter.h"
#include "bumblebee/planner/rewriter/VariablesRewriter.h"
#include "utils.h"

using namespace bumblebee;
using namespace std;
using namespace filesystem;



TEST(ArithRewriterTest, NoRewriting) {
    ParserInputDirector pid(TEXT);

    auto program = getRulesFromFile("rule3");
    EXPECT_EQ(program.size(), 1);
    auto& rule = program[0];

    string beforeRewriting = rule.toString();
    std::cout << beforeRewriting << std::endl;

    ArithRewriter rewriter;
    rewriter.rewrite(rule);
    EXPECT_EQ(rule.toString(), beforeRewriting);
}

TEST(ArithRewriterTest, RewriteArith) {
    ParserInputDirector pid(TEXT);

    auto program = getRulesFromFile("rule4");
    EXPECT_EQ(program.size(), 1);
    auto& rule = program[0];

    string beforeRewriting = rule.toString();
    std::cout << beforeRewriting << std::endl;

    ArithRewriter rewriter;
    rewriter.rewrite(rule);
    std::cout << rule.toString() << std::endl;
    EXPECT_NE(rule.toString(), beforeRewriting);
    EXPECT_EQ(rule.getBody().size(), 5);
}
