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
#include "bumblebee/catalog/PredicateTables.h"

using namespace bumblebee;

namespace {

// A subclass just for testing, defined only in the test
class PredicateTablesTester : public PredicateTables {
public:
    PredicateTablesTester(const char* name, unsigned arity)
        : PredicateTables(name, arity) {
        types_ = std::vector<ConstantType>(arity, UNKNOWN);
    }

    void callUpdateTypes(std::vector<ConstantType> newTypes) {
        updateTypes(newTypes);
    }

    const std::vector<ConstantType>& getTypes() const {
        return types_;
    }

};

class PredicateTablesTest : public ::testing::Test {
protected:
    void SetUp() override {
        table = std::make_unique<PredicateTablesTester>("test", 3);
    }

    std::unique_ptr<PredicateTablesTester> table;
};

TEST_F(PredicateTablesTest, NoChange) {
    std::vector<ConstantType> newTypes = {UNKNOWN, UNKNOWN, UNKNOWN};
    table->callUpdateTypes(newTypes);
    EXPECT_EQ(table->getTypes(), newTypes);
}

TEST_F(PredicateTablesTest, PromoteUnknown) {
    std::vector<ConstantType> newTypes = {TINYINT, SMALLINT, BIGINT};
    table->callUpdateTypes(newTypes);
    EXPECT_EQ(table->getTypes(), newTypes);
}

TEST_F(PredicateTablesTest, PromoteSmallerSize) {
    table->callUpdateTypes({SMALLINT, SMALLINT, SMALLINT});
    table->callUpdateTypes({INTEGER, INTEGER, UINTEGER});
    EXPECT_EQ(table->getTypes()[0], INTEGER);
    EXPECT_EQ(table->getTypes()[1], INTEGER);
    EXPECT_EQ(table->getTypes()[2], UINTEGER);
}

TEST_F(PredicateTablesTest, MixedSignednessSameSize) {
    table->callUpdateTypes({UINTEGER, INTEGER, UINTEGER});
    table->callUpdateTypes({INTEGER, INTEGER, INTEGER});
    EXPECT_EQ(table->getTypes()[0], BIGINT);  // bumped
    EXPECT_EQ(table->getTypes()[1], INTEGER);
    EXPECT_EQ(table->getTypes()[2], BIGINT);  // bumped
}

TEST_F(PredicateTablesTest, FloatToDoublePromotion) {
    table = std::make_unique<PredicateTablesTester>("test", 1);
    table->callUpdateTypes({FLOAT});
    table->callUpdateTypes({DOUBLE});
    EXPECT_EQ(table->getTypes()[0], DOUBLE);
}

TEST_F(PredicateTablesTest, TypeCountMismatchTriggersAssert) {
    std::vector<ConstantType> badSize = {INTEGER, INTEGER}; // only 2 types
    EXPECT_DEATH(table->callUpdateTypes(badSize), "Wrong number of terms for Fact");
}

}  // namespace
