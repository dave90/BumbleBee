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

#include "../../../BumbleBaseTest.hpp"
#include "bumblebee/ClientContext.hpp"
#include "bumblebee/catalog/PredicateTables.hpp"
#include "bumblebee/common/vector_operations/VectorOperations.hpp"
#include "bumblebee/execution/JoinHashTable.hpp"
#include "bumblebee/execution/PRLHashTable.hpp"
#include "bumblebee/execution/PartitionedPRLHashTable.hpp"
#include "bumblebee/execution/Expression.hpp"
#include "bumblebee/execution/JoinPRLHashTable.hpp"
#include "bumblebee/execution/RowLayoutJoinHashTable.hpp"

using namespace bumblebee;

// Phase 5 hash-table coverage. The dedup/group HTs use the IS-NOT-DISTINCT-FROM
// comparator (decision #1 of null_plan.md): two NULLs in the same key column are
// considered equal, so a recursion / GROUP BY / DISTINCT over null-bearing tuples
// reaches a fixed point.

class HTNullTest : public BumbleBaseTest {
};

// Build a DataChunk where col 0 holds sequential ints and col 1 holds the supplied
// values, with positions in `nullPositions` marked as NULL.
static DataChunk makeChunkWithNulls(idx_t N, const std::vector<idx_t> &nullPositions) {
    vector<LogicalType> types{PhysicalType::INTEGER, PhysicalType::INTEGER};
    DataChunk chunk;
    chunk.initialize(types);
    chunk.setCapacity(N);
    chunk.resize(N);
    std::unordered_set<idx_t> nullSet(nullPositions.begin(), nullPositions.end());
    for (idx_t i = 0; i < N; i++) {
        chunk.setValue(0, i, Value((int32_t)i));
        if (nullSet.count(i)) chunk.setValue(1, i, Value::null());
        else chunk.setValue(1, i, Value((int32_t)(i * 2)));
    }
    chunk.setCardinality(N);
    return chunk;
}

TEST_F(HTNullTest, PRLDedupTreatsTwoNullsAsEqual) {
    // Insert two identical null-bearing tuples into a dedup HT — the second one
    // must dedupe to the first, leaving the HT with exactly one row.
    vector<LogicalType> types{PhysicalType::INTEGER, PhysicalType::INTEGER};
    DataChunk first;
    first.initialize(types);
    first.setCapacity(1);
    first.resize(1);
    first.setValue(0, 0, Value((int32_t)5));
    first.setValue(1, 0, Value::null());
    first.setCardinality(1);

    DataChunk second;
    second.initialize(types);
    second.setCapacity(1);
    second.resize(1);
    second.setValue(0, 0, Value((int32_t)5));
    second.setValue(1, 0, Value::null());
    second.setCardinality(1);

    PRLHashTable ht(*clientContext.bufferManager_, types);
    ht.addChunk(first);
    EXPECT_EQ(ht.getSize(), 1u);
    ht.addChunk(second);
    // Two NULLs match under IS-NOT-DISTINCT-FROM → no new group.
    EXPECT_EQ(ht.getSize(), 1u);
}

TEST_F(HTNullTest, PRLDedupKeepsTuplesThatDifferInNonNullCol) {
    vector<LogicalType> types{PhysicalType::INTEGER, PhysicalType::INTEGER};
    DataChunk a, b;
    a.initialize(types); a.setCapacity(1); a.resize(1);
    a.setValue(0, 0, Value((int32_t)5));
    a.setValue(1, 0, Value::null());
    a.setCardinality(1);

    b.initialize(types); b.setCapacity(1); b.resize(1);
    b.setValue(0, 0, Value((int32_t)6));  // differs in col 0
    b.setValue(1, 0, Value::null());      // both NULL in col 1
    b.setCardinality(1);

    PRLHashTable ht(*clientContext.bufferManager_, types);
    ht.addChunk(a);
    ht.addChunk(b);
    EXPECT_EQ(ht.getSize(), 2u);
}

TEST_F(HTNullTest, PRLDedupRoundTripPreservesNulls) {
    // Round-trip a chunk with multiple nulls through the dedup HT and verify the
    // mask survives scan.
    const idx_t N = 50;
    auto chunk = makeChunkWithNulls(N, {5, 17, 42});
    vector<LogicalType> types{PhysicalType::INTEGER, PhysicalType::INTEGER};

    PRLHashTable ht(*clientContext.bufferManager_, types);
    ht.addChunk(chunk);
    EXPECT_EQ(ht.getSize(), N);  // every row distinct in col 0 → all kept

    // Scan back into a result chunk; preserve the NULL bits.
    DataChunk result;
    result.initialize(types);
    ht.scan(0, result, N);
    ASSERT_EQ(result.getSize(), N);

    // Build a (col0 -> col1 null?) map and check 3 null positions are NULL, others valid.
    std::unordered_set<int32_t> nullKeysInOutput;
    for (idx_t i = 0; i < result.getSize(); i++) {
        const auto k = result.getValue(0, i).getNumericValue<int32_t>();
        if (!result.data_[1].rowIsValid(i)) nullKeysInOutput.insert(k);
    }
    EXPECT_EQ(nullKeysInOutput.size(), 3u);
    EXPECT_TRUE(nullKeysInOutput.count(5));
    EXPECT_TRUE(nullKeysInOutput.count(17));
    EXPECT_TRUE(nullKeysInOutput.count(42));
}

TEST_F(HTNullTest, PartitionedPRLDedupAcrossMultipleBatches) {
    // Multi-batch (>5000) input split across 4 hash partitions; nulls must survive
    // and dedupe correctly. This is the configuration the e2e `null/null.multibatch`
    // test exercises under `-d`.
    const idx_t N = 5300;
    vector<idx_t> nullPositions;
    for (idx_t i = 0; i < N; i += 100) nullPositions.push_back(i);
    auto chunk = makeChunkWithNulls(N, nullPositions);
    vector<LogicalType> types{PhysicalType::INTEGER, PhysicalType::INTEGER};

    PartitionedPRLHashTable ht(*clientContext.bufferManager_, types);
    ht.addChunk(chunk);
    EXPECT_EQ(ht.getSize(), N);

    // Scan all entries in STANDARD_VECTOR_SIZE batches and re-collect the nulls.
    DataChunk result;
    result.initialize(types);
    idx_t offset = 0;
    idx_t totalNullCount = 0;
    std::unordered_set<int32_t> seenNullKeys;
    while (offset < N) {
        result.setCardinality(0);
        ht.scan(offset, result);
        for (idx_t i = 0; i < result.getSize(); i++) {
            if (!result.data_[1].rowIsValid(i)) {
                totalNullCount++;
                seenNullKeys.insert(result.getValue(0, i).getNumericValue<int32_t>());
            }
        }
        offset += result.getSize();
    }
    EXPECT_EQ(totalNullCount, nullPositions.size());
    // Every original null-bearing tuple is recovered (each col 0 key is distinct).
    EXPECT_EQ(seenNullKeys.size(), nullPositions.size());
}

TEST_F(HTNullTest, JoinHashTableExcludesNullKeysFromMatch) {
    // INNER join with a NULL key on either side must produce NO match (NULL-excludes).
    // Build side (`right`): rows {(0, 0), (1, 10), (2, 20)} with col 0 NULL at row 1.
    // Probe side (`left`):  rows {(0, 0), (1, 10), (2, 20)} with col 0 NULL at row 0.
    //
    // Without NULL-excludes the defensive NullValue<uint32_t>()=0 fill would let the
    // NULL build key spuriously match the probe's 0 — which is exactly the bug this
    // change closes.
    vector<LogicalType> types{PhysicalType::UINTEGER, PhysicalType::INTEGER};
    DataChunk rchunk, lchunk;
    rchunk.initialize(types); rchunk.setCapacity(3); rchunk.resize(3);
    rchunk.setValue(0, 0, Value((uint32_t)0));
    rchunk.setValue(1, 0, Value((int32_t)0));
    rchunk.setValue(0, 1, Value::null());  // NULL build key
    rchunk.setValue(1, 1, Value((int32_t)10));
    rchunk.setValue(0, 2, Value((uint32_t)2));
    rchunk.setValue(1, 2, Value((int32_t)20));
    rchunk.setCardinality(3);

    lchunk.initialize(types); lchunk.setCapacity(3); lchunk.resize(3);
    lchunk.setValue(0, 0, Value::null());  // NULL probe key
    lchunk.setValue(1, 0, Value((int32_t)0));
    lchunk.setValue(0, 1, Value((uint32_t)1));
    lchunk.setValue(1, 1, Value((int32_t)10));
    lchunk.setValue(0, 2, Value((uint32_t)2));
    lchunk.setValue(1, 2, Value((int32_t)20));
    lchunk.setCardinality(3);

    auto ptable = std::make_shared<PredicateTables>(&clientContext, "b", 2);
    vector<idx_t> keys{0};
    vector<idx_t> payloads{1};
    const idx_t buckets = 8;
    JoinHashTable ht(ptable->predicate_.get(), keys, payloads, buckets);

    Vector rhash(LogicalTypeId::HASH);
    rchunk.hash(rhash, keys);
    ht.addDataChunkSel(rhash, rchunk);
    ht.initDirectory();
    for (idx_t i = 0; i < buckets; i++) ht.build(i, i);

    Vector lhash(LogicalTypeId::HASH);
    lchunk.hash(lhash, keys);
    SelectionVector lsel(STANDARD_VECTOR_SIZE), rsel(STANDARD_VECTOR_SIZE);
    vector<Expression> conditions;
    conditions.push_back(Expression::generateExpression(EQUAL, 0, 0));

    idx_t lpos = 0, rpos = 0;
    idx_t total = 0;
    while (true) {
        auto got = ht.probe(lpos, rpos, lchunk, lhash, lsel, rsel, conditions);
        if (got == 0) break;
        total += got;
        // Every reported match must come from a probe row whose key is NOT NULL and
        // map to a build row whose key is NOT NULL.
        for (idx_t i = 0; i < got; i++) {
            auto lrow = lsel.getIndex(i);
            auto rrow = rsel.getIndex(i);
            EXPECT_TRUE(lchunk.data_[0].rowIsValid(lrow));
            EXPECT_TRUE(ht.getDataChunk().data_[0].rowIsValid(rrow));
        }
    }
    // Only row 2 (probe key=2, build key=2) is a valid match. NULLs are excluded.
    EXPECT_EQ(total, 1u);
}

TEST_F(HTNullTest, PartitionedPRLDedupCollapsesDuplicateNullTuples) {
    // Same key (col 0) AND same NULL in col 1 → must dedupe to one row, even when
    // the duplicates land in different chunks across the same partition.
    vector<LogicalType> types{PhysicalType::INTEGER, PhysicalType::INTEGER};
    DataChunk a, b;
    a.initialize(types); a.setCapacity(3); a.resize(3);
    for (idx_t i = 0; i < 3; i++) {
        a.setValue(0, i, Value((int32_t)i));
        a.setValue(1, i, Value::null());
    }
    a.setCardinality(3);

    b.initialize(types); b.setCapacity(3); b.resize(3);
    for (idx_t i = 0; i < 3; i++) {
        b.setValue(0, i, Value((int32_t)i));  // same keys as `a`
        b.setValue(1, i, Value::null());      // same NULLs as `a`
    }
    b.setCardinality(3);

    PartitionedPRLHashTable ht(*clientContext.bufferManager_, types);
    ht.addChunk(a);
    ht.addChunk(b);
    EXPECT_EQ(ht.getSize(), 3u);  // (0,NULL), (1,NULL), (2,NULL) — second batch fully deduped
}

// JoinPRLHashTable (the incremental row-layout join HT used for recursion): NULL
// build/probe keys are excluded under NULL-excludes — routed through
// RowOperations::equal in JoinPRLHashTable::probe.
TEST_F(HTNullTest, JoinPRLHashTableExcludesNullKeys) {
    vector<LogicalType> types{PhysicalType::INTEGER, PhysicalType::INTEGER};
    vector<idx_t> keys{0};
    vector<idx_t> payload{1};

    DataChunk rchunk;
    rchunk.initialize(types); rchunk.setCapacity(3); rchunk.resize(3);
    rchunk.setValue(0, 0, Value((int32_t)0));
    rchunk.setValue(1, 0, Value((int32_t)0));
    rchunk.setValue(0, 1, Value::null());  // NULL build key
    rchunk.setValue(1, 1, Value((int32_t)10));
    rchunk.setValue(0, 2, Value((int32_t)2));
    rchunk.setValue(1, 2, Value((int32_t)20));
    rchunk.setCardinality(3);

    auto ht = std::make_unique<JoinPRLHashTable>(*clientContext.bufferManager_, types, keys, payload, 8, false);
    ht->addChunk(rchunk);

    // Probe keys: row 0 NULL → no match; row 1 = 2 → matches the build row 2.
    vector<LogicalType> keyTypes{PhysicalType::INTEGER};
    DataChunk lchunk;
    lchunk.initialize(keyTypes); lchunk.setCapacity(2); lchunk.resize(2);
    lchunk.setValue(0, 0, Value::null());
    lchunk.setValue(0, 1, Value((int32_t)2));
    lchunk.setCardinality(2);

    Vector lhash(LogicalTypeId::HASH);
    lchunk.hash(lhash);
    SelectionVector lsel(STANDARD_VECTOR_SIZE), rsel(STANDARD_VECTOR_SIZE);
    idx_t lpos = 0, rpos = 0;
    DataChunk result;
    ht->probe(lpos, rpos, lchunk, lhash, lsel, rsel, result);

    EXPECT_EQ(result.getSize(), 1u);  // only probe row 1 (=2) matches build row 2
    EXPECT_EQ(lsel.getIndex(0), 1u);  // the non-NULL probe row
}

// RowLayoutJoinHashTable (the incremental join HT for delta/recursive maintenance):
// NULL build/probe keys excluded under NULL-excludes — same `RowOperations::equal`
// path as JoinPRLHashTable. Distinguished from JoinPRLHashTable by its `match()`
// API used by the delta pipeline.
TEST_F(HTNullTest, RowLayoutJoinHashTableExcludesNullKeys) {
    vector<LogicalType> types{PhysicalType::INTEGER, PhysicalType::INTEGER};
    vector<idx_t> keys{0};
    vector<idx_t> payload{1};

    DataChunk rchunk;
    rchunk.initialize(types); rchunk.setCapacity(3); rchunk.resize(3);
    rchunk.setValue(0, 0, Value((int32_t)5));
    rchunk.setValue(1, 0, Value((int32_t)50));
    rchunk.setValue(0, 1, Value::null());  // NULL build key
    rchunk.setValue(1, 1, Value((int32_t)10));
    rchunk.setValue(0, 2, Value((int32_t)7));
    rchunk.setValue(1, 2, Value((int32_t)70));
    rchunk.setCardinality(3);

    RowLayoutJoinHashTable ht(*clientContext.bufferManager_, types, keys, payload);
    ht.addChunk(rchunk);
    ht.finalize();

    // match() expects an lchunk with exactly key columns.
    vector<LogicalType> keyTypes{PhysicalType::INTEGER};
    DataChunk lchunk;
    lchunk.initialize(keyTypes); lchunk.setCapacity(3); lchunk.resize(3);
    lchunk.setValue(0, 0, Value::null());     // NULL probe key
    lchunk.setValue(0, 1, Value((int32_t)5)); // matches build row 0
    lchunk.setValue(0, 2, Value((int32_t)99));// no build row with key 99
    lchunk.setCardinality(3);

    Vector lhash(LogicalTypeId::HASH);
    lchunk.hash(lhash);

    SelectionVector mSel(STANDARD_VECTOR_SIZE), nmSel(STANDARD_VECTOR_SIZE);
    idx_t mcount = 0, nmcount = 0;
    ht.match(lchunk, lhash, mSel, mcount, nmSel, nmcount);

    EXPECT_EQ(mcount, 1u);
    EXPECT_EQ(mSel.getIndex(0), 1u);  // probe row 1 (=5) matched
    EXPECT_EQ(nmcount, 2u);           // probe rows 0 (NULL) and 2 (=99) excluded
}

// JoinHashTable refine path (multiple conditions): the FIRST condition seeds the
// candidate set; subsequent conditions refine it. NULL on a refine condition's
// column must drop the candidate. Exercises RefineHashJoin::operation, which
// Phase 5 also extended with the validity pointers.
TEST_F(HTNullTest, JoinHashTableRefinePathDropsCandidateOnNullSecondCondition) {
    // Build: row k has key=k and a second column whose value is k*10, with NULL at row 1.
    // Probe: key = same as build, but probe's col 1 has values 0, 10, 20 (no nulls).
    // Conditions: (1) left.col0 == right.col0  (2) left.col1 == right.col1.
    // Without NULL-excludes: row 1's NULL stored as defensive fill could spuriously
    // refine-match against probe row 1 (= 10) — which is wrong. With NULL-excludes,
    // refine drops it.
    vector<LogicalType> types{PhysicalType::INTEGER, PhysicalType::INTEGER};
    DataChunk rchunk;
    rchunk.initialize(types); rchunk.setCapacity(3); rchunk.resize(3);
    rchunk.setValue(0, 0, Value((int32_t)0));
    rchunk.setValue(1, 0, Value((int32_t)0));
    rchunk.setValue(0, 1, Value((int32_t)1));
    rchunk.setValue(1, 1, Value::null());  // NULL on the REFINE-condition column
    rchunk.setValue(0, 2, Value((int32_t)2));
    rchunk.setValue(1, 2, Value((int32_t)20));
    rchunk.setCardinality(3);

    DataChunk lchunk;
    lchunk.initialize(types); lchunk.setCapacity(3); lchunk.resize(3);
    lchunk.setValue(0, 0, Value((int32_t)0));
    lchunk.setValue(1, 0, Value((int32_t)0));
    lchunk.setValue(0, 1, Value((int32_t)1));
    lchunk.setValue(1, 1, Value((int32_t)10));  // would refine-match a NULL build fill if not excluded
    lchunk.setValue(0, 2, Value((int32_t)2));
    lchunk.setValue(1, 2, Value((int32_t)20));
    lchunk.setCardinality(3);

    auto ptable = std::make_shared<PredicateTables>(&clientContext, "b", 2);
    vector<idx_t> keys{0};
    vector<idx_t> payloads{1};
    const idx_t buckets = 8;
    JoinHashTable ht(ptable->predicate_.get(), keys, payloads, buckets);

    Vector rhash(LogicalTypeId::HASH);
    rchunk.hash(rhash, keys);
    ht.addDataChunkSel(rhash, rchunk);
    ht.initDirectory();
    for (idx_t i = 0; i < buckets; i++) ht.build(i, i);

    Vector lhash(LogicalTypeId::HASH);
    lchunk.hash(lhash, keys);
    SelectionVector lsel(STANDARD_VECTOR_SIZE), rsel(STANDARD_VECTOR_SIZE);
    vector<Expression> conditions;
    conditions.push_back(Expression::generateExpression(EQUAL, 0, 0));
    conditions.push_back(Expression::generateExpression(EQUAL, 1, 1));  // refine condition

    idx_t lpos = 0, rpos = 0;
    idx_t total = 0;
    while (true) {
        auto got = ht.probe(lpos, rpos, lchunk, lhash, lsel, rsel, conditions);
        if (!got) break;
        for (idx_t i = 0; i < got; i++) {
            // Every matched row's BUILD side must have valid col 1 (the refine column).
            auto rrow = rsel.getIndex(i);
            EXPECT_TRUE(ht.getDataChunk().data_[1].rowIsValid(rrow));
        }
        total += got;
    }
    // Only rows 0 (0,0) and 2 (2,20) match. Row 1 dropped by refine because the
    // build's col 1 is NULL.
    EXPECT_EQ(total, 2u);
}

// Multi-key join: 2-key join where the SECOND key has a NULL on the build side
// for one row → that row must be excluded under NULL-excludes, even though the
// FIRST key matches a probe row.
TEST_F(HTNullTest, JoinHashTableMultiKeyOneKeyNullExcludes) {
    vector<LogicalType> types{PhysicalType::INTEGER, PhysicalType::INTEGER};
    DataChunk rchunk;
    rchunk.initialize(types); rchunk.setCapacity(2); rchunk.resize(2);
    rchunk.setValue(0, 0, Value((int32_t)5));
    rchunk.setValue(1, 0, Value((int32_t)50));
    rchunk.setValue(0, 1, Value((int32_t)5));
    rchunk.setValue(1, 1, Value::null());  // NULL in second key
    rchunk.setCardinality(2);

    DataChunk lchunk;
    lchunk.initialize(types); lchunk.setCapacity(1); lchunk.resize(1);
    lchunk.setValue(0, 0, Value((int32_t)5));
    lchunk.setValue(1, 0, Value((int32_t)50));
    lchunk.setCardinality(1);

    auto ptable = std::make_shared<PredicateTables>(&clientContext, "b", 2);
    vector<idx_t> keys{0, 1};
    vector<idx_t> payloads{};
    const idx_t buckets = 8;
    JoinHashTable ht(ptable->predicate_.get(), keys, payloads, buckets);

    Vector rhash(LogicalTypeId::HASH);
    rchunk.hash(rhash, keys);
    ht.addDataChunkSel(rhash, rchunk);
    ht.initDirectory();
    for (idx_t i = 0; i < buckets; i++) ht.build(i, i);

    Vector lhash(LogicalTypeId::HASH);
    lchunk.hash(lhash, keys);
    SelectionVector lsel(STANDARD_VECTOR_SIZE), rsel(STANDARD_VECTOR_SIZE);
    vector<Expression> conditions;
    conditions.push_back(Expression::generateExpression(EQUAL, 0, 0));
    conditions.push_back(Expression::generateExpression(EQUAL, 1, 1));

    idx_t lpos = 0, rpos = 0;
    idx_t total = 0;
    while (true) {
        auto got = ht.probe(lpos, rpos, lchunk, lhash, lsel, rsel, conditions);
        if (!got) break;
        total += got;
    }
    // Only build row 0 (5, 50) matches probe row 0. Build row 1's second key is NULL.
    EXPECT_EQ(total, 1u);
}