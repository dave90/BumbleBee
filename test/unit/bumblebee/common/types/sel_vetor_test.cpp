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
#include "bumblebee/common/types/SelectionVector.h"

using namespace bumblebee;

TEST(SelectionVectorTest, DefaultConstructor) {
    SelectionVector sv;
    EXPECT_EQ(sv.getData(), nullptr);
    EXPECT_EQ(sv.getIndex(5), 5);  // fallback if nullptr
    EXPECT_EQ(sv.toString(), "");
}

TEST(SelectionVectorTest, ConstructorWithCount) {
    idx_t count = 10;
    SelectionVector sv(count);

    ASSERT_NE(sv.getData(), nullptr);
    for (idx_t i = 0; i < count; ++i) {
        sv.setIndex(i, i * 2);
    }
    for (idx_t i = 0; i < count; ++i) {
        EXPECT_EQ(sv.getIndex(i), i * 2);
    }
}

TEST(SelectionVectorTest, ConstructorWithStartAndCount) {
    idx_t start = 100;
    idx_t count = 5;
    SelectionVector sv(start, count);

    for (idx_t i = 0; i < count; ++i) {
        EXPECT_EQ(sv.getIndex(i), start + i);
    }
}

TEST(SelectionVectorTest, ConstructorWithRawPointer) {
    sel_t buffer[5] = {3, 1, 4, 1, 5};
    SelectionVector sv(buffer);

    for (idx_t i = 0; i < 5; ++i) {
        EXPECT_EQ(sv.getIndex(i), buffer[i]);
    }
}

TEST(SelectionVectorTest, ConstructorWithSharedPointer) {
    sel_ptr_t ptr(new sel_t[3]{7, 8, 9});
    SelectionVector sv(ptr);

    EXPECT_EQ(sv.getSelData(), ptr);
}

TEST(SelectionVectorTest, CopyConstructor) {
    SelectionVector original(3);
    original.setIndex(0, 10);
    original.setIndex(1, 20);
    original.setIndex(2, 30);

    SelectionVector copy(original);
    for (idx_t i = 0; i < 3; ++i) {
        EXPECT_EQ(copy.getIndex(i), original.getIndex(i));
    }

    // Test shared memory (because it's shared_ptr)
    copy.setIndex(1, 99);
    EXPECT_EQ(original.getIndex(1), 99);
}

TEST(SelectionVectorTest, SwapFunction) {
    SelectionVector sv(2);
    sv.setIndex(0, 1);
    sv.setIndex(1, 2);

    sv.swap(0, 1);

    EXPECT_EQ(sv.getIndex(0), 2);
    EXPECT_EQ(sv.getIndex(1), 1);
}

TEST(SelectionVectorTest, SliceFunction) {
    SelectionVector base(5);
    for (idx_t i = 0; i < 5; ++i) base.setIndex(i, i * 10);

    SelectionVector selector(3);
    selector.setIndex(0, 1);
    selector.setIndex(1, 3);
    selector.setIndex(2, 4);

    auto sliced = base.slice(selector, 3);
    EXPECT_EQ(sliced[0], 10);  // base[1]
    EXPECT_EQ(sliced[1], 30);  // base[3]
    EXPECT_EQ(sliced[2], 40);  // base[4]
}

TEST(SelectionVectorTest, OperatorSquareBrackets) {
    SelectionVector sv(2);
    sv[0] = 100;
    sv[1] = 200;

    EXPECT_EQ(sv.getIndex(0), 100);
    EXPECT_EQ(sv.getIndex(1), 200);
}
