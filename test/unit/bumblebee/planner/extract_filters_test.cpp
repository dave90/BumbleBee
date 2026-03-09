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
#include "bumblebee/planner/optimizer/PhysicalOptimizer.hpp"
#include "bumblebee/planner/filter/ConstantFilter.hpp"
#include "bumblebee/planner/filter/ConjunctionFilter.hpp"
#include "utils.h"

using namespace bumblebee;
using namespace std::filesystem;

static void assertConstantFilter(TableFilterSet& filters, idx_t colIdx, Binop expectedOp, Value expectedVal) {
    ASSERT_TRUE(filters.filters_.contains(colIdx)) << "No filter at column index " << colIdx;
    auto& filter = filters.filters_[colIdx];
    ASSERT_EQ(filter->filterType_, TableFilterType::CONSTANT_COMPARISON);
    auto& cf = dynamic_cast<ConstantFilter&>(*filter);
    EXPECT_EQ(cf.comparisonType_, expectedOp);
    EXPECT_EQ(cf.constant_, expectedVal);
}

// Test 1: Direct comparison - b(X,Y), X > 10
TEST(ExtractFiltersTest, DirectGreater) {
    auto program = getRulesFromFile("filter_rule1");
    ASSERT_EQ(program.size(), 1);
    auto& body = program[0].getBody();
    std::vector<std::string> names = {"X", "Y"};

    auto filters = PhysicalOptimizer::extractConstantFilters(body, names);

    EXPECT_EQ(filters.filters_.size(), 1);
    assertConstantFilter(filters, 0, GREATER, Value((uint8_t)10));
}

// Test 2: Multiple filters - b(X,Y), X > 10, Y < 20
TEST(ExtractFiltersTest, MultipleFilters) {
    auto program = getRulesFromFile("filter_rule2");
    ASSERT_EQ(program.size(), 1);
    auto& body = program[0].getBody();
    std::vector<std::string> names = {"X", "Y"};

    auto filters = PhysicalOptimizer::extractConstantFilters(body, names);

    EXPECT_EQ(filters.filters_.size(), 2);
    assertConstantFilter(filters, 0, GREATER, Value((uint8_t)10));
    assertConstantFilter(filters, 1, LESS, Value((uint8_t)20));
}

// Test 3: Chain resolution - C = 10, b(X,Y), X > C
TEST(ExtractFiltersTest, ChainResolution) {
    auto program = getRulesFromFile("filter_rule3");
    ASSERT_EQ(program.size(), 1);
    auto& body = program[0].getBody();
    std::vector<std::string> names = {"X", "Y"};

    auto filters = PhysicalOptimizer::extractConstantFilters(body, names);

    EXPECT_EQ(filters.filters_.size(), 1);
    assertConstantFilter(filters, 0, GREATER, Value((uint8_t)10));
}

// Test 4: Flipped operand - b(X,Y), 10 < X
TEST(ExtractFiltersTest, FlippedOperand) {
    auto program = getRulesFromFile("filter_rule4");
    ASSERT_EQ(program.size(), 1);
    auto& body = program[0].getBody();
    std::vector<std::string> names = {"X", "Y"};

    auto filters = PhysicalOptimizer::extractConstantFilters(body, names);

    EXPECT_EQ(filters.filters_.size(), 1);
    assertConstantFilter(filters, 0, GREATER, Value((uint8_t)10));
}

// Test 5: Equality filter - b(X,Y), X = 10
TEST(ExtractFiltersTest, EqualityFilter) {
    auto program = getRulesFromFile("filter_rule5");
    ASSERT_EQ(program.size(), 1);
    auto& body = program[0].getBody();
    std::vector<std::string> names = {"X", "Y"};

    auto filters = PhysicalOptimizer::extractConstantFilters(body, names);

    EXPECT_EQ(filters.filters_.size(), 1);
    assertConstantFilter(filters, 0, EQUAL, Value((uint8_t)10));
}

// Test 6: Same column, multiple filters - b(X,Y), X > 10, X < 100
// pushFilter creates a ConjunctionAndFilter when two filters target the same column
TEST(ExtractFiltersTest, SameColumnMultiFilter) {
    auto program = getRulesFromFile("filter_rule6");
    ASSERT_EQ(program.size(), 1);
    auto& body = program[0].getBody();
    std::vector<std::string> names = {"X", "Y"};

    auto filters = PhysicalOptimizer::extractConstantFilters(body, names);

    EXPECT_EQ(filters.filters_.size(), 1);
    ASSERT_TRUE(filters.filters_.contains(0));
    auto& filter = filters.filters_[0];
    ASSERT_EQ(filter->filterType_, TableFilterType::CONJUNCTION_AND);
    auto& andFilter = dynamic_cast<ConjunctionAndFilter&>(*filter);
    ASSERT_EQ(andFilter.childFilters_.size(), 2);

    auto& child0 = dynamic_cast<ConstantFilter&>(*andFilter.childFilters_[0]);
    EXPECT_EQ(child0.comparisonType_, GREATER);
    EXPECT_EQ(child0.constant_, Value((uint8_t)10));

    auto& child1 = dynamic_cast<ConstantFilter&>(*andFilter.childFilters_[1]);
    EXPECT_EQ(child1.comparisonType_, LESS);
    EXPECT_EQ(child1.constant_, Value((uint8_t)100));
}

// Test 7: No matching variable - b(X,Y), Z > 10
TEST(ExtractFiltersTest, NoMatchingVar) {
    auto program = getRulesFromFile("filter_rule7");
    ASSERT_EQ(program.size(), 1);
    auto& body = program[0].getBody();
    std::vector<std::string> names = {"X", "Y"};

    auto filters = PhysicalOptimizer::extractConstantFilters(body, names);

    EXPECT_EQ(filters.filters_.size(), 0);
}

// Test 8: Transitive chain - X=3, Y=X, b(K,W), K < Y
TEST(ExtractFiltersTest, TransitiveChain) {
    auto program = getRulesFromFile("filter_rule8");
    ASSERT_EQ(program.size(), 1);
    auto& body = program[0].getBody();
    std::vector<std::string> names = {"K", "W"};

    auto filters = PhysicalOptimizer::extractConstantFilters(body, names);

    EXPECT_EQ(filters.filters_.size(), 1);
    assertConstantFilter(filters, 0, LESS, Value((uint8_t)3));
}

// Test 9: Long transitive chain - X=3, Z=X, Y=Z, b(K,W), K < Y
TEST(ExtractFiltersTest, LongTransitiveChain) {
    auto program = getRulesFromFile("filter_rule9");
    ASSERT_EQ(program.size(), 1);
    auto& body = program[0].getBody();
    std::vector<std::string> names = {"K", "W"};

    auto filters = PhysicalOptimizer::extractConstantFilters(body, names);

    EXPECT_EQ(filters.filters_.size(), 1);
    assertConstantFilter(filters, 0, LESS, Value((uint8_t)3));
}

// Test 10: Wildcard skip - names contains "*"
TEST(ExtractFiltersTest, WildcardSkip) {
    auto program = getRulesFromFile("filter_rule1");
    ASSERT_EQ(program.size(), 1);
    auto& body = program[0].getBody();
    std::vector<std::string> names = {"*"};

    auto filters = PhysicalOptimizer::extractConstantFilters(body, names);

    EXPECT_EQ(filters.filters_.size(), 0);
}

// Test 11: Empty body
TEST(ExtractFiltersTest, EmptyBody) {
    atoms_vector_t emptyBody;
    std::vector<std::string> names = {"X", "Y"};

    auto filters = PhysicalOptimizer::extractConstantFilters(emptyBody, names);

    EXPECT_EQ(filters.filters_.size(), 0);
}

// Test 12: No builtins in body - only classical atom b(X,Y)
TEST(ExtractFiltersTest, NoBuiltins) {
    auto program = getRulesFromFile("filter_rule1");
    ASSERT_EQ(program.size(), 1);
    // Use only the first atom (the classical atom b(X,Y)), skip the builtin
    atoms_vector_t bodyNoBuiltins;
    bodyNoBuiltins.push_back(std::move(program[0].getBody()[0]));
    std::vector<std::string> names = {"X", "Y"};

    auto filters = PhysicalOptimizer::extractConstantFilters(bodyNoBuiltins, names);

    EXPECT_EQ(filters.filters_.size(), 0);
}

// Test 13: Unequal filter - b(X,Y), X != 10
TEST(ExtractFiltersTest, UnequalFilter) {
    auto program = getRulesFromFile("filter_rule10");
    ASSERT_EQ(program.size(), 1);
    auto& body = program[0].getBody();
    std::vector<std::string> names = {"X", "Y"};

    auto filters = PhysicalOptimizer::extractConstantFilters(body, names);

    EXPECT_EQ(filters.filters_.size(), 1);
    assertConstantFilter(filters, 0, UNEQUAL, Value((uint8_t)10));
}