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

#include <memory>
#include <gtest/gtest.h>

#include "bumblebee/catalog/PredicateTables.hpp"
#include "bumblebee/common/Constants.hpp"
#include "bumblebee/common/vector_operations/VectorOperations.hpp"
#include "bumblebee/execution/atom/expression/PhysicalExpression.hpp"
using namespace bumblebee;


class PhysicalExpressionTest : public ::testing::Test {
    // Creates and returns a DataChunk initialized with a predefined set of column types: INTEGER, UINTEGER, and BIGINT.
    // For each of the count rows, it populates the columns with incrementing values:
    // Column 0 (INTEGER): sequential int32_t values starting from 0.
    // Column 1 (UINTEGER): uint32_t values equal to i * 10.
    // Column 2 (BIGINT): int64_t values equal to i * 100.
    // The function sets the cardinality of the chunk to count and returns it.
    // This utility is primarily used for generating consistent and type-diverse data for testing the ChunkCollection class.
protected:
    std::shared_ptr<PredicateTables> ptable;
    ClientContext client_context;
    ThreadContext context{client_context};


    void SetUp() override{
        ptable = std::make_shared<PredicateTables>(&client_context, "a",5);
    }

    vector<LogicalType> testTypes{LogicalTypeId::BIGINT, LogicalTypeId::UINTEGER, LogicalTypeId::BIGINT, LogicalTypeId::USMALLINT,LogicalTypeId::SMALLINT  };

    DataChunk createChunkWithValue( idx_t count = 1, idx_t offset=0) {
        DataChunk chunk;
        chunk.initialize(testTypes);

        for (idx_t i = 0; i < count; ++i) {
            chunk.setValue(0, i, Value((int64_t) (i + offset) ));
            chunk.setValue(1, i, Value((uint32_t) ((i + offset) * 10) ));
            chunk.setValue(2, i, Value((int64_t) ((i + offset) * 20) ));
            chunk.setValue(3, i, Value((uint16_t) ((i + offset) * 30) ));
            chunk.setValue(4, i, Value((int16_t) ((i + offset) * 40) ));
        }
        chunk.setCardinality(count);
        return chunk;
    }
};

Expression generateExpression(Binop op, vector<idx_t> leftCols, vector<Operator> leftOps,vector<idx_t> rightCols, vector<Operator> rightOps) {
    return Expression(op, leftCols, leftOps, rightCols, rightOps);
}

TEST_F(PhysicalExpressionTest, PhysicalExpressionEQTest) {

    idx_t count = 100;
    Expression expr = generateExpression(EQUAL, {0}, {}, {1}, {} );

    PhysicalExpression pe(expr, testTypes);
    auto gstate = pe.getGlobalState();
    auto state = pe.getState();

    auto input = createChunkWithValue(count, 0);
    DataChunk output;
    output.initializeEmpty(testTypes);

    pe.execute(context, input, output, *state);
    std::cout << output.toString() <<std::endl;
    EXPECT_NE(output.getSize(), input.getSize());
    EXPECT_EQ(output.getSize(), 1); // only first row all 0 is equal
}


TEST_F(PhysicalExpressionTest, PhysicalExpressionAssignmentTest) {
    // assign the second column to first column
    idx_t count = 100;
    Expression expr = generateExpression(ASSIGNMENT, {0}, {}, {2}, {} );

    PhysicalExpression pe(expr, testTypes);
    auto gstate = pe.getGlobalState();
    auto state = pe.getState();

    auto input = createChunkWithValue(count, 0);
    DataChunk output;
    output.initializeEmpty(testTypes);

    pe.execute(context, input, output, *state);
    std::cout << output.toString() <<std::endl;
    EXPECT_EQ(output.getSize(), input.getSize());
    auto result = VectorOperations::equals(output.data_[0],output.data_[2], nullptr, count, nullptr );
    EXPECT_EQ(result, count);
    result = VectorOperations::equals(output.data_[0],input.data_[2], nullptr, count, nullptr );
    EXPECT_EQ(result, count);
}


TEST_F(PhysicalExpressionTest, PhysicalExpressionAssignmentMultiOpTest) {
    // assign the second column to first column
    idx_t count = 100;
    Expression expr = generateExpression(ASSIGNMENT, {0}, {}, {1,2,3,4,2}, {Operator::MINUS, Operator::PLUS, Operator::TIMES, Operator::DIV} );
    // Expression expr = generateExpression(ASSIGNMENT, {0}, {}, {1,2,3}, {PLUS, PLUS} );

    PhysicalExpression pe(expr, testTypes);
    auto gstate = pe.getGlobalState();
    auto state = pe.getState();

    auto input = createChunkWithValue(count, 0);
    std::cout << input.toString() <<std::endl;

    DataChunk output;
    output.initializeEmpty(testTypes);

    pe.execute(context, input, output, *state);
    std::cout << output.toString() <<std::endl;
    EXPECT_EQ(output.getSize(), input.getSize());
    for (idx_t i = 0; i < count; ++i) {
        auto v1 = input.getValue(1, i).getNumericValue<uint32_t>();
        auto v2 = input.getValue(2, i).getNumericValue<int64_t>();
        auto v3 = input.getValue(3, i).getNumericValue<uint16_t>();
        auto v4 = input.getValue(4, i).getNumericValue<int16_t>();
        auto result = v1 - v2 + v3  * v4  / v2;

        EXPECT_EQ(output.getValue(0, i).getNumericValue<int64_t>(), result);
    }
}

TEST_F(PhysicalExpressionTest, PhysicalExpressionAssignmentMultiOpPrecedence1) {
    // v1 + v2 * v3 - v4  (should be v1 + (v2 * v3) - v4)
    idx_t count = 100;
    Expression expr = generateExpression(ASSIGNMENT, {0}, {}, {1, 2, 3, 4}, {Operator::PLUS, Operator::TIMES, Operator::MINUS});

    PhysicalExpression pe(expr, testTypes);
    auto gstate = pe.getGlobalState();
    auto state = pe.getState();

    // use offset 1 to avoid all-zero row; not strictly needed here but consistent
    auto input = createChunkWithValue(count, 1);
    DataChunk output;
    output.initializeEmpty(testTypes);

    pe.execute(context, input, output, *state);
    EXPECT_EQ(output.getSize(), input.getSize());

    for (idx_t i = 0; i < count; ++i) {
        auto v1 = input.getValue(1, i).getNumericValue<uint32_t>();
        auto v2 = input.getValue(2, i).getNumericValue<int64_t>();
        auto v3 = input.getValue(3, i).getNumericValue<uint16_t>();
        auto v4 = input.getValue(4, i).getNumericValue<int16_t>();

        auto result = v1 + v2 * v3 - v4; // precedence: v2 * v3 first
        EXPECT_EQ(output.getValue(0, i).getNumericValue<int64_t>(), result);
    }
}

TEST_F(PhysicalExpressionTest, PhysicalExpressionAssignmentMultiOpPrecedence2) {
    // v1 * v2 + v3 / v4 - v2  (should be (v1 * v2) + (v3 / v4) - v2)
    idx_t count = 100;
    Expression expr = generateExpression(ASSIGNMENT, {0}, {}, {1, 2, 3, 4, 2},
                                         {Operator::TIMES, Operator::PLUS, Operator::DIV, Operator::MINUS});

    PhysicalExpression pe(expr, testTypes);
    auto gstate = pe.getGlobalState();
    auto state = pe.getState();

    // offset 1 to avoid division by zero on v4
    auto input = createChunkWithValue(count, 1);
    DataChunk output;
    output.initializeEmpty(testTypes);

    pe.execute(context, input, output, *state);
    EXPECT_EQ(output.getSize(), input.getSize());

    for (idx_t i = 0; i < count; ++i) {
        auto v1 = input.getValue(1, i).getNumericValue<uint32_t>();
        auto v2 = input.getValue(2, i).getNumericValue<int64_t>();
        auto v3 = input.getValue(3, i).getNumericValue<uint16_t>();
        auto v4 = input.getValue(4, i).getNumericValue<int16_t>();

        auto result = v1 * v2 + v3 / v4 - v2; // * and / before + and -
        EXPECT_EQ(output.getValue(0, i).getNumericValue<int64_t>(), result);
    }
}

TEST_F(PhysicalExpressionTest, PhysicalExpressionAssignmentMultiOpPrecedence3) {
    // v1 - v2 * v3 + v1 / v4  (should be v1 - (v2 * v3) + (v1 / v4))
    idx_t count = 100;
    Expression expr = generateExpression(ASSIGNMENT, {0}, {}, {1, 2, 3, 1, 4},
                                         {Operator::MINUS, Operator::TIMES, Operator::PLUS, Operator::DIV});

    PhysicalExpression pe(expr, testTypes);
    auto gstate = pe.getGlobalState();
    auto state = pe.getState();

    // offset 1 to avoid division by zero on v4 and v1
    auto input = createChunkWithValue(count, 1);
    DataChunk output;
    output.initializeEmpty(testTypes);

    pe.execute(context, input, output, *state);
    EXPECT_EQ(output.getSize(), input.getSize());

    for (idx_t i = 0; i < count; ++i) {
        auto v1 = input.getValue(1, i).getNumericValue<uint32_t>();
        auto v2 = input.getValue(2, i).getNumericValue<int64_t>();
        auto v3 = input.getValue(3, i).getNumericValue<uint16_t>();
        auto v4 = input.getValue(4, i).getNumericValue<int16_t>();

        auto result = v1 - v2 * v3 + v1 / v4; // v2 * v3, v1 / v4 first
        EXPECT_EQ(output.getValue(0, i).getNumericValue<int64_t>(), result);
    }
}
