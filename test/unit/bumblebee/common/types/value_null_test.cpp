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

#include "bumblebee/common/types/Value.hpp"

using namespace bumblebee;

TEST(ValueNullTest, NullConstructsNull) {
    Value v = Value::null();
    EXPECT_TRUE(v.isNull());
    EXPECT_EQ(v.toString(), "NULL");
}

TEST(ValueNullTest, NonNullReportsNotNull) {
    Value v(42);
    EXPECT_FALSE(v.isNull());
    EXPECT_EQ(v.toString(), "42");
}

TEST(ValueNullTest, NullEqualsNull) {
    Value a = Value::null();
    Value b = Value::null();
    EXPECT_EQ(a, b);
}

TEST(ValueNullTest, NullNotEqualNonNull) {
    Value a = Value::null();
    Value b(0);
    EXPECT_NE(a, b);
    EXPECT_NE(b, a);
}

TEST(ValueNullTest, NullSurvivesMove) {
    Value a = Value::null();
    Value b = std::move(a);
    EXPECT_TRUE(b.isNull());
}