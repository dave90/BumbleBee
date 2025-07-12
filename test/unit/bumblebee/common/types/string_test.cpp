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
#include "bumblebee/common/types/BumbleString.h"

using namespace bumblebee;

TEST(BumbleStringTest, InlineConstructorShortString) {
    const char* str = "short";  // length = 5
    BumbleString s(str);

    EXPECT_EQ(s.length(), 5);
    EXPECT_TRUE(s.isInlined());
    EXPECT_STREQ(s.c_str(), str);
    EXPECT_STREQ(s.getString().c_str(), str);
    EXPECT_EQ(strcmp(s.getDataUnsafe(), str), 0);
    EXPECT_EQ(strcmp(s.getPrefix(), str), 0);
}

TEST(BumbleStringTest, NonInlineConstructorLongString) {
    const char* str = "this_is_a_longer_string"; // > 11
    BumbleString s(str);

    EXPECT_EQ(s.length(), strlen(str));
    EXPECT_FALSE(s.isInlined());
    EXPECT_STREQ(s.c_str(), str);
    EXPECT_EQ(s.getString(), std::string(str));

    // prefix should contain the first 11 chars
    std::string expected_prefix(str, BumbleString::PREFIX_LENGTH);
    EXPECT_EQ(std::string(s.getPrefix()), expected_prefix);
}

TEST(BumbleStringTest, ConstructorWithExplicitLength) {
    const char* str = "123456789012345"; // len = 15
    BumbleString s(str, 15);

    EXPECT_EQ(s.length(), 15);
    EXPECT_FALSE(s.isInlined());
    EXPECT_STREQ(s.c_str(), str);
    EXPECT_EQ(s.getString(), std::string(str, 15));
}

TEST(BumbleStringTest, CopyConstructor) {
    const char* str = "inline_cp";
    BumbleString original(str);
    BumbleString copy(original);

    EXPECT_EQ(copy.length(), original.length());
    EXPECT_STREQ(copy.c_str(), original.c_str());
    EXPECT_EQ(copy.getString(), original.getString());
    EXPECT_EQ(copy.isInlined(), original.isInlined());
}

TEST(BumbleStringTest, ComparisonOperator) {
    BumbleString a("apple");
    BumbleString b("banana");

    EXPECT_TRUE(a < b);
    EXPECT_FALSE(b < a);

    BumbleString c("apple");
    EXPECT_FALSE(a < c);
    EXPECT_FALSE(c < a);
}

TEST(BumbleStringTest, GetDataWriteableReturnsSameAsUnsafe) {
    const char* str = "shorty";
    BumbleString s(str);

    EXPECT_STREQ(s.getDataUnsafe(), s.getDataWriteable());
}

TEST(BumbleStringTest, StaticIsInlinedFunction) {
    EXPECT_TRUE(BumbleString::isInlined(0));
    EXPECT_TRUE(BumbleString::isInlined(11));
    EXPECT_FALSE(BumbleString::isInlined(12));
    EXPECT_FALSE(BumbleString::isInlined(100));
}
