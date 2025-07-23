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

#include "bumblebee/execution/atom/scan/PhysicalChunkScan.h"

using namespace bumblebee;
using namespace std;

class PhysicalScanTest : public ::testing::Test {
    // Creates and returns a DataChunk initialized with a predefined set of column types: INTEGER, UINTEGER, and BIGINT.
    // For each of the count rows, it populates the columns with incrementing values:
    // Column 0 (INTEGER): sequential int32_t values starting from 0.
    // Column 1 (UINTEGER): uint32_t values equal to i * 10.
    // Column 2 (BIGINT): int64_t values equal to i * 100.
    // The function sets the cardinality of the chunk to count and returns it.
    // This utility is primarily used for generating consistent and type-diverse data for testing the ChunkCollection class.
protected:
    shared_ptr<PredicateTables> ptable;
    ClientContext client_context;
    ThreadContext context{client_context};

    void SetUp() override{
        ptable = make_shared<PredicateTables>("a",3);
    }

    std::vector<ConstantType> testTypes{ConstantType::INTEGER, ConstantType::UINTEGER, ConstantType::BIGINT};

    DataChunk createChunkWithValue( idx_t count = 1, idx_t offset=0) {
        DataChunk chunk;
        chunk.initialize(testTypes);
        chunk.setCardinality(count);
        for (idx_t i = 0; i < count; ++i) {
            chunk.setValue(0, i, Value((int32_t) (i + offset) ));
            chunk.setValue(1, i, Value((uint32_t) ((i + offset) * 10) ));
            chunk.setValue(2, i, Value((int64_t) ((i + offset) * 100) ));
        }
        chunk.setCardinality(count);
        return chunk;
    }

    void populatePTable(idx_t chunks = 10, idx_t elements=STANDARD_VECTOR_SIZE) {
        for (unsigned int i = 0; i < chunks; ++i) {
            DataChunk chunk = createChunkWithValue(elements, i*STANDARD_VECTOR_SIZE);
            ptable->append(chunk);
        }
    }
};



TEST_F(PhysicalScanTest, PhysicalScanSimpleTest) {
    populatePTable(10, STANDARD_VECTOR_SIZE);
    PhysicalChunkScan pcs(testTypes, 0, ptable.get());
    auto state = pcs.getState();
    auto gstate = pcs.getGlobalState();

    DataChunk input;
    input.initializeEmpty(testTypes);
    auto scanRows = 0;
    auto chunksCounter = 0;
    while (pcs.getData(context, input, *state, *gstate) != AtomResultType::FINISHED) {
        scanRows+= input.getSize();
        chunksCounter++;
        // check first row value
        std::cout << input.toString() << std::endl;
    }
    EXPECT_EQ(chunksCounter, MORSEL_SIZE / STANDARD_VECTOR_SIZE);
    EXPECT_EQ(scanRows, MORSEL_SIZE);
}


TEST_F(PhysicalScanTest, PhysicalScanProjSimpleTest) {
    populatePTable(10, STANDARD_VECTOR_SIZE);
    std::vector<ConstantType> types{ConstantType::INTEGER, ConstantType::UINTEGER};
    std::vector<idx_t> cols = {0,1};
    PhysicalChunkScan pcs(types, cols, 0, ptable.get());
    auto state = pcs.getState();
    auto gstate = pcs.getGlobalState();

    DataChunk input;
    input.initializeEmpty(types);
    auto scanRows = 0;
    auto chunksCounter = 0;
    while (pcs.getData(context, input, *state, *gstate) != AtomResultType::FINISHED) {
        scanRows+= input.getSize();
        chunksCounter++;
        // check first row value
        std::cout << input.toString() << std::endl;
    }
    EXPECT_EQ(chunksCounter, MORSEL_SIZE / STANDARD_VECTOR_SIZE);
    EXPECT_EQ(scanRows, MORSEL_SIZE);
}


TEST_F(PhysicalScanTest, PhysicalScanMultiThreadTest) {
    constexpr int NUM_THREADS = 3;
    constexpr idx_t CHUNK_COUNT = 10;
    constexpr idx_t ELEMENTS_PER_CHUNK = STANDARD_VECTOR_SIZE;

    // Populate the table with a known number of chunks and rows
    populatePTable(CHUNK_COUNT, ELEMENTS_PER_CHUNK);

    // Create the scan operator and shared global state
    PhysicalChunkScan pcs(testTypes, 0, ptable.get());
    auto gstate = pcs.getGlobalState();

    // Use a vector to collect total rows processed by each thread
    std::vector<idx_t> threadRowCounts(NUM_THREADS, 0);

    // Create and launch threads
    std::vector<std::thread> threads;
    for (int t = 0; t < NUM_THREADS; ++t) {
        threads.emplace_back([t, &pcs, &gstate, &threadRowCounts, this]() {
            auto state = pcs.getState(); // Local state for each thread
            DataChunk chunk;
            chunk.initializeEmpty(testTypes);
            while (pcs.getData(context, chunk, *state, *gstate) != AtomResultType::FINISHED) {
                threadRowCounts[t] += chunk.getSize();
            }
        });
    }

    // Join all threads
    for (auto& thread : threads) {
        thread.join();
    }

    // Verify that the total number of rows matches expectation
    idx_t totalRows = std::accumulate(threadRowCounts.begin(), threadRowCounts.end(), idx_t(0));
    EXPECT_EQ(totalRows, CHUNK_COUNT * ELEMENTS_PER_CHUNK);

}
