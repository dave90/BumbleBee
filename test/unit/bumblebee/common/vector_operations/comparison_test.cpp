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
using namespace std;

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