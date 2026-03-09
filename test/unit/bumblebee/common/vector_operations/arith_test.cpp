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

#include "../../BumbleBaseTest.hpp"
#include "bumblebee/common/Constants.hpp"
#include "bumblebee/common/types/Decimal.hpp"
#include "bumblebee/common/types/Vector.hpp"
#include "bumblebee/common/vector_operations/VectorOperations.hpp"

using namespace bumblebee;

class VectorOperationsArithTest : public BumbleBaseTest {


};

TEST_F(VectorOperationsArithTest, SumFlatVectors) {
    vector<int32_t> values = {1,2,3,4};
    Vector v1 = generateVector(PhysicalType::INTEGER, values);
    Vector v2 = generateVector(PhysicalType::INTEGER, values);
    Vector v3(PhysicalType::INTEGER);

    VectorOperations::sum(v1,v2,v3,values.size());
    // expected sum
    for (idx_t i = 0; i < values.size(); i++) {
        EXPECT_EQ(values[i] + values[i], v3.getValue(i).getNumericValue<int32_t>());
    }

}

TEST_F(VectorOperationsArithTest, DifferenceFlatVectors) {
    vector<int16_t> values1 = {10, 20, -30, 40};
    vector<int16_t> values2 = {1, 5, -10, 50};
    Vector v1 = generateVector(PhysicalType::SMALLINT, values1);
    Vector v2 = generateVector(PhysicalType::SMALLINT, values2);
    Vector v3(PhysicalType::SMALLINT);

    VectorOperations::difference(v1, v2, v3, values1.size());
    for (idx_t i = 0; i < values1.size(); i++) {
        EXPECT_EQ(values1[i] - values2[i], v3.getValue(i).getNumericValue<int16_t>());
    }
}

TEST_F(VectorOperationsArithTest, DotCrossTypeVectors) {
    vector<uint32_t> values1 = {10, 20, 30, 40};
    vector<float> values2 = {1.5f, -2.5f, 3.0f, 0.5f};
    Vector v1 = generateVector(PhysicalType::UINTEGER, values1);
    Vector v2 = generateVector(PhysicalType::FLOAT, values2);
    Vector v3(PhysicalType::DOUBLE); // result type must be wide enough (DOUBLE here)

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

    Vector v_left  = generateVector(PhysicalType::UINTEGER, left_vals);
    Vector v_right = generateVector(PhysicalType::SMALLINT, right_vals);
    Vector v_out(PhysicalType::UINTEGER); // result wide enough and aligned with LEFT type

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

    Vector v_left  = generateVector(PhysicalType::UINTEGER, left_vals);
    Vector v_right(right_vals);
    EXPECT_EQ(v_right.getVectorType() , VectorType::CONSTANT_VECTOR);

    Vector v_out(PhysicalType::UINTEGER); // result wide enough and aligned with LEFT type

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


TEST_F(VectorOperationsArithTest, NegateIntVectors) {
    vector<int16_t> values1 = {10, 20, -30, 40};
    Vector v1 = generateVector(PhysicalType::SMALLINT, values1);
    Vector v3(PhysicalType::SMALLINT);

    VectorOperations::negate(v1, v3, values1.size());
    for (idx_t i = 0; i < values1.size(); i++) {
        EXPECT_EQ(values1[i] * -1, v3.getValue(i).getNumericValue<int16_t>());
    }
}


TEST_F(VectorOperationsArithTest, NegateDecimalVectors) {
    vector<int16_t> values1 = {10, 20, -30, 40};
    int scale = 2;
    auto type = LogicalType::createDecimal(Decimal::MAX_WIDTH_INT16,scale);
    Vector v1 = generateVector(type, values1);
    Vector v3(type );

    VectorOperations::negate(v1, v3, values1.size());
    Vector v4(LogicalTypeId::STRING);
    VectorOperations::tryCast(v3, v4, values1.size(), nullptr);
    for (idx_t i = 0; i < values1.size(); i++) {
        EXPECT_EQ(Decimal::toString(values1[i] * -1, scale), v4.getValue(i).toString());
    }
}


TEST_F(VectorOperationsArithTest, DecimalMultiplyVectors) {
    vector<int16_t> values1 = {1, 10, 100, 1000};
    vector<int16_t> values2 = {10, 100, 1000, 10000};
    int scale1 = 2;
    int scale2 = 3;
    int resultScale = 5;
    Vector v1 = generateVector(LogicalType::createDecimal(Decimal::MAX_WIDTH_INT32,scale1), values1);
    Vector v2 = generateVector(LogicalType::createDecimal(Decimal::MAX_WIDTH_INT32,scale2), values2);
    Vector resultVec(LogicalType::createDecimal(Decimal::MAX_WIDTH_INT64, resultScale) );

    VectorOperations::dot(v1, v2, resultVec, values1.size());
    Vector resultStringVec(LogicalTypeId::STRING);
    VectorOperations::tryCast(resultVec, resultStringVec, values1.size(), nullptr);
    for (idx_t i = 0; i < values1.size(); i++) {
        EXPECT_EQ(Decimal::toString(values1[i]*values2[i], resultScale), resultStringVec.getValue(i).toString());
    }
}


TEST_F(VectorOperationsArithTest, DecimalDivisionDifferentScales) {
    // Choose values that divide cleanly to avoid ambiguity about rounding policy (you do truncation).
    // left real:  12.34  (scale 2, raw 1234)
    // right real: 2.0    (scale 1, raw 20)
    // result scale: 3  => expected real: 6.170 => raw 6170
    vector<int32_t> left_vals  = {1234,  -1234, 500,  -500}; // scale 2
    vector<int32_t> right_vals = {20,    20,    10,   -10};  // scale 1

    int leftScale = 2;
    int rightScale = 1;
    int resultScale = 2;

    Vector v_left  = generateVector(LogicalType::createDecimal(Decimal::MAX_WIDTH_INT32, leftScale), left_vals);
    Vector v_right = generateVector(LogicalType::createDecimal(Decimal::MAX_WIDTH_INT32, rightScale), right_vals);
    Vector v_out   = Vector(LogicalType::createDecimal(Decimal::MAX_WIDTH_INT64, resultScale));

    VectorOperations::division(v_left, v_right, v_out, left_vals.size());

    Vector out_str(LogicalTypeId::STRING);
    VectorOperations::tryCast(v_out, out_str, left_vals.size(), nullptr);

    for (idx_t i = 0; i < left_vals.size(); i++) {
        // expected_raw = (L * 10^(resultScale + rightScale - leftScale)) / R
        int exp = resultScale + rightScale - leftScale;

        __int128 num = static_cast<__int128>(left_vals[i]);
        __int128 den = static_cast<__int128>(right_vals[i]);
        if (exp >= 0) num *= NumericHelper::POWERS_OF_TEN[exp];
        else          num /= NumericHelper::POWERS_OF_TEN[-exp];
        int64_t expected_raw = static_cast<int64_t>(num / den);
        std::cout << Decimal::toString(expected_raw, resultScale) << std::endl;
        EXPECT_EQ(Decimal::toString(expected_raw, resultScale), out_str.getValue(i).toString())
            << "Mismatch at index " << i;
    }
}