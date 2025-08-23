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

#include "bumblebee/common/Constants.h"
#include "bumblebee/common/types/Vector.h"
#include "bumblebee/common/vector_operations/VectorOperations.h"

using namespace bumblebee;

class VectorOperationsComparisonTest : public ::testing::Test {
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

TEST_F(VectorOperationsComparisonTest, CompareEqualFlatVectors) {
    vector<int32_t> values = {1,2,3,4};
    Vector v1 = generateVector(INTEGER, values);
    Vector v2 = generateVector(INTEGER, values);

    auto match = VectorOperations::equals(v1,v2,nullptr,values.size(), nullptr);
    // expected all match
    EXPECT_EQ(match, values.size());


    SelectionVector sel(STANDARD_VECTOR_SIZE);
    match = VectorOperations::equals(v1,v2,nullptr,values.size(), &sel);
    EXPECT_EQ(match, values.size());
    // expected all the idx are in the selection vector
    for (idx_t i = 0; i < match; i++) {
        EXPECT_EQ(sel[i], i);
    }

}


TEST_F(VectorOperationsComparisonTest, CompareEqualFlatVectorsWithSel) {
    vector<int32_t> values = {1,2,3,4};
    Vector v1 = generateVector(INTEGER, values);
    Vector v2 = generateVector(INTEGER, values);


    SelectionVector sel(STANDARD_VECTOR_SIZE);
    SelectionVector selResult(STANDARD_VECTOR_SIZE);
    selResult.setIndex(0,3);
    selResult.setIndex(1,2);
    selResult.setIndex(2,1);
    selResult.setIndex(3,0);

    auto match = VectorOperations::equals(v1,v2,&selResult,values.size(), &sel);
    std::cout << sel.toString(10) << std::endl;
    EXPECT_EQ(match, values.size());

}

TEST_F(VectorOperationsComparisonTest, CompareNotEqualFlatVectors) {
    vector<int32_t> values = {1,2,3,4};
    Vector v1 = generateVector(INTEGER, values);
    Vector v2 = generateVector(INTEGER, values);

    auto match = VectorOperations::notEquals(v1,v2,nullptr,values.size(), nullptr);
    // expected all match
    EXPECT_EQ(match, 0);
}

TEST_F(VectorOperationsComparisonTest, CompareEqualFlatVectorsDifferentTypes) {
    vector<int32_t> values1 = {1,2,3,4};
    vector<uint16_t> values2 = {1,2,3,4};
    Vector v1 = generateVector(INTEGER, values1);
    Vector v2 = generateVector(USMALLINT, values2);

    auto match = VectorOperations::equals(v1,v2,nullptr,values1.size(), nullptr);
    // expected all match
    EXPECT_EQ(match, values1.size());
}

TEST_F(VectorOperationsComparisonTest, SignedVsUnsignedComparison) {
    vector<int32_t> signed_values = {-1, 0, 1, 4};
    vector<uint32_t> unsigned_values = {0, 0, 1, 3};
    Vector v1 = generateVector(INTEGER, signed_values);
    Vector v2 = generateVector(UINTEGER, unsigned_values);

    SelectionVector sel(STANDARD_VECTOR_SIZE);
    auto match = VectorOperations::greaterThan(v1, v2, nullptr, signed_values.size(), &sel);
    std::cout << sel.toString(match) << std::endl;
    EXPECT_EQ(match, 1); // only v1[3] > v2[3] is false, v1[2] == v2[2], v1[1] == v2[1], v1[0] < v2[0]
}

TEST_F(VectorOperationsComparisonTest, FloatVsIntegerComparison) {
    vector<float> float_values = {1.0f, 2.5f, 3.0f, 4.1f};
    vector<int32_t> int_values = {1, 3, 2, 4};
    Vector v1 = generateVector(FLOAT, float_values);
    Vector v2 = generateVector(INTEGER, int_values);

    auto match = VectorOperations::lessThanEquals(v1, v2, nullptr, float_values.size(), nullptr);
    EXPECT_EQ(match, 2); // all except last: 4.1 > 4 and 3.0 > 2
}

TEST_F(VectorOperationsComparisonTest, MixedTypesEdgeValues) {
    vector<int8_t> values1 = {0, INT8_MAX, 100, INT8_MAX};
    vector<uint16_t> values2 = {0, 0, 100, 200};
    Vector v1 = generateVector(TINYINT, values1);
    Vector v2 = generateVector(USMALLINT, values2);

    SelectionVector sel(STANDARD_VECTOR_SIZE);
    auto match = VectorOperations::greaterThan(v1, v2, nullptr, values1.size(), &sel);
    EXPECT_EQ(match, 1); // INT8_MAX > 0
    EXPECT_EQ(sel.getIndex(0), 1);
}

TEST_F(VectorOperationsComparisonTest, CompareEqualStringVectors) {
    vector<const char*> values = {"apple", "banana", "miao", "meowmeowmeowmeowmeowmeow"};
    Vector v1 = generateVector(STRING, values);
    Vector v2 = generateVector(STRING, values);

    auto match = VectorOperations::equals(v1, v2, nullptr, values.size(), nullptr);
    EXPECT_EQ(match, values.size());

    SelectionVector sel(STANDARD_VECTOR_SIZE);
    match = VectorOperations::equals(v1, v2, nullptr, values.size(), &sel);
    EXPECT_EQ(match, values.size());
    for (idx_t i = 0; i < match; i++) {
        EXPECT_EQ(sel[i], i);
    }
}

TEST_F(VectorOperationsComparisonTest, CompareLessThanStringVectors) {
    vector<const char*> v1_values = {"ant", "bat", "miao", "dog"};
    vector<const char*> v2_values = {"apple", "banana", "meow", "donkey"};

    Vector v1 = generateVector(STRING, v1_values);
    Vector v2 = generateVector(STRING, v2_values);

    SelectionVector sel(STANDARD_VECTOR_SIZE);
    auto match = VectorOperations::lessThan(v1, v2, nullptr, v1_values.size(), &sel);
    std::cout << sel.toString(match) << std::endl;
    EXPECT_EQ(match, 2); // "ant" < "apple", "bat" > "banana", "miao" > "meow", "dog" > "donkey"
}

