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
using namespace std;

class PhysicalNLTest : public ::testing::Test {
    // Creates and returns a DataChunk initialized with a predefined set of column types: INTEGER, UINTEGER, and BIGINT.
    // For each of the count rows, it populates the columns with incrementing values:
    // Column 0 (INTEGER): sequential int32_t values starting from 0.
    // Column 1 (UINTEGER): uint32_t values equal to i * 10.
    // Column 2 (BIGINT): int64_t values equal to i * 100.
    // The function sets the cardinality of the chunk to count and returns it.
    // This utility is primarily used for generating consistent and type-diverse data for testing the ChunkCollection class.
protected:
    shared_ptr<PredicateTables> ptableLeft;
    shared_ptr<PredicateTables> ptableRight;
    ClientContext client_context;
    ThreadContext context{client_context};

    void SetUp() override{
        ptableLeft = make_shared<PredicateTables>("a",3);
        ptableRight = make_shared<PredicateTables>("b",3);
    }

    std::vector<ConstantType> typesLeft{ConstantType::INTEGER, ConstantType::UINTEGER, ConstantType::BIGINT};
    std::vector<ConstantType> typesRight{ConstantType::UINTEGER, ConstantType::BIGINT, ConstantType::INTEGER};

    DataChunk createChunkWithValue( std::vector<ConstantType> testTypes, idx_t count = 1, idx_t offset=0 ) {
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

    void populatePTable(shared_ptr<PredicateTables> ptable ,std::vector<ConstantType> types, idx_t chunks = 10, idx_t elements=STANDARD_VECTOR_SIZE) {
        for (unsigned int i = 0; i < chunks; ++i) {
            DataChunk chunk = createChunkWithValue(types, elements, i*STANDARD_VECTOR_SIZE);
            ptable->append(chunk);
        }
    }
};



TEST_F(PhysicalNLTest, PhysicalNLSimpleTest) {
    populatePTable(ptableLeft, typesLeft, 1, 10);
    populatePTable(ptableRight, typesRight, 1, 20);
    std::vector<idx_t> dccols = {3,4,5};
    std::vector<idx_t> selcols = {0,1,2};
    std::vector<ConstantType> resultType = typesLeft; // Start with vec1
    resultType.insert(resultType.end(), typesRight.begin(), typesRight.end());

    vector<Expression> conditions;
    conditions.emplace_back(Expression::generateExpression(GREATER_OR_EQ, 1, 0));

    PhysicalNestedLoop pnl(resultType, dccols,selcols, 200, ptableRight.get(),conditions);
    cout << pnl.toString() << endl;

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

