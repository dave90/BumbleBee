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
#include "bumblebee/common/vector_operations/VectorOperations.h"
#include "bumblebee/common/Assert.h"

using namespace bumblebee;

class VectorOperationsCastTest : public ::testing::Test {
protected:

    template <class T>
    void setValuesVector(Vector &v, vector<T> values ) {
        for (idx_t i = 0; i < values.size(); i++) {
            Value val(values[i]);
            v.setValue(i, val.cast(v.getType()));
        }
    }

    template <class T>
    Vector generateVector(ConstantType type, vector<T> values ) {
        Vector v1(type,values.size());
        setValuesVector<T>(v1,values);
        return v1;
    }

};

TEST_F(VectorOperationsCastTest, BasicCast) {
    vector<int> data = {0,10,20,30};
    ConstantType sourceType = INTEGER;
    ConstantType resultType = BIGINT;
    Vector input = generateVector(sourceType, data);
    EXPECT_EQ(input.getType(), sourceType);
    Vector result(resultType, data.size());

    VectorOperations::cast(input,result,data.size());
    for (idx_t i = 0; i < data.size(); i++) {
        EXPECT_EQ(result.getValue(i).cast(BIGINT).getNumericValue<int64_t>(), ( (int64_t) data[i]) );
    }
}

TEST_F(VectorOperationsCastTest, UintToIntCast) {
    vector<int> data = {0,10,20,30};
    ConstantType sourceType = USMALLINT;
    ConstantType resultType = SMALLINT;
    Vector input = generateVector(sourceType, data);
    EXPECT_EQ(input.getType(), sourceType);
    Vector result(resultType, data.size());

    VectorOperations::cast(input,result,data.size());
    for (idx_t i = 0; i < data.size(); i++) {
        EXPECT_EQ(result.getValue(i).cast(BIGINT).getNumericValue<int64_t>(), ( (int64_t) data[i]) );
    }
}

TEST_F(VectorOperationsCastTest, IntToFloatCast) {
    vector<int> data = {0,10,20,30};
    ConstantType sourceType = USMALLINT;
    ConstantType resultType = FLOAT;
    Vector input = generateVector(sourceType, data);
    EXPECT_EQ(input.getType(), sourceType);
    Vector result(resultType, data.size());

    VectorOperations::cast(input,result,data.size());
    for (idx_t i = 0; i < data.size(); i++) {
        EXPECT_EQ(result.getValue(i).cast(DOUBLE).getNumericValue<double>(), ( (double) data[i]) );
    }
}
