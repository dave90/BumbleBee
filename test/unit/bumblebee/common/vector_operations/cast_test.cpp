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
#include "bumblebee/common/vector_operations/VectorOperations.hpp"
#include "bumblebee/common/Assert.hpp"
#include "bumblebee/common/NumericUtils.hpp"
#include "bumblebee/common/types/Decimal.hpp"

using namespace bumblebee;

class VectorOperationsCastTest : public BumbleBaseTest {
protected:

};

TEST_F(VectorOperationsCastTest, BasicCast) {
    vector<int> data = {0,10,20,30};
    PhysicalType sourceType = PhysicalType::INTEGER;
    PhysicalType resultType = PhysicalType::BIGINT;
    Vector input = generateVector(sourceType, data);
    EXPECT_EQ(input.getType(), sourceType);
    Vector result(resultType, data.size());

    VectorOperations::cast(input,result,data.size());
    for (idx_t i = 0; i < data.size(); i++) {
        EXPECT_EQ(result.getValue(i).cast(PhysicalType::BIGINT).getNumericValue<int64_t>(), ( (int64_t) data[i]) );
    }
}

TEST_F(VectorOperationsCastTest, UintToIntCast) {
    vector<int> data = {0,10,20,30};
    PhysicalType sourceType = PhysicalType::USMALLINT;
    PhysicalType resultType = PhysicalType::SMALLINT;
    Vector input = generateVector(sourceType, data);
    EXPECT_EQ(input.getType(), sourceType);
    Vector result(resultType, data.size());

    VectorOperations::cast(input,result,data.size());
    for (idx_t i = 0; i < data.size(); i++) {
        EXPECT_EQ(result.getValue(i).cast(PhysicalType::BIGINT).getNumericValue<int64_t>(), ( (int64_t) data[i]) );
    }
}

TEST_F(VectorOperationsCastTest, IntToFloatCast) {
    vector<int> data = {0,10,20,30};
    PhysicalType sourceType = PhysicalType::USMALLINT;
    PhysicalType resultType = PhysicalType::FLOAT;
    Vector input = generateVector(sourceType, data);
    EXPECT_EQ(input.getType(), sourceType);
    Vector result(resultType, data.size());

    VectorOperations::cast(input,result,data.size());
    for (idx_t i = 0; i < data.size(); i++) {
        EXPECT_EQ(result.getValue(i).cast(PhysicalType::DOUBLE).getNumericValue<double>(), ( (double) data[i]) );
    }
}



TEST_F(VectorOperationsCastTest, TryCastBigIntIntoSmall) {
    vector<uint64_t> data = {0,10,20,30};
    PhysicalType sourceType = PhysicalType::UBIGINT;
    PhysicalType resultType = PhysicalType::USMALLINT;
    Vector input1 = generateVector(sourceType, data);
    EXPECT_EQ(input1.getType(), sourceType);
    Vector result1(resultType, data.size());
    std::unique_ptr<string> error = std::make_unique<string>();
    bool r = VectorOperations::tryCast(input1,result1,data.size(), error.get());
    EXPECT_EQ(r, true);
    EXPECT_EQ(error->size(), 0);
    for (idx_t i = 0; i < data.size(); i++) {
        EXPECT_EQ(result1.getValue(i).cast(resultType).getNumericValue<uint16_t>(), ( (uint16_t) data[i]) );
    }

    data.push_back(65536);
    Vector input2 = generateVector(sourceType, data);
    Vector result2(resultType, data.size());
    r = VectorOperations::tryCast(input2,result2,data.size(), error.get());
    EXPECT_EQ(r, false);
    EXPECT_GE(error->size(), 0);

}



TEST_F(VectorOperationsCastTest, TryCastBigIntIntoString) {
    vector<uint64_t> data = {0,10,20,30,100000};
    PhysicalType sourceType = PhysicalType::UBIGINT;
    PhysicalType resultType = PhysicalType::STRING;
    Vector input1 = generateVector(sourceType, data);
    EXPECT_EQ(input1.getType(), sourceType);
    Vector result1(resultType, data.size());
    std::unique_ptr<string> error = std::make_unique<string>();
    bool r = VectorOperations::tryCast(input1,result1,data.size(), error.get());
    EXPECT_EQ(r, true);
    EXPECT_EQ(error->size(), 0);
    for (idx_t i = 0; i < data.size(); i++) {
        EXPECT_EQ(result1.getValue(i).toString(), std::to_string( data[i]) );
    }
}



TEST_F(VectorOperationsCastTest, TryCastStringIntoBigInt) {
    vector<string> data = {"0","10","20","30","100002340"};
    PhysicalType sourceType = PhysicalType::STRING;
    PhysicalType resultType = PhysicalType::BIGINT;
    Vector input1 = generateVector(sourceType, data);
    EXPECT_EQ(input1.getType(), sourceType);
    Vector result1(resultType, data.size());
    std::unique_ptr<string> error = std::make_unique<string>();
    bool r = VectorOperations::tryCast(input1,result1,data.size(), error.get());
    EXPECT_EQ(r, true);
    EXPECT_EQ(error->size(), 0);
    for (idx_t i = 0; i < data.size(); i++) {
        EXPECT_EQ(result1.getValue(i).cast(resultType).getNumericValue<int64_t>(), ( atoi(data[i].c_str())) );
    }

    data.push_back("12a3");
    Vector input2 = generateVector(sourceType, data);
    Vector result2(resultType, data.size());
    error = std::make_unique<string>();
    r = VectorOperations::tryCast(input2,result2,data.size(), error.get());
    EXPECT_EQ(r, false);
    EXPECT_GE(error->size(), 0);

}


TEST_F(VectorOperationsCastTest, TryCastDecimalIntoString) {
    vector<uint64_t> data = {2,11,20,30,100000,1};
    auto scale = 2;
    LogicalType sourceType = LogicalType::createDecimal(9, scale);
    PhysicalType resultType = PhysicalType::STRING;
    Vector input1 = generateVector(sourceType, data);
    EXPECT_EQ(input1.getType(), sourceType.getPhysicalType());
    Vector result1(resultType, data.size());
    std::unique_ptr<string> error = std::make_unique<string>();
    bool r = VectorOperations::tryCast(input1,result1,data.size(), error.get());
    EXPECT_EQ(r, true);
    EXPECT_EQ(error->size(), 0);
    auto expected = formatDecimalVector(scale, data);
    for (idx_t i = 0; i < expected.size(); i++) {
        EXPECT_EQ(result1.getValue(i).toString(), expected[i] );
    }
}


TEST_F(VectorOperationsCastTest, TryCastIntegerIntoDecimal) {
    vector<uint64_t> data = {2,11,20,30,100000,1};
    auto scale = 2;
    LogicalType sourceType = LogicalTypeId::INTEGER;
    LogicalType resultType = LogicalType::createDecimal(Decimal::MAX_WIDTH_INT64, scale);
    BB_ASSERT(resultType.getPhysicalType() == PhysicalType::BIGINT);
    Vector input1 = generateVector(sourceType, data);
    EXPECT_EQ(input1.getType(), sourceType.getPhysicalType());

    Vector result1(resultType, data.size());
    std::unique_ptr<string> error = std::make_unique<string>();
    bool r = VectorOperations::tryCast(input1,result1,data.size(), error.get());
    EXPECT_EQ(r, true);
    EXPECT_EQ(error->size(), 0);
    for (idx_t i = 0; i < data.size(); i++) {
        auto scaledData = result1.getValue(i).getNumericValue<int64_t>() / NumericHelper::POWERS_OF_TEN[scale];
        EXPECT_EQ(scaledData, data[i] );
    }
}


TEST_F(VectorOperationsCastTest, TryCastDoubleIntoDecimal) {
    vector<double> data = {2,11.04,20.009,30.123,100000,1.99999};
    vector<string> expectedData = {"2.00","11.04","20.01","30.12","100000.00","2.00"};
    auto scale = 2;
    LogicalType sourceType = LogicalTypeId::DOUBLE;
    LogicalType resultType = LogicalType::createDecimal(Decimal::MAX_WIDTH_INT64, scale);
    BB_ASSERT(resultType.getPhysicalType() == PhysicalType::BIGINT);
    Vector input1 = generateVector(sourceType, data);
    EXPECT_EQ(input1.getType(), sourceType.getPhysicalType());

    Vector result1(resultType, data.size());
    std::unique_ptr<string> error = std::make_unique<string>();
    bool r = VectorOperations::tryCast(input1,result1,data.size(), error.get());
    EXPECT_EQ(r, true);
    EXPECT_EQ(error->size(), 0);
    Vector result2(LogicalTypeId::STRING, data.size());
    VectorOperations::tryCast(result1,result2,data.size(), error.get());

    for (idx_t i = 0; i < data.size(); i++) {
        EXPECT_EQ(result2.getValue(i).toString(), expectedData[i] );
    }
}


TEST_F(VectorOperationsCastTest, TryCastDecimalIntoDecimal1) {
    vector<int32_t> data = {2,11,111,100000};
    vector<string> expectedData = {"0.0200","0.1100","1.1100","1000.0000"};

    auto scale = 2;
    auto resultScale = 4;
    LogicalType sourceType = LogicalType::createDecimal(Decimal::MAX_WIDTH_INT32, scale);
    LogicalType resultType = LogicalType::createDecimal(Decimal::MAX_WIDTH_INT64, resultScale);
    BB_ASSERT(resultType.getPhysicalType() == PhysicalType::BIGINT);
    Vector input1 = generateVector(sourceType, data);
    EXPECT_EQ(input1.getType(), sourceType.getPhysicalType());

    Vector result1(resultType, data.size());
    std::unique_ptr<string> error = std::make_unique<string>();
    bool r = VectorOperations::tryCast(input1,result1,data.size(), error.get());
    EXPECT_EQ(r, true);
    EXPECT_EQ(error->size(), 0);
    Vector result2(LogicalTypeId::STRING, data.size());
    VectorOperations::tryCast(result1,result2,data.size(), error.get());

    for (idx_t i = 0; i < data.size(); i++) {
        EXPECT_EQ(result2.getValue(i).toString(), expectedData[i] );
    }
}


TEST_F(VectorOperationsCastTest, TryCastDecimalIntoDecimal2) {
    vector<int32_t> data = {2,11,111,100000};
    vector<string> expectedData = {"0.00","0.00","0.01","10.00"};

    auto scale = 4;
    auto resultScale = 2;
    LogicalType sourceType = LogicalType::createDecimal(Decimal::MAX_WIDTH_INT32, scale);
    LogicalType resultType = LogicalType::createDecimal(Decimal::MAX_WIDTH_INT64, resultScale);
    BB_ASSERT(resultType.getPhysicalType() == PhysicalType::BIGINT);
    Vector input1 = generateVector(sourceType, data);
    EXPECT_EQ(input1.getType(), sourceType.getPhysicalType());

    Vector result1(resultType, data.size());
    std::unique_ptr<string> error = std::make_unique<string>();
    bool r = VectorOperations::tryCast(input1,result1,data.size(), error.get());
    EXPECT_EQ(r, true);
    EXPECT_EQ(error->size(), 0);
    Vector result2(LogicalTypeId::STRING, data.size());
    VectorOperations::tryCast(result1,result2,data.size(), error.get());

    for (idx_t i = 0; i < data.size(); i++) {
        EXPECT_EQ(result2.getValue(i).toString(), expectedData[i] );
    }
}