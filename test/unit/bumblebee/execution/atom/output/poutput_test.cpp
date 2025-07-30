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


#include <thread>
#include <gtest/gtest.h>

#include "bumblebee/catalog/PredicateTables.h"
#include "bumblebee/execution/atom/output/PhysicalChunkOutput.h"
#include "bumblebee/output/OutputBuilder.h"

using namespace bumblebee;
using namespace std;

class PhysicalOutputTest : public ::testing::Test {
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
    std::vector<ConstantType> extraTestTypes{ConstantType::INTEGER, ConstantType::UINTEGER, ConstantType::BIGINT, ConstantType::BIGINT};

    DataChunk createChunkWithValue( idx_t count = 1, idx_t offset=0, bool extraCol = true) {
        DataChunk chunk;
        if (!extraCol)
            chunk.initialize(testTypes);
        else
            chunk.initialize(extraTestTypes);

        for (idx_t i = 0; i < count; ++i) {
            chunk.setValue(0, i, Value((int32_t) (i + offset) ));
            chunk.setValue(1, i, Value((uint32_t) ((i + offset) * 10) ));
            chunk.setValue(2, i, Value((int64_t) ((i + offset) * 100) ));
        }
        chunk.setCardinality(count);
        return chunk;
    }

};


TEST_F(PhysicalOutputTest, PhysicalChunkOutputSingleThreadedTest) {

    std::vector<idx_t> cols = {0,1,2};
    PhysicalChunkOutput pco(extraTestTypes,cols, 0, ptable.get());
    auto gstate = pco.getGlobalState();
    auto state = pco.getState();

    DataChunk chunk = createChunkWithValue(STANDARD_VECTOR_SIZE, 0, true);
    auto result = pco.sink(context, chunk, *state, *gstate);

    EXPECT_EQ(result, AtomResultType::NEED_MORE_INPUT);

    pco.finalize(context, *gstate);

    ASSERT_EQ(ptable->chunkCount(), 1);
    EXPECT_EQ(ptable->getChunk(0).getSize(), STANDARD_VECTOR_SIZE);
}


TEST_F(PhysicalOutputTest, PhysicalChunkOutputMultiThreadedTest) {
    constexpr int NUM_THREADS = 2;
    constexpr int TOTAL_CHUNKS = 8;
    std::vector<idx_t> cols = {0,1,2};
    PhysicalChunkOutput pco(testTypes, cols,0, ptable.get());
    auto gstate = pco.getGlobalState();

    std::vector<std::thread> threads;

    for (int t = 0; t < NUM_THREADS; ++t) {
        threads.emplace_back([&pco, &gstate, this]() {
            auto state = pco.getState();
            for (int i = 0; i < TOTAL_CHUNKS / NUM_THREADS; ++i) {
                DataChunk chunk = createChunkWithValue(STANDARD_VECTOR_SIZE, 0 , false);
                pco.sink(context, chunk, *state, *gstate);
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    pco.finalize(context, *gstate);

    EXPECT_EQ(ptable->chunkCount(), TOTAL_CHUNKS);
    for (idx_t i = 0; i < ptable->chunkCount(); ++i) {
        EXPECT_EQ(ptable->getChunk(i).getSize(), STANDARD_VECTOR_SIZE);
    }
}


TEST_F(PhysicalOutputTest, PhysicalChunkOutputFlushPartialChunksTest) {
    constexpr int NUM_THREADS = 2;
    constexpr int ROWS_PER_THREAD = 3 * (STANDARD_VECTOR_SIZE / 2); // ensure partial flushes
    std::vector<idx_t> cols = {0,1,2};
    PhysicalChunkOutput pco(testTypes, cols,0, ptable.get());
    auto gstate = pco.getGlobalState();

    std::vector<std::thread> threads;

    for (int t = 0; t < NUM_THREADS; ++t) {
        threads.emplace_back([&pco, &gstate, this, t]() {
            auto state = pco.getState();
            AtomResultType result;
            for (int i = 0; i < ROWS_PER_THREAD; i += STANDARD_VECTOR_SIZE / 2) {
                DataChunk chunk = createChunkWithValue(STANDARD_VECTOR_SIZE / 2, 0 , false);
                result = pco.sink(context, chunk, *state, *gstate);
            }
            // final chunk is not full so expected have more output
            EXPECT_EQ(result, AtomResultType::HAVE_MORE_OUTPUT);
            // Call passing empty chunk
            DataChunk chunk = createChunkWithValue(0, 0, false);
            result = pco.sink(context, chunk, *state, *gstate);
            EXPECT_EQ(result, AtomResultType::NEED_MORE_INPUT);
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    pco.finalize(context, *gstate);

    // Check total rows (each thread = 3 half-sized chunks)
    idx_t expectedRows = NUM_THREADS * ROWS_PER_THREAD;
    idx_t actualRows = 0;
    for (idx_t i = 0; i < ptable->chunkCount(); ++i) {
        actualRows += ptable->getChunk(i).getSize();
    }

    EXPECT_EQ(actualRows, expectedRows);
}
