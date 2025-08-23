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

#include <numeric>
#include <thread>

#include "bumblebee/catalog/PredicateTables.h"
#include <gtest/gtest.h>

#include "bumblebee/ClientContext.h"
#include "bumblebee/execution/atom/join/PhysicalCrossProduct.h"
#include "bumblebee/execution/atom/join/PhysicalNestedLoop.h"
#include "bumblebee/parallel/ThreadContext.h"

using namespace bumblebee;

class PhysicalNLTest : public ::testing::Test {
    // Creates and returns a DataChunk initialized with a predefined set of column types: INTEGER, UINTEGER, and BIGINT.
    // For each of the count rows, it populates the columns with incrementing values:
    // Column 0 (INTEGER): sequential int32_t values starting from 0.
    // Column 1 (UINTEGER): uint32_t values equal to i * 10.
    // Column 2 (BIGINT): int64_t values equal to i * 100.
    // The function sets the cardinality of the chunk to count and returns it.
    // This utility is primarily used for generating consistent and type-diverse data for testing the ChunkCollection class.
protected:
    std::shared_ptr<PredicateTables> ptableLeft;
    std::shared_ptr<PredicateTables> ptableRight;
    ClientContext client_context;
    ThreadContext context{client_context};

    void SetUp() override{
        ptableLeft = std::make_shared<PredicateTables>("a",3);
        ptableRight = std::make_shared<PredicateTables>("b",3);
    }

    vector<ConstantType> typesLeft{ConstantType::INTEGER, ConstantType::UINTEGER, ConstantType::BIGINT};
    vector<ConstantType> typesRight{ConstantType::UINTEGER, ConstantType::BIGINT, ConstantType::INTEGER};

    DataChunk createChunkWithValue( vector<ConstantType> testTypes, idx_t count = 1, idx_t offset=0 ) {
        DataChunk chunk;
        chunk.initialize(testTypes);
        chunk.setCardinality(count);
        for (idx_t i = 0; i < count; ++i) {
            for (idx_t j = 0; j < testTypes.size(); ++j) {
                auto value = Value((int64_t) ((i+offset)*10*j));
                chunk.setValue(j, i, value.cast(testTypes[j]));
            }
        }
        chunk.setCardinality(count);
        return chunk;
    }

    void populatePTable(std::shared_ptr<PredicateTables> ptable ,vector<ConstantType> types, idx_t chunks = 10, idx_t elements=STANDARD_VECTOR_SIZE) {
        for (unsigned int i = 0; i < chunks; ++i) {
            DataChunk chunk = createChunkWithValue(types, elements, i*STANDARD_VECTOR_SIZE);
            ptable->append(chunk);
        }
    }
};



TEST_F(PhysicalNLTest, PhysicalNLSimpleTest) {
    populatePTable(ptableLeft, typesLeft, 1, 10);
    populatePTable(ptableRight, typesRight, 1, 20);
    vector<idx_t> dccols = {3,4,5};
    vector<idx_t> selcols = {0,1,2};
    vector<ConstantType> resultType = typesLeft; // Start with vec1
    resultType.insert(resultType.end(), typesRight.begin(), typesRight.end());

    vector<Expression> conditions;
    conditions.emplace_back(Expression::generateExpression(GREATER_OR_EQ, 1, 0));

    PhysicalNestedLoop pnl(resultType, dccols,selcols, 200, ptableRight.get(),conditions);
    std::cout << pnl.toString() << std::endl;

    auto state = pnl.getState();

    DataChunk& input = ptableLeft->getChunk(0);
    DataChunk output;
    output.initializeEmpty(resultType);
    auto scanRows = 0;
    auto chunksCounter = 0;
    std::cout << input.toString() << std::endl;

    while (pnl.execute(context, input, output, *state.get()) != AtomResultType::NEED_MORE_INPUT) {
        scanRows+= output.getSize();
        chunksCounter++;
        EXPECT_GE(STANDARD_VECTOR_SIZE, output.getSize());
        std::cout << output.toString() << std::endl;
    }
    EXPECT_EQ(output.getSize(), 0); // last chunk expected empty
    scanRows+= output.getSize();
    chunksCounter++;

    // expected result like a cross produce as left col 1 >= right 0
    EXPECT_EQ(chunksCounter, 2);
    EXPECT_EQ(scanRows, 20*10);
}

TEST_F(PhysicalNLTest, EmptyLeftTableTest) {
    populatePTable(ptableLeft, typesLeft, 0);  // No data
    populatePTable(ptableRight, typesRight, 1, 10);

    vector<idx_t> dccols = {3,4,5};
    vector<idx_t> selcols = {0,1,2};
    vector<ConstantType> resultType = typesLeft;
    resultType.insert(resultType.end(), typesRight.begin(), typesRight.end());

    vector<Expression> conditions;
    conditions.emplace_back(Expression::generateExpression(EQUAL, 0, 0)); // a.col0 == b.col0

    PhysicalNestedLoop pnl(resultType, dccols, selcols, 200, ptableRight.get(), conditions);
    auto state = pnl.getState();

    DataChunk input;
    input.initializeEmpty(typesLeft);
    input.setCardinality(0);

    DataChunk output;
    output.initializeEmpty(resultType);

    auto res = pnl.execute(context, input, output, *state.get());
    EXPECT_EQ(output.getSize(), 0);
    EXPECT_EQ(res, AtomResultType::NEED_MORE_INPUT);
}

TEST_F(PhysicalNLTest, NoMatchingRowsTest) {
    populatePTable(ptableLeft, typesLeft, 1, 5);    // Left: values 0..4
    populatePTable(ptableRight, typesRight, 1, 5);  // Right: values 0..4

    vector<idx_t> dccols = {3,4,5};
    vector<idx_t> selcols = {0,1,2};
    vector<ConstantType> resultType = typesLeft;
    resultType.insert(resultType.end(), typesRight.begin(), typesRight.end());

    vector<Expression> conditions;
    conditions.emplace_back(Expression::generateExpression(LESS, 0, 0)); // a.col0 < b.col0
    // Since all a.col0 == b.col0, this condition is never true

    PhysicalNestedLoop pnl(resultType, dccols, selcols, 200, ptableRight.get(), conditions);
    auto state = pnl.getState();

    DataChunk& input = ptableLeft->getChunk(0);
    DataChunk output;
    output.initializeEmpty(resultType);

    auto rows = 0;
    while (pnl.execute(context, input, output, *state.get()) != AtomResultType::NEED_MORE_INPUT) {
        rows += output.getSize();
        EXPECT_EQ(output.getSize(), 0); // Each output chunk should be empty
    }
    EXPECT_EQ(rows, 0);
}


TEST_F(PhysicalNLTest, MultiChunkJoinTest) {
    populatePTable(ptableLeft, typesLeft, 3 );
    populatePTable(ptableRight, typesRight, 2);

    vector<idx_t> dccols = {3,4,5};
    vector<idx_t> selcols = {0,1,2};
    vector<ConstantType> resultType = typesLeft;
    resultType.insert(resultType.end(), typesRight.begin(), typesRight.end());

    vector<Expression> conditions; // No predicate: Full cross product
    conditions.emplace_back(Expression::generateExpression(EQUAL, 1, 1)); // a.col1 == b.col1

    PhysicalNestedLoop pnl(resultType, dccols, selcols, 200, ptableRight.get(), conditions);

    auto state = pnl.getState();
    DataChunk output;
    output.initializeEmpty(resultType);

    int64_t totalRows = 0;
    for (idx_t i = 0; i < 3; ++i) {
        DataChunk& input = ptableLeft->getChunk(i);
        while (pnl.execute(context, input, output, *state.get()) != AtomResultType::NEED_MORE_INPUT) {
            totalRows += output.getSize();
        }
    }

    EXPECT_EQ(totalRows, STANDARD_VECTOR_SIZE * 2);
}
