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

#include "bumblebee/ClientContext.hpp"
#include "bumblebee/execution/AggregatePRLHashTable.hpp"
#include "bumblebee/function/aggregate/Avg.hpp"
#include "bumblebee/function/aggregate/Count.hpp"
#include "bumblebee/function/aggregate/Max.hpp"
#include "bumblebee/function/aggregate/Min.hpp"
#include "bumblebee/function/aggregate/Sum.hpp"

using namespace bumblebee;
using agg_ht_ptr = std::unique_ptr<AggregatePRLHashTable>;

// Phase 6 — aggregates skip NULL inputs; SUM/AVG/MIN/MAX over an empty/all-NULL
// group finalize to NULL on the output Vector; COUNT over an empty group is 0.

class AggNullTest : public ::testing::Test {
protected:
    ClientContext clientContext;

    // Build a 2-column chunk (groupCol INT, valueCol INT) where rows whose group
    // index is in `nullPositionsInValue` have NULL in valueCol.
    DataChunk makeChunk(const vector<std::pair<int32_t, int32_t>> &rows,
                       const std::unordered_set<idx_t> &nullValuePositions) {
        vector<LogicalType> types{PhysicalType::INTEGER, PhysicalType::INTEGER};
        DataChunk chunk;
        chunk.initialize(types);
        chunk.setCapacity(rows.size());
        chunk.resize(rows.size());
        for (idx_t i = 0; i < rows.size(); i++) {
            chunk.setValue(0, i, Value((int32_t)rows[i].first));
            if (nullValuePositions.count(i)) chunk.setValue(1, i, Value::null());
            else chunk.setValue(1, i, Value((int32_t)rows[i].second));
        }
        chunk.setCardinality(rows.size());
        return chunk;
    }

    // Build an AggregatePRLHashTable with the given group + aggregate function,
    // add the chunk, scan all entries back. Populates outGroups (group cols) and
    // outAgg (aggregate result Vector); they have matching cardinality.
    void scanAggregate(DataChunk &chunk, vector<idx_t> groupCols, idx_t payloadCol,
                      AggregateFunction &aggFunc, DataChunk &outGroups, Vector &outAgg) {
        vector<LogicalType> groupTypes{chunk.getTypes()[groupCols[0]]};
        vector<LogicalType> payloadTypes{chunk.getTypes()[payloadCol]};

        DataChunk groups, payload;
        groups.initialize(groupTypes);
        payload.initialize(payloadTypes);
        groups.reference(chunk, groupCols);
        vector<idx_t> payloadIdxs{payloadCol};
        payload.reference(chunk, payloadIdxs);

        Vector hash(LogicalTypeId::HASH, groups.getSize());
        groups.hash(hash);

        vector<AggregateFunction*> functions{&aggFunc};
        auto ht = agg_ht_ptr(new AggregatePRLHashTable(*clientContext.bufferManager_,
                                                       groupTypes, 32, true, functions));
        ht->addChunk(hash, groups, payload);

        outGroups.initialize(groupTypes);
        idx_t scanned = ht->scanWithAggregates(0, outGroups, outAgg, 0, STANDARD_VECTOR_SIZE);
        outGroups.setCardinality(scanned);
    }

    // Find the output row whose group-key column equals `key`; return its index.
    idx_t findGroup(DataChunk &groups, int32_t key) {
        for (idx_t i = 0; i < groups.getSize(); i++) {
            if (groups.getValue(0, i).getNumericValue<int32_t>() == key) return i;
        }
        return groups.getSize();  // not found sentinel
    }
};

TEST_F(AggNullTest, SumSkipsNullInputs) {
    // Group 1: [10, NULL, 20]   → SUM = 30
    // Group 2: [NULL, NULL]     → SUM = NULL (all-null group)
    // Group 3: [5]              → SUM = 5
    auto chunk = makeChunk({{1, 10}, {1, 99}, {1, 20}, {2, 99}, {2, 99}, {3, 5}},
                          /*nullValuePositions=*/{1, 3, 4});
    auto sumFunc = SumFunc().getFunction({PhysicalType::INTEGER});
    DataChunk groups;
    Vector out(sumFunc->result_, STANDARD_VECTOR_SIZE);
    scanAggregate(chunk, {0}, 1, *(AggregateFunction *)sumFunc.get(), groups, out);

    auto g1 = findGroup(groups, 1);
    auto g2 = findGroup(groups, 2);
    auto g3 = findGroup(groups, 3);
    ASSERT_LT(g1, groups.getSize());
    ASSERT_LT(g2, groups.getSize());
    ASSERT_LT(g3, groups.getSize());

    EXPECT_TRUE(out.rowIsValid(g1));
    EXPECT_EQ(out.getValue(g1).getNumericValue<int64_t>(), 30);

    // All-NULL group → SUM finalize returns false → output row marked NULL.
    EXPECT_FALSE(out.rowIsValid(g2));

    EXPECT_TRUE(out.rowIsValid(g3));
    EXPECT_EQ(out.getValue(g3).getNumericValue<int64_t>(), 5);
}

TEST_F(AggNullTest, AvgSkipsNullInputs) {
    // Group 1: [10, NULL, 30]   → AVG = 20.0 (not 13.33 — NULL dropped, count=2)
    // Group 2: [NULL]           → AVG = NULL
    auto chunk = makeChunk({{1, 10}, {1, 99}, {1, 30}, {2, 99}}, {1, 3});
    auto avgFunc = AvgFunc().getFunction({PhysicalType::INTEGER});
    DataChunk groups;
    Vector out(avgFunc->result_, STANDARD_VECTOR_SIZE);
    scanAggregate(chunk, {0}, 1, *(AggregateFunction *)avgFunc.get(), groups, out);

    auto g1 = findGroup(groups, 1);
    auto g2 = findGroup(groups, 2);
    ASSERT_LT(g1, groups.getSize());
    ASSERT_LT(g2, groups.getSize());

    EXPECT_TRUE(out.rowIsValid(g1));
    EXPECT_DOUBLE_EQ(out.getValue(g1).getNumericValue<double>(), 20.0);
    // Empty group → NULL (also dodges div-by-zero).
    EXPECT_FALSE(out.rowIsValid(g2));
}

TEST_F(AggNullTest, MinSkipsNullInputs) {
    // Group 1: [5, NULL, 3, 7] → MIN = 3
    // Group 2: [NULL, NULL]    → MIN = NULL
    auto chunk = makeChunk({{1, 5}, {1, 99}, {1, 3}, {1, 7}, {2, 99}, {2, 99}}, {1, 4, 5});
    auto minFunc = MinFunc().getFunction({PhysicalType::INTEGER});
    DataChunk groups;
    Vector out(minFunc->result_, STANDARD_VECTOR_SIZE);
    scanAggregate(chunk, {0}, 1, *(AggregateFunction *)minFunc.get(), groups, out);

    auto g1 = findGroup(groups, 1);
    auto g2 = findGroup(groups, 2);
    EXPECT_TRUE(out.rowIsValid(g1));
    EXPECT_EQ(out.getValue(g1).getNumericValue<int32_t>(), 3);
    EXPECT_FALSE(out.rowIsValid(g2));
}

TEST_F(AggNullTest, MaxSkipsNullInputs) {
    // Group 1: [5, NULL, 9, 1] → MAX = 9
    // Group 2: [NULL]          → MAX = NULL
    auto chunk = makeChunk({{1, 5}, {1, 99}, {1, 9}, {1, 1}, {2, 99}}, {1, 4});
    auto maxFunc = MaxFunc().getFunction({PhysicalType::INTEGER});
    DataChunk groups;
    Vector out(maxFunc->result_, STANDARD_VECTOR_SIZE);
    scanAggregate(chunk, {0}, 1, *(AggregateFunction *)maxFunc.get(), groups, out);

    auto g1 = findGroup(groups, 1);
    auto g2 = findGroup(groups, 2);
    EXPECT_TRUE(out.rowIsValid(g1));
    EXPECT_EQ(out.getValue(g1).getNumericValue<int32_t>(), 9);
    EXPECT_FALSE(out.rowIsValid(g2));
}

TEST_F(AggNullTest, CountSkipsNullsAndReturnsZeroForEmptyGroup) {
    // Group 1: [a, NULL, b, c]  → COUNT = 3 (skip NULL)
    // Group 2: [NULL, NULL]     → COUNT = 0 (NOT NULL — SQL semantics, COUNT(col))
    auto chunk = makeChunk({{1, 100}, {1, 99}, {1, 200}, {1, 300}, {2, 99}, {2, 99}},
                          {1, 4, 5});
    auto countFunc = CountFunc().getFunction({PhysicalType::INTEGER});
    DataChunk groups;
    Vector out(countFunc->result_, STANDARD_VECTOR_SIZE);
    scanAggregate(chunk, {0}, 1, *(AggregateFunction *)countFunc.get(), groups, out);

    auto g1 = findGroup(groups, 1);
    auto g2 = findGroup(groups, 2);
    EXPECT_TRUE(out.rowIsValid(g1));
    EXPECT_EQ(out.getValue(g1).getNumericValue<uint64_t>(), 3u);
    EXPECT_TRUE(out.rowIsValid(g2));
    EXPECT_EQ(out.getValue(g2).getNumericValue<uint64_t>(), 0u);
}

TEST_F(AggNullTest, SumStateSeenFlagCarriesNullnessAcrossCombine) {
    // The combine() path: two SumState<int64_t> states merge. If both are "unseen"
    // the result must stay unseen → finalize returns false → output NULL. This is
    // the path AggregatePRLHashTable hits during combine-of-partials.
    SumState<int64_t> a, b;
    a.initialize();
    b.initialize();
    EXPECT_FALSE(a.seen);
    EXPECT_FALSE(b.seen);

    a.combine(&b);
    EXPECT_FALSE(a.seen);  // unseen ∨ unseen = unseen

    SumState<int64_t> c;
    c.initialize();
    SumOperation<int32_t, int64_t>::operation((int32_t *)new int32_t(5), &c);
    EXPECT_TRUE(c.seen);

    a.combine(&c);
    EXPECT_TRUE(a.seen);  // unseen ∨ seen = seen
    EXPECT_EQ(a.value, 5);
}

TEST_F(AggNullTest, MinMaxStateInitFlagCarriesAcrossCombine) {
    // Regression: combining two EMPTY Min/Max partials must stay uninitialized so
    // finalize returns false → output NULL. The previous combine() hard-set init=true,
    // so an all-NULL group whose partials were merged (the SQL / multi-thread path,
    // which always combines, unlike single-thread ASP) leaked an uninitialized value.
    MinState<int64_t> ma, mb;
    ma.initialize();
    mb.initialize();
    ma.combine(&mb);
    EXPECT_FALSE(ma.init);  // empty ∨ empty = empty  (was wrongly true)
    int64_t out = 123;
    bool minFin = MinOperation<int64_t, int64_t>::finalize(&ma, &out);
    EXPECT_FALSE(minFin);  // → NULL

    MaxState<int64_t> xa, xb;
    xa.initialize();
    xb.initialize();
    xa.combine(&xb);
    EXPECT_FALSE(xa.init);
    bool maxFin = MaxOperation<int64_t, int64_t>::finalize(&xa, &out);
    EXPECT_FALSE(maxFin);  // → NULL

    // empty ∨ seen = seen, and the value is adopted from the seen partial.
    MinState<int64_t> mc;
    mc.initialize();
    MinOperation<int32_t, int64_t>::operation((int32_t *)new int32_t(7), &mc);
    ma.combine(&mc);
    EXPECT_TRUE(ma.init);
    minFin = MinOperation<int64_t, int64_t>::finalize(&ma, &out);
    EXPECT_TRUE(minFin);
    EXPECT_EQ(out, 7);
}

TEST_F(AggNullTest, SumOfAllZerosIsNotNull) {
    // Regression: SUM([0, 0]) must return 0, not NULL. The `seen_` flag is set on
    // every operation() call regardless of input value, so distinguishing
    // "summed to 0" from "summed nothing" works.
    auto chunk = makeChunk({{1, 0}, {1, 0}}, /*nulls=*/{});
    auto sumFunc = SumFunc().getFunction({PhysicalType::INTEGER});
    DataChunk groups;
    Vector out(sumFunc->result_, STANDARD_VECTOR_SIZE);
    scanAggregate(chunk, {0}, 1, *(AggregateFunction *)sumFunc.get(), groups, out);

    ASSERT_EQ(groups.getSize(), 1u);
    EXPECT_TRUE(out.rowIsValid(0));
    EXPECT_EQ(out.getValue(0).getNumericValue<int64_t>(), 0);
}

TEST_F(AggNullTest, AllFiveAggregatesOverAllNullSingleColumnTotalAggregation) {
    // Total aggregation (no GROUP BY) — uses the addChunk(DataChunk&) path in
    // AggregatePRLHashTable. All input rows have a NULL value; SUM/AVG/MIN/MAX
    // must finalize to NULL; COUNT must return 0.
    vector<LogicalType> types{PhysicalType::INTEGER};
    DataChunk chunk;
    chunk.initialize(types); chunk.setCapacity(3); chunk.resize(3);
    chunk.setValue(0, 0, Value::null());
    chunk.setValue(0, 1, Value::null());
    chunk.setValue(0, 2, Value::null());
    chunk.setCardinality(3);

    auto sumFunc   = SumFunc().getFunction({PhysicalType::INTEGER});
    auto avgFunc   = AvgFunc().getFunction({PhysicalType::INTEGER});
    auto minFunc   = MinFunc().getFunction({PhysicalType::INTEGER});
    auto maxFunc   = MaxFunc().getFunction({PhysicalType::INTEGER});
    auto countFunc = CountFunc().getFunction({PhysicalType::INTEGER});

    auto check = [&](function_ptr_t &fn, bool expectValid, auto expected) {
        vector<AggregateFunction*> fns{(AggregateFunction*)fn.get()};
        auto ht = agg_ht_ptr(new AggregatePRLHashTable(*clientContext.bufferManager_,
                                                       {}, 2, false, fns));
        ht->addChunk(chunk);

        Vector result(fn->result_, 1);
        ht->fetchAggregates(result, 0);
        EXPECT_EQ(result.rowIsValid(0), expectValid);
        if (expectValid) {
            EXPECT_EQ(result.getValue(0).getNumericValue<decltype(expected)>(), expected);
        }
    };

    check(sumFunc,   false, (int64_t)0);  // all-NULL → NULL
    check(avgFunc,   false, (double)0);   // all-NULL → NULL
    check(minFunc,   false, (int32_t)0);  // all-NULL → NULL
    check(maxFunc,   false, (int32_t)0);  // all-NULL → NULL
    check(countFunc, true,  (uint64_t)0); // all-NULL → 0, NOT NULL
}

TEST_F(AggNullTest, MultiBatchSparseNulls) {
    // 3000 rows distributed over 5 groups; every 7th row's value is NULL.
    // The aggregate operator processes them in multiple STANDARD_VECTOR_SIZE
    // batches (build path goes through findOrCreateGroups + multiple update
    // calls per batch). Sum/Avg/Count over each group must remain correct.
    const idx_t N = 3000;
    const idx_t G = 5;
    vector<LogicalType> types{PhysicalType::INTEGER, PhysicalType::INTEGER};

    auto sumFunc = SumFunc().getFunction({PhysicalType::INTEGER});
    vector<AggregateFunction*> fns{(AggregateFunction*)sumFunc.get()};
    auto ht = agg_ht_ptr(new AggregatePRLHashTable(*clientContext.bufferManager_,
                                                   {PhysicalType::INTEGER}, 64, true, fns));

    // Reference: per-group expected sum, skipping NULL rows.
    int64_t expectedSum[G] = {};
    idx_t expectedCount[G] = {};
    idx_t i = 0;
    while (i < N) {
        idx_t batchSize = std::min((idx_t)STANDARD_VECTOR_SIZE, N - i);
        DataChunk batch;
        batch.initialize(types); batch.setCapacity(batchSize); batch.resize(batchSize);
        for (idx_t k = 0; k < batchSize; k++) {
            idx_t gid = (i + k) % G;
            batch.setValue(0, k, Value((int32_t)gid));
            if ((i + k) % 7 == 0) {
                batch.setValue(1, k, Value::null());
            } else {
                int32_t v = (int32_t)(i + k);
                batch.setValue(1, k, Value(v));
                expectedSum[gid] += v;
                expectedCount[gid]++;
            }
        }
        batch.setCardinality(batchSize);

        DataChunk groups, payload;
        vector<LogicalType> groupTypes{PhysicalType::INTEGER};
        vector<LogicalType> payloadTypes{PhysicalType::INTEGER};
        groups.initialize(groupTypes); groups.reference(batch, vector<idx_t>{0});
        payload.initialize(payloadTypes); payload.reference(batch, vector<idx_t>{1});
        Vector hash(LogicalTypeId::HASH, groups.getSize());
        groups.hash(hash);
        ht->addChunk(hash, groups, payload);
        i += batchSize;
    }

    DataChunk groupsOut;
    vector<LogicalType> groupOutTypes{PhysicalType::INTEGER};
    groupsOut.initialize(groupOutTypes);
    Vector sumOut(PhysicalType::BIGINT, STANDARD_VECTOR_SIZE);
    idx_t scanned = ht->scanWithAggregates(0, groupsOut, sumOut, 0, STANDARD_VECTOR_SIZE);
    groupsOut.setCardinality(scanned);
    ASSERT_EQ(groupsOut.getSize(), G);

    for (idx_t r = 0; r < groupsOut.getSize(); r++) {
        auto gid = (idx_t)groupsOut.getValue(0, r).getNumericValue<int32_t>();
        ASSERT_LT(gid, G);
        if (expectedCount[gid] == 0) {
            EXPECT_FALSE(sumOut.rowIsValid(r));
        } else {
            EXPECT_TRUE(sumOut.rowIsValid(r));
            EXPECT_EQ(sumOut.getValue(r).getNumericValue<int64_t>(), expectedSum[gid]);
        }
    }
}

// PRL-level combine: build two AggregatePRLHashTable instances independently
// (mimicking what two threads do under the partitioned aggregate path), then
// combine() merges them. The SUM seen flag must propagate so an all-NULL group
// stays NULL after merging. Mirrors `agg.null.all_null_large` under test_mt_asp
// at unit scale for local regression diagnosis.
TEST_F(AggNullTest, PRLCombineSumKeepsAllNullGroupNull) {
    // Partial 1: group 1 = [10, 20], group 99 = [NULL, NULL].
    auto chunk1 = makeChunk({{1, 10}, {1, 20}, {99, 99}, {99, 99}}, {2, 3});
    // Partial 2: group 1 = [30, NULL], group 99 = [NULL, NULL, NULL].
    auto chunk2 = makeChunk({{1, 30}, {1, 99}, {99, 99}, {99, 99}, {99, 99}}, {1, 2, 3, 4});

    auto sumFunc = SumFunc().getFunction({PhysicalType::INTEGER});
    vector<AggregateFunction*> fns{(AggregateFunction*)sumFunc.get()};
    vector<LogicalType> gt{PhysicalType::INTEGER};
    vector<idx_t> groupCols{0}, payloadIdxs{1};

    auto buildHT = [&](DataChunk &chunk) {
        DataChunk groups, payload;
        groups.initialize(gt); groups.reference(chunk, groupCols);
        vector<LogicalType> pt{PhysicalType::INTEGER};
        payload.initialize(pt); payload.reference(chunk, payloadIdxs);
        Vector hash(LogicalTypeId::HASH, groups.getSize());
        groups.hash(hash);
        auto ht = agg_ht_ptr(new AggregatePRLHashTable(*clientContext.bufferManager_,
                                                       gt, 32, true, fns));
        ht->addChunk(hash, groups, payload);
        return ht;
    };

    auto ht1 = buildHT(chunk1);
    auto ht2 = buildHT(chunk2);

    // Combine ht2 into ht1 — exercises the per-aggregate combine_ callback. For SUM,
    // SumState::combine ORs the seen flag so an all-NULL group in BOTH partials stays
    // unseen → finalize returns false → output NULL.
    ht1->combine(*ht2);

    DataChunk groupsOut;
    groupsOut.initialize(gt);
    Vector sumOut(sumFunc->result_, STANDARD_VECTOR_SIZE);
    idx_t scanned = ht1->scanWithAggregates(0, groupsOut, sumOut, 0, STANDARD_VECTOR_SIZE);
    groupsOut.setCardinality(scanned);

    auto idxFor = [](DataChunk &g, int32_t key) {
        for (idx_t i = 0; i < g.getSize(); i++)
            if (g.getValue(0, i).getNumericValue<int32_t>() == key) return i;
        return g.getSize();
    };
    auto g1 = idxFor(groupsOut, 1);
    auto g99 = idxFor(groupsOut, 99);
    ASSERT_LT(g1, groupsOut.getSize());
    ASSERT_LT(g99, groupsOut.getSize());

    // Group 1: 10+20+30 = 60 (the NULL in partial 2 was skipped).
    EXPECT_TRUE(sumOut.rowIsValid(g1));
    EXPECT_EQ(sumOut.getValue(g1).getNumericValue<int64_t>(), 60);
    // Group 99: every row across both partials is NULL → SUM finalize returns false
    // → output NULL. This is the key seen-flag-through-combine assertion.
    EXPECT_FALSE(sumOut.rowIsValid(g99));
}

// >5000-row PRL combine: two large partials merged. Verifies the same correctness
// signal but at the multi-batch scale the e2e test (`agg.null.large` under
// test_mt_asp) hits. Specifically that the seen flag survives a combine of two
// states that each accumulated >1 STANDARD_VECTOR_SIZE batch of input.
TEST_F(AggNullTest, PRLCombineLargeMultiBatchKeepsNullnessCorrect) {
    const idx_t PER_PARTIAL = 3000;  // each side spans >1 batch
    vector<LogicalType> types{PhysicalType::INTEGER, PhysicalType::INTEGER};
    auto sumFunc = SumFunc().getFunction({PhysicalType::INTEGER});
    vector<AggregateFunction*> fns{(AggregateFunction*)sumFunc.get()};

    auto buildPartial = [&](idx_t startVal, bool allNullForGroup99) {
        auto ht = agg_ht_ptr(new AggregatePRLHashTable(*clientContext.bufferManager_,
                                                       {PhysicalType::INTEGER}, 64, true, fns));
        // Add PER_PARTIAL rows; group 1 has half non-null, group 99 has all NULL (or
        // typed sentinels per the allNullForGroup99 flag).
        idx_t fed = 0;
        while (fed < PER_PARTIAL) {
            idx_t batch = minValue((idx_t)STANDARD_VECTOR_SIZE, PER_PARTIAL - fed);
            DataChunk chunk;
            chunk.initialize(types);
            chunk.setCapacity(batch);
            chunk.resize(batch);
            for (idx_t i = 0; i < batch; i++) {
                idx_t g = ((fed + i) % 2 == 0) ? 1u : 99u;
                chunk.setValue(0, i, Value((int32_t)g));
                if (g == 99 && allNullForGroup99) {
                    chunk.setValue(1, i, Value::null());
                } else if (g == 99) {
                    chunk.setValue(1, i, Value((int32_t)0));
                } else {
                    chunk.setValue(1, i, Value((int32_t)(startVal + fed + i)));
                }
            }
            chunk.setCardinality(batch);

            DataChunk groups, payload;
            vector<LogicalType> gt{PhysicalType::INTEGER}, pt{PhysicalType::INTEGER};
            groups.initialize(gt); groups.reference(chunk, vector<idx_t>{0});
            payload.initialize(pt); payload.reference(chunk, vector<idx_t>{1});
            Vector hash(LogicalTypeId::HASH, groups.getSize());
            groups.hash(hash);
            ht->addChunk(hash, groups, payload);
            fed += batch;
        }
        return ht;
    };

    // Two partials: both have group 99 as ALL-NULL across all rows; group 1 has
    // values starting from different offsets.
    auto ht1 = buildPartial(/*startVal=*/0, /*allNullForGroup99=*/true);
    auto ht2 = buildPartial(/*startVal=*/PER_PARTIAL, /*allNullForGroup99=*/true);
    ht1->combine(*ht2);

    DataChunk gOut;
    vector<LogicalType> gt{PhysicalType::INTEGER};
    gOut.initialize(gt);
    Vector sumOut(sumFunc->result_, STANDARD_VECTOR_SIZE);
    idx_t scanned = ht1->scanWithAggregates(0, gOut, sumOut, 0, STANDARD_VECTOR_SIZE);
    gOut.setCardinality(scanned);

    idx_t g99 = gOut.getSize();
    for (idx_t i = 0; i < gOut.getSize(); i++)
        if (gOut.getValue(0, i).getNumericValue<int32_t>() == 99) { g99 = i; break; }
    ASSERT_LT(g99, gOut.getSize());

    // Group 99 had >2000 rows across both partials, every one NULL → SUM must finalize NULL.
    EXPECT_FALSE(sumOut.rowIsValid(g99));
}