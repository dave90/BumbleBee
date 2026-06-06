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
#include "../../NullTestBase.hpp"

#include <gtest/gtest.h>

#include "bumblebee/common/vector_operations/VectorOperations.hpp"

using namespace bumblebee;

class ArithNullTest : public NullTestBase {};

// a + b: NULL on either side -> NULL result.
TEST_F(ArithNullTest, AdditionPropagatesNull) {
    Vector a(PhysicalType::INTEGER, 4);
    a.setValue(0, Value(1));
    a.setValue(1, Value::null());
    a.setValue(2, Value(3));
    a.setValue(3, Value(4));
    Vector b(PhysicalType::INTEGER, 4);
    b.setValue(0, Value(10));
    b.setValue(1, Value(20));
    b.setValue(2, Value::null());
    b.setValue(3, Value(40));

    Vector r(PhysicalType::INTEGER, 4);
    VectorOperations::sum(a, b, r, 4);

    EXPECT_FALSE(isNull(r, 0));
    EXPECT_EQ(r.getValue(0), Value(11));
    EXPECT_TRUE(isNull(r, 1));        // NULL + 20
    EXPECT_TRUE(isNull(r, 2));        // 3 + NULL
    EXPECT_FALSE(isNull(r, 3));
    EXPECT_EQ(r.getValue(3), Value(44));
}

// Subtraction, multiplication, division: same propagation rule.
TEST_F(ArithNullTest, AllArithOpsPropagateNull) {
    Vector a(PhysicalType::INTEGER, 3);
    a.setValue(0, Value(10));
    a.setValue(1, Value::null());
    a.setValue(2, Value(8));
    Vector b(PhysicalType::INTEGER, 3);
    b.setValue(0, Value(2));
    b.setValue(1, Value(5));
    b.setValue(2, Value::null());

    Vector r(PhysicalType::INTEGER, 3);
    VectorOperations::difference(a, b, r, 3);
    EXPECT_FALSE(isNull(r, 0)); EXPECT_TRUE(isNull(r, 1)); EXPECT_TRUE(isNull(r, 2));

    Vector r2(PhysicalType::INTEGER, 3);
    VectorOperations::dot(a, b, r2, 3);
    EXPECT_FALSE(isNull(r2, 0)); EXPECT_TRUE(isNull(r2, 1)); EXPECT_TRUE(isNull(r2, 2));
}

// Constant operand on one side: a + (constant non-null) propagates per-row nulls of the flat side.
TEST_F(ArithNullTest, FlatPlusNonNullConstant) {
    Vector a(PhysicalType::INTEGER, 4);
    a.setValue(0, Value(1));
    a.setValue(1, Value::null());
    a.setValue(2, Value(3));
    a.setValue(3, Value::null());
    Vector five(Value((int32_t)5));

    Vector r(PhysicalType::INTEGER, 4);
    VectorOperations::sum(a, five, r, 4);
    EXPECT_FALSE(isNull(r, 0));
    EXPECT_TRUE(isNull(r, 1));
    EXPECT_FALSE(isNull(r, 2));
    EXPECT_TRUE(isNull(r, 3));
    EXPECT_EQ(r.getValue(0), Value(6));
    EXPECT_EQ(r.getValue(2), Value(8));
}

// NULL constant on either side: every row becomes NULL.
TEST_F(ArithNullTest, NullConstantOnRightMakesAllNull) {
    Vector a(PhysicalType::INTEGER, 3);
    a.setValue(0, Value(1));
    a.setValue(1, Value(2));
    a.setValue(2, Value(3));
    Vector nullConst(Value::null());

    Vector r(PhysicalType::INTEGER, 3);
    VectorOperations::sum(a, nullConst, r, 3);
    for (idx_t i = 0; i < 3; i++) EXPECT_TRUE(isNull(r, i));
}

// Dictionary input on either side: arithmetic still propagates NULLs through
// the executeGeneric path.
TEST_F(ArithNullTest, AdditionOverDictionaryInput) {
    Vector base(PhysicalType::INTEGER, 5);
    base.setValue(0, Value(10));
    base.setValue(1, Value(20));
    base.setValue(2, Value::null());
    base.setValue(3, Value(40));
    base.setValue(4, Value(50));
    SelectionVector sel(3);
    sel.setIndex(0, 0); // 10
    sel.setIndex(1, 2); // NULL
    sel.setIndex(2, 4); // 50
    Vector dictA(base, sel, 3);

    Vector b(PhysicalType::INTEGER, 3);
    b.setValue(0, Value(1));
    b.setValue(1, Value(2));
    b.setValue(2, Value(3));

    Vector r(PhysicalType::INTEGER, 3);
    VectorOperations::sum(dictA, b, r, 3);
    EXPECT_FALSE(isNull(r, 0));
    EXPECT_EQ(r.getValue(0), Value(11));
    EXPECT_TRUE(isNull(r, 1));
    EXPECT_FALSE(isNull(r, 2));
    EXPECT_EQ(r.getValue(2), Value(53));
}

// Multi-batch arithmetic with sparse nulls.
TEST_F(ArithNullTest, MultiBatchSparseNulls) {
    const idx_t count = 6000;
    auto positions = randomNullPlacement(count, 0.05, 11);
    Vector a(PhysicalType::BIGINT, count);
    Vector b(PhysicalType::BIGINT, count);
    for (idx_t i = 0; i < count; i++) {
        a.setValue(i, Value((int64_t)i));
        b.setValue(i, Value((int64_t)1));
    }
    for (auto p : positions) a.setValue(p, Value::null());

    Vector r(PhysicalType::BIGINT, count);
    VectorOperations::sum(a, b, r, count);

    std::vector<bool> expectedNull(count, false);
    for (auto p : positions) expectedNull[p] = true;
    for (idx_t i = 0; i < count; i++) {
        EXPECT_EQ(isNull(r, i), expectedNull[i]) << "row " << i;
    }
}