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


/*
 * Copyright (C) 2025 Davide Fuscà
 * GPLv3-or-later
 */

#include <numeric>
#include <thread>

#include <gtest/gtest.h>

#include "bumblebee/catalog/PredicateTables.hpp"
#include "bumblebee/ClientContext.hpp"
#include "bumblebee/parallel/ThreadContext.hpp"

#include "bumblebee/execution/atom/join/PhysicalHashJoin.hpp"

using namespace bumblebee;

class PhysicalHTJoinTest : public ::testing::Test {
protected:
    std::shared_ptr<PredicateTables> ptableLeft;
    std::shared_ptr<PredicateTables> ptableRight;
    ClientContext client_context;
    ThreadContext context{client_context};

    void SetUp() override {
        ptableLeft = std::make_shared<PredicateTables>("a", 3);
        ptableRight = std::make_shared<PredicateTables>("b", 3);
    }

    // Left:  (INTEGER, UINTEGER, BIGINT)
    // Right: (UINTEGER, BIGINT,  INTEGER)
    vector<ConstantType> typesLeft{ConstantType::INTEGER, ConstantType::UINTEGER, ConstantType::BIGINT};
    vector<ConstantType> typesRight{ConstantType::UINTEGER, ConstantType::BIGINT, ConstantType::INTEGER};

    // Helper identical to the one in your NL tests (same data pattern)
    DataChunk createChunkWithValue(vector<ConstantType> testTypes, idx_t count = 1, idx_t offset = 0) {
        DataChunk chunk;
        chunk.initialize(testTypes);
        chunk.setCardinality(count);
        for (idx_t i = 0; i < count; ++i) {
            for (idx_t j = 0; j < testTypes.size(); ++j) {
                auto value = Value((int64_t)((i + offset) * 10 * j));
                chunk.setValue(j, i, value.cast(testTypes[j]));
            }
        }
        chunk.setCardinality(count);
        return chunk;
    }

    void populatePTable(std::shared_ptr<PredicateTables> ptable, vector<ConstantType> types,
                        idx_t chunks = 10, idx_t elements = STANDARD_VECTOR_SIZE) {
        for (unsigned int i = 0; i < chunks; ++i) {
            DataChunk chunk = createChunkWithValue(types, elements, i * STANDARD_VECTOR_SIZE);
            ptable->append(chunk);
        }
    }

    // Convenience to run the typical hash-join pipeline for the right side:
    // 1) STATS (sink all right chunks)
    // 2) STATS.finalize() to init directory
    // 3) BUILD.getData() repeatedly to build buckets
    // 4) BUILD.finalize() to set ready
    void buildRightHashTable(const vector<idx_t>& rightKeyIdx) {
        // STATS operator (sink)
        // For STATS, dcCols are the columns we project from the coming input chunk (right table).
        // Here we keep all right columns: {0,1,2}
        vector<idx_t> dcColsRight{0, 1, 2};
        vector<idx_t> dummySel; // not used by STATS
        PhysicalHashJoin statsOp(typesRight, dcColsRight, dummySel,
            ptableRight.get(), rightKeyIdx, PhysicalHashType::COLLECT);

        auto gstateStats = statsOp.getGlobalState();
        auto stateStats = statsOp.getState();

        // Sink all available right chunks
        for (idx_t i = 0; i < ptableRight->chunkCount(); ++i) {
            DataChunk& r = ptableRight->getChunk(i);
            auto res = statsOp.sink(context, r, *stateStats.get(), *gstateStats.get());
            EXPECT_EQ(res, AtomResultType::NEED_MORE_INPUT);
        }
        // Finalize STATS → initDirectory
        statsOp.finalize(context, *gstateStats.get());

        // BUILD operator (source)
        PhysicalHashJoin buildOp(typesRight, dcColsRight, dummySel,
                                  ptableRight.get(), rightKeyIdx, PhysicalHashType::BUILD);
        auto gstateBuild = buildOp.getGlobalState();
        auto stateBuild = buildOp.getState();

        // Build buckets (getData may need to be called multiple times; it is safe to call extra times)
        DataChunk dummyOut;
        dummyOut.initializeEmpty(typesRight);
        for (int i = 0; i < 16; ++i) {
            auto r = buildOp.getData(context, dummyOut, *stateBuild.get(), *gstateBuild.get());
            EXPECT_EQ(r, AtomResultType::FINISHED);
        }

        // Finalize BUILD → clear stats & setReady
        buildOp.finalize(context, *gstateBuild.get());
    }
};


// End-to-end equality join: left col1 == right col0
// With the data generator, right.col0 == 0 for all rows,
// so only left row(s) where col1 == 0 (i==0) match, producing (#right_rows) results.
TEST_F(PhysicalHTJoinTest, EqualityJoin_EndToEnd) {
    // Prepare data
    populatePTable(ptableLeft,  typesLeft,  1, 10); // 10 left rows
    populatePTable(ptableRight, typesRight, 1, 20); // 20 right rows

    // Build right hash table on key = right col 0
    vector<idx_t> rightKeys{0};
    buildRightHashTable(rightKeys);

    // PROBE operator
    vector<idx_t> dccols = {3,4,5};     // where to place right-side projected cols (same pattern as NL tests)
    vector<idx_t> selcols = {0,1,2};    // select all right cols for output composition
    vector<ConstantType> resultType = typesLeft;
    resultType.insert(resultType.end(), typesRight.begin(), typesRight.end());

    vector<idx_t> lkeys{1}; // left key is left col1
    vector<Expression> conditions;
    conditions.emplace_back(Expression::generateExpression(EQUAL, 1, 0)); // l.col1 == r.col0

    PhysicalHashJoin probeOp(resultType, dccols, selcols, ptableRight.get(),
                             rightKeys, lkeys, conditions);

    auto state = probeOp.getState();

    DataChunk& input = ptableLeft->getChunk(0);
    DataChunk output;
    output.initializeEmpty(resultType);

    idx_t total = 0;
    int chunks = 0;
    while (probeOp.execute(context, input, output, *state.get()) != AtomResultType::NEED_MORE_INPUT) {
        EXPECT_GE(STANDARD_VECTOR_SIZE, output.getSize());
        total += output.getSize();
        chunks++;
    }
    EXPECT_EQ(output.getSize(), 0);    // last is empty
    EXPECT_EQ(total, 20);              // only left i==0 matches → 20 right rows
    EXPECT_GE(chunks, 1);
}


//  Empty right table → no output (but pipeline still sets HT ready)
TEST_F(PhysicalHTJoinTest, EmptyRight_NoMatches) {
    populatePTable(ptableLeft, typesLeft, 1, 10);  // Left has data
    // Right stays empty

    // Still run pipeline so HT is ready (on same right key layout)
    vector<idx_t> rightKeys{0};
    buildRightHashTable(rightKeys);

    // PROBE: l.col1 == r.col0
    vector<idx_t> dccols = {3,4,5};
    vector<idx_t> selcols = {0,1,2};
    vector<ConstantType> resultType = typesLeft;
    resultType.insert(resultType.end(), typesRight.begin(), typesRight.end());

    vector<idx_t> lkeys{1};
    vector<Expression> conditions;
    conditions.emplace_back(Expression::generateExpression(EQUAL, 1, 0));

    PhysicalHashJoin probeOp(resultType, dccols, selcols, ptableRight.get(),
                             rightKeys, lkeys, conditions);

    auto state = probeOp.getState();

    DataChunk& input = ptableLeft->getChunk(0);
    DataChunk output;
    output.initializeEmpty(resultType);

    auto res = probeOp.execute(context, input, output, *state.get());
    // Without any right rows, we should ask for more input immediately (no output)
    EXPECT_EQ(res, AtomResultType::NEED_MORE_INPUT);
    EXPECT_EQ(output.getSize(), 0);
}


// Multi-chunk right build + probe: expect (#right_rows_total) matches for the single matching left row (i==0)
TEST_F(PhysicalHTJoinTest, MultiChunkRight_BuildAndProbe) {
    populatePTable(ptableLeft,  typesLeft,  1, 10);              // 10 left rows
    populatePTable(ptableRight, typesRight, 3, STANDARD_VECTOR_SIZE); // 3 right chunks

    const idx_t rightRowsTotal = 3 * STANDARD_VECTOR_SIZE;

    vector<idx_t> rightKeys{0};
    buildRightHashTable(rightKeys);

    vector<idx_t> dccols = {3,4,5};
    vector<idx_t> selcols = {0,1,2};
    vector<ConstantType> resultType = typesLeft;
    resultType.insert(resultType.end(), typesRight.begin(), typesRight.end());

    vector<idx_t> lkeys{1};
    vector<Expression> conditions;
    conditions.emplace_back(Expression::generateExpression(EQUAL, 1, 0));

    PhysicalHashJoin probeOp(resultType, dccols, selcols, ptableRight.get(),
                             rightKeys, lkeys, conditions);

    auto state = probeOp.getState();

    DataChunk& input = ptableLeft->getChunk(0);
    DataChunk output;
    output.initializeEmpty(resultType);

    idx_t produced = 0;
    while (probeOp.execute(context, input, output, *state.get()) != AtomResultType::NEED_MORE_INPUT) {
        produced += output.getSize();
    }
    EXPECT_EQ(produced, rightRowsTotal); // only left row i==0 matches, joining with all right rows
}



// Probe with empty input chunk: MUST return NEED_MORE_INPUT and leave output empty
TEST_F(PhysicalHTJoinTest, ProbeWithEmptyInput_ReturnsNeedMoreInput) {
    // Right: build empty-but-ready hash table
    vector<idx_t> rightKeys{0};
    buildRightHashTable(rightKeys);

    // PROBE setup
    vector<idx_t> dccols = {3,4,5};
    vector<idx_t> selcols = {0,1,2};
    vector<ConstantType> resultType = typesLeft;
    resultType.insert(resultType.end(), typesRight.begin(), typesRight.end());

    vector<idx_t> lkeys{1};
    vector<Expression> conditions;
    conditions.emplace_back(Expression::generateExpression(EQUAL, 1, 0));

    PhysicalHashJoin probeOp(resultType, dccols, selcols, ptableRight.get(),
                             rightKeys, lkeys, conditions);
    auto state = probeOp.getState();

    // Empty left input
    DataChunk emptyIn;
    emptyIn.initializeEmpty(typesLeft);
    emptyIn.setCardinality(0);

    DataChunk out;
    out.initializeEmpty(resultType);

    auto res = probeOp.execute(context, emptyIn, out, *state.get());
    EXPECT_EQ(res, AtomResultType::NEED_MORE_INPUT);
    EXPECT_EQ(out.getSize(), 0);
}
