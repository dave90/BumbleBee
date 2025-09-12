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

#include "bumblebee/common/Constants.hpp"
#include "bumblebee/common/types/Vector.hpp"
#include "bumblebee/common/vector_operations/VectorOperations.hpp"

using namespace bumblebee;

class VectorOperationsArithTest : public ::testing::Test {
protected:

    template <class T>
    void setValuesVector(Vector &v, vector<T> values ) {
        for (idx_t i = 0; i < values.size(); i++) {
            v.setValue(i, values[i]);
        }
    }

    template <class T>
    Vector generateVector(ConstantType type, vector<T> values ) {
        Vector v1(type,values.size());
        setValuesVector<T>(v1,values);
        return v1;
    }

};

TEST_F(VectorOperationsArithTest, SumFlatVectors) {
    vector<int32_t> values = {1,2,3,4};
    Vector v1 = generateVector(INTEGER, values);
    Vector v2 = generateVector(INTEGER, values);
    Vector v3(INTEGER);

    VectorOperations::sum(v1,v2,v3,values.size());
    // expected sum
    for (idx_t i = 0; i < values.size(); i++) {
        EXPECT_EQ(values[i] + values[i], v3.getValue(i).getNumericValue<int32_t>());
    }

}

TEST_F(VectorOperationsArithTest, DifferenceFlatVectors) {
    vector<int16_t> values1 = {10, 20, -30, 40};
    vector<int16_t> values2 = {1, 5, -10, 50};
    Vector v1 = generateVector(SMALLINT, values1);
    Vector v2 = generateVector(SMALLINT, values2);
    Vector v3(SMALLINT);

    VectorOperations::difference(v1, v2, v3, values1.size());
    for (idx_t i = 0; i < values1.size(); i++) {
        EXPECT_EQ(values1[i] - values2[i], v3.getValue(i).getNumericValue<int16_t>());
    }
}

TEST_F(VectorOperationsArithTest, DotCrossTypeVectors) {
    vector<uint32_t> values1 = {10, 20, 30, 40};
    vector<float> values2 = {1.5f, -2.5f, 3.0f, 0.5f};
    Vector v1 = generateVector(UINTEGER, values1);
    Vector v2 = generateVector(FLOAT, values2);
    Vector v3(DOUBLE); // result type must be wide enough (DOUBLE here)

    VectorOperations::dot(v1, v2, v3, values1.size());
    std::cout << v3.toString(4) << std::endl;

    for (idx_t i = 0; i < values1.size(); i++) {
        EXPECT_EQ(static_cast<double>(values1[i]) * static_cast<double>(values2[i]), v3.getValue(i).getNumericValue<double>());
    }
}

// We make the result type match the LEFT vector’s type, since dispatch is on the left.
TEST_F(VectorOperationsArithTest, AndCrossTypeVectors) {
    vector<uint32_t> left_vals  = {
        0xFFFFFFFFu, // 11111111 11111111 11111111 11111111  -> all bits set (32 ones)
        0xABCDEF01u, // 10101011 11001101 11101111 00000001  -> mixed pattern, high bits set
        0x0000FFFFu, // 00000000 00000000 11111111 11111111  -> lower 16 bits set, upper 16 bits clear
        0x13579BDFu  // 00010011 01010111 10011011 11011111  -> "checkerboard-ish" odd/even bits set
    };
    // Use values that fit in int16_t:
    // 0x00FF  ->  255
    // 0x0F0F  ->  3855
    // 0xF0F0  -> -3856  (two's complement)
    // -1      -> -1
    vector<int16_t>  right_vals = {255, 3855, -3856, 1};

    Vector v_left  = generateVector(UINTEGER, left_vals);
    Vector v_right = generateVector(SMALLINT, right_vals);
    Vector v_out(UINTEGER); // result wide enough and aligned with LEFT type

    VectorOperations::lAnd(v_left, v_right, v_out, left_vals.size());

    for (idx_t i = 0; i < left_vals.size(); i++) {
        // Explicitly cast RIGHT to the LEFT's representation before &,
        // mirroring what a left-dispatched implementation would effectively do.
        auto rhs_as_left = static_cast<uint32_t>(static_cast<uint16_t>(right_vals[i]));
        uint32_t expected = left_vals[i] & rhs_as_left;
        EXPECT_EQ(expected, v_out.getValue(i).getNumericValue<uint32_t>())
            << "Mismatch at index " << i;
    }
}


TEST_F(VectorOperationsArithTest, AndCrossTypeConstantVectors) {
    vector<uint32_t> left_vals  = {
        0xFFFFFFFFu, // 11111111 11111111 11111111 11111111  -> all bits set (32 ones)
        0xABCDEF01u, // 10101011 11001101 11101111 00000001  -> mixed pattern, high bits set
        0x0000FFFFu, // 00000000 00000000 11111111 11111111  -> lower 16 bits set, upper 16 bits clear
        0x13579BDFu  // 00010011 01010111 10011011 11011111  -> "checkerboard-ish" odd/even bits set
    };
    // Use values that fit in int16_t:
    // 0x00FF  ->  255
    int16_t  right_vals = 255;

    Vector v_left  = generateVector(UINTEGER, left_vals);
    Vector v_right(right_vals);
    EXPECT_EQ(v_right.getVectorType() , VectorType::CONSTANT_VECTOR);

    Vector v_out(UINTEGER); // result wide enough and aligned with LEFT type

    VectorOperations::lAnd(v_left, v_right, v_out, left_vals.size());

    for (idx_t i = 0; i < left_vals.size(); i++) {
        // Explicitly cast RIGHT to the LEFT's representation before &,
        // mirroring what a left-dispatched implementation would effectively do.
        auto rhs_as_left = static_cast<uint32_t>(static_cast<uint16_t>(right_vals));
        uint32_t expected = left_vals[i] & right_vals;
        EXPECT_EQ(expected, v_out.getValue(i).getNumericValue<uint32_t>())
            << "Mismatch at index " << i;
    }
}