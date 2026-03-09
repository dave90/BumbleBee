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

#include "bumblebee/catalog/PredicateTables.hpp"
#include <gtest/gtest.h>

#include "bumblebee/ClientContext.hpp"
#include "bumblebee/execution/atom/join/PhysicalCrossProduct.hpp"
#include "bumblebee/parallel/ThreadContext.hpp"

using namespace bumblebee;

class PhysicalCrossJoinTest : public ::testing::Test {
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
        ptableLeft = std::make_shared<PredicateTables>(&client_context,"a",3);
        ptableRight = std::make_shared<PredicateTables>(&client_context, "b",3);
    }

    vector<LogicalType> testTypesLeft{LogicalTypeId::INTEGER, LogicalTypeId::UINTEGER, LogicalTypeId::BIGINT};
    vector<LogicalType> testTypesRight{LogicalTypeId::UINTEGER, LogicalTypeId::BIGINT, LogicalTypeId::INTEGER};

    DataChunk createChunkWithValue( vector<LogicalType> testTypes, idx_t count = 1, idx_t offset=0 ) {
        DataChunk chunk;
        chunk.initialize(testTypes);
        chunk.setCardinality(count);
        for (idx_t i = 0; i < count; ++i) {
            for (idx_t j = 0; j < testTypes.size(); ++j) {
                auto value = Value((int64_t) ((i+offset)*10*j));
                chunk.setValue(j, i, value.cast(testTypes[j].getPhysicalType()));
            }
        }
        chunk.setCardinality(count);
        return chunk;
    }

    void populatePTable(std::shared_ptr<PredicateTables> ptable ,vector<LogicalType> types, idx_t chunks = 10, idx_t elements=STANDARD_VECTOR_SIZE) {
        for (unsigned int i = 0; i < chunks; ++i) {
            DataChunk chunk = createChunkWithValue(types, elements, i*STANDARD_VECTOR_SIZE);
            ptable->append(chunk);
        }
    }
};



TEST_F(PhysicalCrossJoinTest, PhysicalCrossSimpleTest) {
    populatePTable(ptableLeft, testTypesLeft, 1, 10);
    populatePTable(ptableRight, testTypesRight, 1, 20);
    vector<idx_t> dccols = {3,4,5};
    vector<idx_t> selcols = {0,1,2};
    vector<LogicalType> resultType = testTypesLeft; // Start with vec1
    resultType.insert(resultType.end(), testTypesRight.begin(), testTypesRight.end());

    PhysicalCrossProduct pcj(resultType, dccols,selcols, ptableRight.get());
    auto state = pcj.getState();

    DataChunk& input = ptableLeft->getChunk(0);
    DataChunk output;
    output.initializeEmpty(resultType);
    auto scanRows = 0;
    auto chunksCounter = 0;
    while (pcj.execute(context, input, output, *state.get()) != AtomResultType::NEED_MORE_INPUT) {
        scanRows+= output.getSize();
        chunksCounter++;
        // check first row value
        std::cout << output.toString() << std::endl;
    }
    scanRows+= output.getSize();
    chunksCounter++;

    EXPECT_EQ(chunksCounter, 21); // number of chunks is based on size of the right side
    EXPECT_EQ(scanRows, 20*10);
}


TEST_F(PhysicalCrossJoinTest, PhysicalCrossPrjTest) {

    populatePTable(ptableLeft, testTypesLeft, 1, 10);
    populatePTable(ptableRight, testTypesRight, 1, 20);
    // select only column 0 and 2
    vector<idx_t> dccols = {3,4};
    vector<idx_t> selcols = {0,2};
    vector<LogicalType> resultType = testTypesLeft;
    for (auto c : selcols)
        resultType.push_back(testTypesRight[c]);

    PhysicalCrossProduct pcj(resultType, dccols,selcols, ptableRight.get());
    auto state = pcj.getState();

    DataChunk& input = ptableLeft->getChunk(0);

    DataChunk output;
    output.initializeEmpty(resultType);
    auto scanRows = 0;
    auto chunksCounter = 0;
    while (pcj.execute(context, input, output, *state.get()) != AtomResultType::NEED_MORE_INPUT) {
        scanRows+= input.getSize();
        chunksCounter++;
        // check first row value
        std::cout << output.toString() << std::endl;
    }
    scanRows+= output.getSize();
    chunksCounter++;

    EXPECT_EQ(chunksCounter, 21);
    EXPECT_EQ(scanRows, 20*10);
}



TEST_F(PhysicalCrossJoinTest, PhysicalCrossNoInputTest) {
    vector<idx_t> cols = {0,1};
    auto dcCols = cols;
    PhysicalCrossProduct pcj(testTypesRight, dcCols,cols, ptableRight.get());
    auto state = pcj.getState();

    DataChunk input;
    input.initializeEmpty(testTypesLeft);
    vector<LogicalType> resultType = testTypesLeft; // Start with vec1
    resultType.insert(resultType.end(), testTypesRight.begin(), testTypesRight.end());
    DataChunk output;
    output.initializeEmpty(resultType);
    auto result = pcj.execute(context, input, output, *state.get());
    EXPECT_EQ(result, AtomResultType::NEED_MORE_INPUT);
    EXPECT_EQ(output.getSize(), 0);
}
