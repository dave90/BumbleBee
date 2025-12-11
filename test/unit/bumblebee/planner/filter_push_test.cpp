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

#include "bumblebee/parser/ParserInputDirector.hpp"
#include "bumblebee/parser/statement/Rule.hpp"
#include "utils.h"
#include "bumblebee/planner/rewriter/ArithRewriter.hpp"
#include "bumblebee/planner/rewriter/FilterPushDownRewriter.hpp"

using namespace bumblebee;
using namespace std;
using namespace filesystem;



TEST(FilterRewriterTest, NoRewriting) {

    auto program = getRulesFromFile("rule3");
    EXPECT_EQ(program.size(), 1);
    auto& rule = program[0];

    string beforeRewriting = rule.toString();
    std::cout << beforeRewriting << std::endl;

    FilterPushDownRewriter rewriter;
    rewriter.rewrite(rule);
    EXPECT_EQ(rule.toString(), beforeRewriting);
}

TEST(FilterRewriterTest, OrderBuiltin) {

    auto program = getRulesFromFile("rule5");
    EXPECT_EQ(program.size(), 1);
    auto& rule = program[0];

    string beforeRewriting = rule.toString();
    std::cout << beforeRewriting << std::endl;

    FilterPushDownRewriter rewriter;
    rewriter.rewrite(rule);
    std::cout << rule.toString() << std::endl;
    EXPECT_NE(rule.toString(), beforeRewriting);
    EXPECT_EQ(rule.toString(), "a(X,Y) :- b(X,Y,T), Z = X, O = T+Z, X == Y, b(Z,100,W), W > 100.");
}

TEST(FilterRewriterTest, OrderOneBuiltin) {

    auto program = getRulesFromFile("rule6");
    EXPECT_EQ(program.size(), 1);
    auto& rule = program[0];

    string beforeRewriting = rule.toString();
    std::cout << beforeRewriting << std::endl;

    ArithRewriter arithRewriter;
    FilterPushDownRewriter filterRewriter;
    arithRewriter.rewrite(rule);
    std::cout << rule.toString() << std::endl;
    filterRewriter.rewrite(rule);
    std::cout << rule.toString() << std::endl;
    EXPECT_NE(rule.toString(), beforeRewriting);
}