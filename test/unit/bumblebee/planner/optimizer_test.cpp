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
#include "bumblebee/planner/optimizer/PhysicalOptimizer.hpp"
#include "bumblebee/planner/rewriter/FilterPushDownRewriter.hpp"

using namespace bumblebee;
using namespace std;
using namespace filesystem;



TEST(OptimizerTest, OptimizeSimpleRule) {
    ParserInputDirector pid(TEXT, true, false);

    auto program = getRulesFromFile("program1");
    EXPECT_EQ(program.size(), 1);
    auto& rule = program[0];

    string beforeRewriting = rule.toString();
    std::cout << beforeRewriting << std::endl;

    ClientContext context;
    PhysicalOptimizer optimizer(context);
    auto prules = optimizer.optimize(rule);
    EXPECT_EQ(prules.size(), 1);
    auto prule = prules[0];
    std::cout << prule->toString() << std::endl;
    EXPECT_EQ(prule->getPhysicalAtomsSize(), 0);
}

TEST(OptimizerTest, OptimizeBuiltinRule) {
    ParserInputDirector pid(TEXT, true , false);

    auto program = getRulesFromFile("program2");
    EXPECT_EQ(program.size(), 1);
    auto& rule = program[0];

    // set first binop as assignment
    rule.getBody()[1].setBinop(ASSIGNMENT);
    string beforeRewriting = rule.toString();
    std::cout << beforeRewriting << std::endl;

    ClientContext context;
    PhysicalOptimizer optimizer(context);
    auto prules = optimizer.optimize(rule);
    EXPECT_EQ(prules.size(), 1);
    auto prule = prules[0];
    std::cout << prule->toString() << std::endl;
    EXPECT_EQ(prule->getPhysicalAtomsSize(), 3);
}


TEST(OptimizerTest, OptimizeCrossProductRule) {
    ParserInputDirector pid(TEXT, true, false);

    auto program = getRulesFromFile("program3");
    EXPECT_EQ(program.size(), 1);
    auto& rule = program[0];

    // set first binop as assignment
    string beforeRewriting = rule.toString();
    std::cout << beforeRewriting << std::endl;

    ClientContext context;
    PhysicalOptimizer optimizer(context);
    auto prules = optimizer.optimize(rule);
    EXPECT_EQ(prules.size(), 1);
    auto prule = prules[0];
    std::cout << prule->toString() << std::endl;
}