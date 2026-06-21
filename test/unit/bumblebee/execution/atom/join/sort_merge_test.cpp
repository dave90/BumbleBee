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

#include <set>

#include <gtest/gtest.h>

#include "../../../BumbleBaseTest.hpp"
#include "bumblebee/catalog/PredicateTables.hpp"
#include "bumblebee/execution/atom/join/PhysicalPieceWiseSortMergeJoin.hpp"
#include "bumblebee/parallel/ThreadContext.hpp"

using namespace bumblebee;

// One output row is a vector<Value>; rows are compared as multisets (order-independent).
using Row = vector<Value>;
using RowMultiset = std::multiset<Row>;

class SortMergeTest : public BumbleBaseTest {
protected:
    ThreadContext context{clientContext};

    // ---- generic chunk generation (reuses BumbleBaseTest helpers) ----

    // Column-major explicit chunk: cols[c] holds the values of column c.
    template <class T>
    DataChunk makeChunk(const vector<LogicalType>& types, const vector<vector<T>>& cols) {
        vector<vector<Value>> data;
        for (auto& c : cols) addData(data, c);
        auto t = types;
        return generateDataChunk(t, data);
    }

    DataChunk emptyChunk(const vector<LogicalType>& types) {
        DataChunk c;
        c.initializeEmpty(types);
        c.setCardinality(0);
        return c;
    }

    // Random chunk over a small integer domain so that inequality joins actually match.
    DataChunk randomIntChunk(const vector<LogicalType>& types, idx_t nrows, int domain) {
        vector<vector<Value>> data(types.size());
        std::uniform_int_distribution<int> dist(-domain, domain);
        for (idx_t c = 0; c < types.size(); ++c)
            for (idx_t r = 0; r < nrows; ++r)
                data[c].push_back(Value((int64_t)dist(rng)));
        auto t = types;
        return generateDataChunk(t, data);
    }

    // ---- generic reference + comparison ----

    static bool applyOp(const Value& l, const Value& r, Binop op) {
        switch (op) {
            case GREATER:       return l > r;
            case GREATER_OR_EQ: return l >= r;
            case LESS:          return l < r;
            case LESS_OR_EQ:    return l <= r;
            default:            return false;
        }
    }

    static Row extractRow(DataChunk& c, idx_t r, idx_t ncols) {
        Row row;
        for (idx_t col = 0; col < ncols; ++col) row.push_back(c.getValue(col, r));
        return row;
    }

    // O(N*M) reference. Output row layout = [all probe cols] ++ [build cols in buildSel].
    RowMultiset bruteForceJoin(DataChunk& probe, const vector<DataChunk*>& builds,
                               idx_t probeKeyCol, idx_t buildKeyCol,
                               const vector<idx_t>& buildSel, Binop op) {
        RowMultiset out;
        idx_t probeArity = probe.columnCount();
        for (idx_t pr = 0; pr < probe.getSize(); ++pr) {
            Value pk = probe.getValue(probeKeyCol, pr);
            for (auto* b : builds)
                for (idx_t br = 0; br < b->getSize(); ++br) {
                    if (!applyOp(pk, b->getValue(buildKeyCol, br), op)) continue;
                    Row row = extractRow(probe, pr, probeArity);
                    for (auto col : buildSel) row.push_back(b->getValue(col, br));
                    out.insert(std::move(row));
                }
        }
        return out;
    }

    // Drive the operator: build (one thread-local state per group, then combine) + probe.
    RowMultiset runJoin(const vector<LogicalType>& buildTypes,
                        const vector<vector<DataChunk*>>& buildThreads,
                        DataChunk& probe, idx_t probeKeyCol, idx_t buildKeyCol,
                        const vector<idx_t>& buildSel, Binop op) {
        auto pt = std::make_shared<PredicateTables>(&clientContext, "b", buildTypes.size());
        pt->setTypes(buildTypes);

        idx_t probeArity = probe.columnCount();
        vector<LogicalType> resultType = probe.getTypes();
        for (auto col : buildSel) resultType.push_back(buildTypes[col]);
        vector<idx_t> dcCols, payloads = buildSel;
        for (idx_t i = 0; i < buildSel.size(); ++i) dcCols.push_back(probeArity + i);

        // --- build ---
        {
            auto dc = dcCols, sel = buildSel;
            PhysicalPieceWiseSortMergeJoin build(clientContext, resultType, dc, sel, pt.get(),
                                                 buildKeyCol, payloads, COLLECT);
            auto gstate = build.getGlobalState();
            for (auto& thread : buildThreads) {
                auto lstate = build.getState();
                for (auto* c : thread) build.sink(context, *c, *lstate, *gstate);
                build.combine(context, *lstate, *gstate);
            }
            build.finalize(context, *gstate);
        }

        // --- probe ---
        auto dc = dcCols, sel = buildSel;
        PhysicalPieceWiseSortMergeJoin probeOp(clientContext, resultType, dc, sel, pt.get(),
                                               buildKeyCol, probeKeyCol, op, payloads);
        auto pstate = probeOp.getState();
        DataChunk output;
        output.initializeEmpty(resultType);

        RowMultiset result;
        while (probeOp.execute(context, probe, output, *pstate) != AtomResultType::NEED_MORE_INPUT) {
            EXPECT_GE((idx_t)STANDARD_VECTOR_SIZE, output.getSize());
            for (idx_t r = 0; r < output.getSize(); ++r)
                result.insert(extractRow(output, r, resultType.size()));
        }
        return result;
    }

    // Convenience: single build key/probe key, build cols all selected, compare for one op.
    void expectJoin(const vector<LogicalType>& buildTypes, const vector<vector<DataChunk*>>& buildThreads,
                    DataChunk& probe, idx_t probeKeyCol, idx_t buildKeyCol,
                    const vector<idx_t>& buildSel, Binop op) {
        vector<DataChunk*> flat;
        for (auto& t : buildThreads) for (auto* c : t) flat.push_back(c);
        auto expected = bruteForceJoin(probe, flat, probeKeyCol, buildKeyCol, buildSel, op);
        auto actual = runJoin(buildTypes, buildThreads, probe, probeKeyCol, buildKeyCol, buildSel, op);
        EXPECT_EQ(actual.size(), expected.size());
        EXPECT_TRUE(actual == expected);
    }

    static constexpr Binop kOps[] = {GREATER, GREATER_OR_EQ, LESS, LESS_OR_EQ};
};

constexpr Binop SortMergeTest::kOps[];

// Single INTEGER key column, single build chunk, all four operators.
TEST_F(SortMergeTest, AllOperatorsSingleChunk) {
    vector<LogicalType> t{LogicalTypeId::INTEGER};
    DataChunk build = makeChunk<int>(t, {{5, 1, 9, 3, 7, 2, 8}});
    DataChunk probe = makeChunk<int>(t, {{4, 6, 0, 9, 5}});
    for (Binop op : kOps) expectJoin(t, {{&build}}, probe, 0, 0, {0}, op);
}

// Many equal keys exercise lower_bound/upper_bound boundaries.
TEST_F(SortMergeTest, DuplicateKeys) {
    vector<LogicalType> t{LogicalTypeId::INTEGER};
    DataChunk build = makeChunk<int>(t, {{2, 2, 2, 5, 5, 1, 2, 5}});
    DataChunk probe = makeChunk<int>(t, {{2, 5, 3, 1}});
    for (Binop op : kOps) expectJoin(t, {{&build}}, probe, 0, 0, {0}, op);
}

// Multi-column build: key is column 1, project columns {0,1,2}.
TEST_F(SortMergeTest, MultiColumnPayload) {
    vector<LogicalType> bt{LogicalTypeId::INTEGER, LogicalTypeId::INTEGER, LogicalTypeId::BIGINT};
    DataChunk build = makeChunk<int>(bt, {{10, 20, 30, 40}, {3, 1, 4, 2}, {100, 200, 300, 400}});
    vector<LogicalType> pt{LogicalTypeId::INTEGER, LogicalTypeId::INTEGER};
    DataChunk probe = makeChunk<int>(pt, {{7, 8}, {2, 5}});
    for (Binop op : kOps) expectJoin(bt, {{&build}}, probe, /*probeKey*/1, /*buildKey*/1, {0, 1, 2}, op);
}

// Payload is a reordered subset of build columns, NOT including the key column.
TEST_F(SortMergeTest, MultiColumnPayloadReorderedSubset) {
    vector<LogicalType> bt{LogicalTypeId::INTEGER, LogicalTypeId::BIGINT,
                           LogicalTypeId::INTEGER, LogicalTypeId::UINTEGER};
    DataChunk build = makeChunk<int>(bt, {{10, 20, 30, 40, 50},   // col0
                                          {1, 2, 3, 4, 5},         // col1
                                          {7, 3, 9, 1, 5},         // col2 = KEY
                                          {100, 200, 300, 400, 500}}); // col3
    vector<LogicalType> pt{LogicalTypeId::INTEGER, LogicalTypeId::INTEGER};
    DataChunk probe = makeChunk<int>(pt, {{6, 8, 2}, {0, 1, 2}});
    // key = build col2 vs probe col0; project build cols {3, 0, 1} (reordered, key excluded)
    for (Binop op : kOps) expectJoin(bt, {{&build}}, probe, /*probeKey*/0, /*buildKey*/2, {3, 0, 1}, op);
}

// Multi-column payload spanning multiple pieces (> STANDARD_VECTOR_SIZE build rows).
TEST_F(SortMergeTest, MultiColumnPayloadMultiPiece) {
    vector<LogicalType> bt{LogicalTypeId::INTEGER, LogicalTypeId::BIGINT, LogicalTypeId::INTEGER};
    DataChunk b0 = randomIntChunk(bt, STANDARD_VECTOR_SIZE, 25);
    DataChunk b1 = randomIntChunk(bt, 300, 25);
    vector<LogicalType> ptp{LogicalTypeId::INTEGER, LogicalTypeId::INTEGER};
    DataChunk probe = randomIntChunk(ptp, 128, 25);
    // key = build col0 vs probe col0; project all build cols {0,1,2}
    for (Binop op : kOps) expectJoin(bt, {{&b0, &b1}}, probe, 0, 0, {0, 1, 2}, op);
}

// addChunk path D: a second chunk fills `open` to EXACTLY STANDARD_VECTOR_SIZE (seal, no leftover).
// Two chunks to one thread-local index; sizes sum to exactly STANDARD_VECTOR_SIZE.
TEST_F(SortMergeTest, AccumulateExactFill) {
    vector<LogicalType> t{LogicalTypeId::INTEGER};
    DataChunk b0 = randomIntChunk(t, STANDARD_VECTOR_SIZE / 3, 20);
    DataChunk b1 = randomIntChunk(t, STANDARD_VECTOR_SIZE - STANDARD_VECTOR_SIZE / 3, 20);
    DataChunk probe = randomIntChunk(t, 128, 20);
    for (Binop op : kOps) expectJoin(t, {{&b0, &b1}}, probe, 0, 0, {0}, op);
}

// addChunk path E: a second chunk OVERFLOWS open's free space -> fill+seal, then the leftover rows
// are copied into a fresh open (the `realSize > full.getSize()` remaining block).
TEST_F(SortMergeTest, AccumulateOverflow) {
    vector<LogicalType> t{LogicalTypeId::INTEGER};
    DataChunk b0 = randomIntChunk(t, STANDARD_VECTOR_SIZE - 50, 20); // open free space = 50
    DataChunk b1 = randomIntChunk(t, 200, 20);                       // 200 > 50 -> overflow leftover = 150
    DataChunk probe = randomIntChunk(t, 128, 20);
    for (Binop op : kOps) expectJoin(t, {{&b0, &b1}}, probe, 0, 0, {0}, op);
}

TEST_F(SortMergeTest, EmptyBuild) {
    vector<LogicalType> t{LogicalTypeId::INTEGER};
    DataChunk probe = makeChunk<int>(t, {{1, 2, 3}});
    auto actual = runJoin(t, {}, probe, 0, 0, {0}, GREATER);
    EXPECT_TRUE(actual.empty());
}

TEST_F(SortMergeTest, EmptyProbeMorsel) {
    vector<LogicalType> t{LogicalTypeId::INTEGER};
    DataChunk build = makeChunk<int>(t, {{1, 2, 3}});
    DataChunk empty = emptyChunk(t);
    auto actual = runJoin(t, {{&build}}, empty, 0, 0, {0}, GREATER);
    EXPECT_TRUE(actual.empty());
}

// > STANDARD_VECTOR_SIZE build rows across two chunks => at least two pieces.
TEST_F(SortMergeTest, MultiPieceBuild) {
    vector<LogicalType> t{LogicalTypeId::INTEGER};
    vector<int> c0, c1;
    for (int i = 0; i < STANDARD_VECTOR_SIZE; ++i)     c0.push_back(i);
    for (int i = 0; i < STANDARD_VECTOR_SIZE / 2; ++i) c1.push_back(i);
    DataChunk b0 = makeChunk<int>(t, {c0});
    DataChunk b1 = makeChunk<int>(t, {c1});
    DataChunk probe = makeChunk<int>(t, {{0, 100, STANDARD_VECTOR_SIZE - 1, STANDARD_VECTOR_SIZE / 4}});
    for (Binop op : kOps) expectJoin(t, {{&b0, &b1}}, probe, 0, 0, {0}, op);
}

// One probe key greater than every build key => GREATER matches all, forcing >1 output batch.
TEST_F(SortMergeTest, AllMatchMultiBatch) {
    vector<LogicalType> t{LogicalTypeId::INTEGER};
    vector<int> all;
    for (int i = 0; i < STANDARD_VECTOR_SIZE + 500; ++i) all.push_back(i % 50);
    vector<DataChunk> chunks;
    for (int off = 0; off < (int)all.size(); off += STANDARD_VECTOR_SIZE) {
        vector<int> slice(all.begin() + off, all.begin() + std::min<int>(off + STANDARD_VECTOR_SIZE, all.size()));
        chunks.push_back(makeChunk<int>(t, {slice}));
    }
    vector<DataChunk*> thread;
    for (auto& c : chunks) thread.push_back(&c);
    DataChunk probe = makeChunk<int>(t, {{1000}});
    expectJoin(t, {thread}, probe, 0, 0, {0}, GREATER);
}

// Two thread-local indexes combined into one global index must match the brute-force result.
TEST_F(SortMergeTest, MultiThreadBuild) {
    vector<LogicalType> t{LogicalTypeId::INTEGER};
    DataChunk t0 = makeChunk<int>(t, {{5, 1, 9, 3}});
    DataChunk t1 = makeChunk<int>(t, {{7, 2, 8, 4, 6}});
    DataChunk probe = makeChunk<int>(t, {{4, 6, 0, 9, 5, 8}});
    for (Binop op : kOps) expectJoin(t, {{&t0}, {&t1}}, probe, 0, 0, {0}, op);
}

// Randomized stress over a small domain, multi-piece, all operators.
TEST_F(SortMergeTest, RandomizedStress) {
    vector<LogicalType> t{LogicalTypeId::INTEGER, LogicalTypeId::BIGINT};
    DataChunk b0 = randomIntChunk(t, STANDARD_VECTOR_SIZE, 30);
    DataChunk b1 = randomIntChunk(t, 700, 30);
    DataChunk probe = randomIntChunk(t, 256, 30);
    for (Binop op : kOps) expectJoin(t, {{&b0, &b1}}, probe, 0, 0, {0, 1}, op);
}
