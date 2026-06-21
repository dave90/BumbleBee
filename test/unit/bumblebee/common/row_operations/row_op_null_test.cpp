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

#include "../../BumbleBaseTest.hpp"
#include "bumblebee/ClientContext.hpp"
#include "bumblebee/common/row_operations/RowOperations.hpp"
#include "bumblebee/common/types/RowDataCollection.hpp"

using namespace bumblebee;

// Tests for Phase 5: the per-row validity bitmap prefix in RowLayout.
//
// Coverage:
//   - RowLayout exposes flagWidth_ matching ceil(cols/8) and shifts column offsets.
//   - scatter writes the validity prefix from VectorData::validity_.
//   - gather reads the prefix back into the output Vector's validity.
//   - fullScanColumn propagates the prefix.
//   - equal() (NULL-excludes) fails to match a NULL key.
//   - notDistinctFrom() (IS-NOT-DISTINCT-FROM) matches NULL == NULL.
//   - Multi-batch (>5000 rows) scatter/gather round-trip preserves nulls.

class RowOpNullTest : public BumbleBaseTest {
public:
    RowOpNullTest(): rowCollection_(*clientContext.bufferManager_, (idx_t)Storage::BLOCK_SIZE, 1, true) {}

protected:
    RowDataCollection rowCollection_;
    vector<buffer_handle_ptr_t> payloadHandles_;
    vector<data_ptr_t> payloadPtrs_;
    idx_t payloadPageOffset_{0};

    Vector createAddresses(idx_t size, RowLayout &layout) {
        auto tupleSize = layout.getRowWidth();
        auto tuplesPerBlock = Storage::BLOCK_SIZE / tupleSize;
        Vector addresses(LogicalTypeId::ADDRESS, size);
        auto addrPtr = FlatVector::getData<data_ptr_t>(addresses);
        for (idx_t i = 0; i < size; i++) {
            if (payloadHandles_.empty() || payloadPageOffset_ == tuplesPerBlock) {
                payloadHandles_.push_back(clientContext.bufferManager_->allocate(Storage::BLOCK_SIZE));
                payloadPtrs_.push_back(payloadHandles_.back()->ptr());
                payloadPageOffset_ = 0;
            }
            addrPtr[i] = payloadPtrs_.back() + (payloadPageOffset_ * tupleSize);
            ++payloadPageOffset_;
        }
        return addresses;
    }
};

TEST_F(RowOpNullTest, FlagWidthMatchesColumnCount) {
    RowLayout layout1;
    layout1.initialize(vector<LogicalType>{PhysicalType::INTEGER});
    EXPECT_EQ(layout1.getFlagWidth(), 1u);
    EXPECT_EQ(layout1.getOffsets()[0], 1u);

    RowLayout layout3;
    layout3.initialize(vector<LogicalType>{PhysicalType::INTEGER, PhysicalType::BIGINT, PhysicalType::INTEGER});
    EXPECT_EQ(layout3.getFlagWidth(), 1u);
    EXPECT_EQ(layout3.getOffsets()[0], 1u);

    RowLayout layout9;
    vector<LogicalType> wide;
    for (idx_t i = 0; i < 9; i++) wide.emplace_back(PhysicalType::INTEGER);
    layout9.initialize(wide);
    EXPECT_EQ(layout9.getFlagWidth(), 2u);
    EXPECT_EQ(layout9.getOffsets()[0], 2u);

    RowLayout layoutEmpty;
    layoutEmpty.initialize(vector<LogicalType>{});
    EXPECT_EQ(layoutEmpty.getFlagWidth(), 0u);
}

// Non-constant layout (has STRING): the validity prefix lives BEFORE the heap pointer
// (Option A — validity at offset 0). Heap pointer slot shifts to flagWidth_; column
// offsets shift to flagWidth_ + sizeof(idx_t) + cumulative-data-width.
TEST_F(RowOpNullTest, FlagWidthShiftsHeapPointerInNonConstantLayout) {
    RowLayout layout;
    layout.initialize(vector<LogicalType>{PhysicalType::STRING, PhysicalType::INTEGER});
    EXPECT_EQ(layout.getFlagWidth(), 1u);
    // Heap pointer immediately after the 1-byte validity prefix.
    EXPECT_EQ(layout.getHeapPointerOffset(), 1u);
    EXPECT_FALSE(layout.allConstant());
    // STRING is 16 bytes (string_t inline header); INTEGER is 4. Offsets land after
    // [validity (1)] + [heap ptr (8)] = 9, then advance per column.
    EXPECT_EQ(layout.getOffsets()[0], 9u);          // STRING
    EXPECT_EQ(layout.getOffsets()[1], 9u + sizeof(string_t));  // INTEGER
}

// Aggregate-only layout: no data columns → flagWidth_ = 0 (no prefix), and aggregate
// offsets start at row offset 0 (no validity bytes wasted).
TEST_F(RowOpNullTest, AggregateOnlyLayoutHasZeroFlagWidth) {
    RowLayout layout;
    layout.initialize(vector<LogicalType>{});
    EXPECT_EQ(layout.getFlagWidth(), 0u);
    EXPECT_EQ(layout.columnCount(), 0u);
    EXPECT_EQ(layout.getAggrOffset(), 0u);  // aggregates start at row offset 0
}

TEST_F(RowOpNullTest, ScatterAndGatherRoundTripWithNulls) {
    vector<LogicalType> types{PhysicalType::INTEGER, PhysicalType::INTEGER};
    DataChunk chunk;
    chunk.initialize(types);
    chunk.setCapacity(5);
    chunk.resize(5);
    for (idx_t i = 0; i < 5; i++) {
        chunk.setValue(0, i, Value((int32_t)(i + 1)));
        // col 1: NULL at rows 1 and 3
        if (i == 1 || i == 3) chunk.setValue(1, i, Value::null());
        else chunk.setValue(1, i, Value((int32_t)(i * 10)));
    }
    chunk.setCardinality(5);

    RowLayout layout;
    layout.initialize(types);
    Vector addresses = createAddresses(chunk.getSize(), layout);

    SelectionVector sel = FlatVector::INCREMENTAL_SELECTION_VECTOR;
    RowOperations::scatter(chunk, chunk.orrify().get(), layout, addresses, rowCollection_, sel, chunk.getSize());

    // Gather col 1 back; the two NULL rows should reappear as NULL in the validity mask.
    Vector out(types[1], chunk.getSize());
    RowOperations::gather(addresses, sel, out, sel, chunk.getSize(), layout.getOffsets()[1], 1);

    for (idx_t i = 0; i < 5; i++) {
        if (i == 1 || i == 3) EXPECT_FALSE(out.rowIsValid(i)) << "row " << i << " should be NULL";
        else EXPECT_TRUE(out.rowIsValid(i)) << "row " << i << " should be valid";
    }
}

TEST_F(RowOpNullTest, FullScanColumnPropagatesValidity) {
    vector<LogicalType> types{PhysicalType::INTEGER};
    DataChunk chunk;
    chunk.initialize(types);
    chunk.setCapacity(4);
    chunk.resize(4);
    chunk.setValue(0, 0, Value((int32_t)10));
    chunk.setValue(0, 1, Value::null());
    chunk.setValue(0, 2, Value((int32_t)30));
    chunk.setValue(0, 3, Value::null());
    chunk.setCardinality(4);

    RowLayout layout;
    layout.initialize(types);
    Vector addresses = createAddresses(chunk.getSize(), layout);
    SelectionVector sel = FlatVector::INCREMENTAL_SELECTION_VECTOR;
    RowOperations::scatter(chunk, chunk.orrify().get(), layout, addresses, rowCollection_, sel, chunk.getSize());

    Vector out(types[0], chunk.getSize());
    RowOperations::fullScanColumn(layout, addresses, out, chunk.getSize(), 0);

    EXPECT_TRUE(out.rowIsValid(0));
    EXPECT_FALSE(out.rowIsValid(1));
    EXPECT_TRUE(out.rowIsValid(2));
    EXPECT_FALSE(out.rowIsValid(3));
}

TEST_F(RowOpNullTest, EqualFailsOnNullKey) {
    // The NULL-excludes comparator must drop matches when either side is NULL.
    vector<LogicalType> types{PhysicalType::INTEGER};
    DataChunk chunk;
    chunk.initialize(types);
    chunk.setCapacity(3);
    chunk.resize(3);
    chunk.setValue(0, 0, Value((int32_t)5));
    chunk.setValue(0, 1, Value::null());
    chunk.setValue(0, 2, Value((int32_t)7));
    chunk.setCardinality(3);

    RowLayout layout;
    layout.initialize(types);
    Vector addresses = createAddresses(chunk.getSize(), layout);
    SelectionVector sel = FlatVector::INCREMENTAL_SELECTION_VECTOR;
    RowOperations::scatter(chunk, chunk.orrify().get(), layout, addresses, rowCollection_, sel, chunk.getSize());

    // Compare the chunk against itself row-by-row through `equal`.
    SelectionVector matchSel(chunk.getSize());
    for (idx_t i = 0; i < chunk.getSize(); i++) matchSel.setIndex(i, i);
    SelectionVector noMatch(chunk.getSize());
    idx_t noMatchCount = 0;
    idx_t count = chunk.getSize();
    RowOperations::equal(chunk, chunk.orrify().get(), layout, addresses, matchSel, count, &noMatch, noMatchCount);

    // Row 1 has a NULL key on both sides; NULL-excludes rejects the match.
    EXPECT_EQ(noMatchCount, 1u);
    EXPECT_EQ(noMatch.getIndex(0), 1u);
}

TEST_F(RowOpNullTest, NotDistinctFromMatchesBothNull) {
    // IS-NOT-DISTINCT-FROM: two NULLs in the same column are equal.
    vector<LogicalType> types{PhysicalType::INTEGER};
    DataChunk chunk;
    chunk.initialize(types);
    chunk.setCapacity(3);
    chunk.resize(3);
    chunk.setValue(0, 0, Value((int32_t)5));
    chunk.setValue(0, 1, Value::null());
    chunk.setValue(0, 2, Value((int32_t)7));
    chunk.setCardinality(3);

    RowLayout layout;
    layout.initialize(types);
    Vector addresses = createAddresses(chunk.getSize(), layout);
    SelectionVector sel = FlatVector::INCREMENTAL_SELECTION_VECTOR;
    RowOperations::scatter(chunk, chunk.orrify().get(), layout, addresses, rowCollection_, sel, chunk.getSize());

    SelectionVector matchSel(chunk.getSize());
    for (idx_t i = 0; i < chunk.getSize(); i++) matchSel.setIndex(i, i);
    SelectionVector noMatch(chunk.getSize());
    idx_t noMatchCount = 0;
    idx_t count = chunk.getSize();
    RowOperations::notDistinctFrom(chunk, chunk.orrify().get(), layout, addresses, matchSel, count, &noMatch, noMatchCount);

    // All three rows match: NULL==NULL is true under IS-NOT-DISTINCT-FROM.
    EXPECT_EQ(noMatchCount, 0u);
}

TEST_F(RowOpNullTest, EqualNullOnRowSideOnly) {
    // Row in storage has NULL key; probe-side has a non-NULL value at the matching row.
    // Expected: NULL-excludes drops the match.
    vector<LogicalType> types{PhysicalType::INTEGER};
    DataChunk stored;
    stored.initialize(types);
    stored.setCapacity(2);
    stored.resize(2);
    stored.setValue(0, 0, Value((int32_t)5));
    stored.setValue(0, 1, Value::null());
    stored.setCardinality(2);

    RowLayout layout;
    layout.initialize(types);
    Vector addresses = createAddresses(stored.getSize(), layout);
    SelectionVector sel = FlatVector::INCREMENTAL_SELECTION_VECTOR;
    RowOperations::scatter(stored, stored.orrify().get(), layout, addresses, rowCollection_, sel, stored.getSize());

    // Probe: a chunk where row 1 has a real value (not NULL) — should still fail to match the NULL row.
    DataChunk probe;
    probe.initialize(types);
    probe.setCapacity(2);
    probe.resize(2);
    probe.setValue(0, 0, Value((int32_t)5));
    probe.setValue(0, 1, Value((int32_t)0));
    probe.setCardinality(2);

    SelectionVector matchSel(probe.getSize());
    for (idx_t i = 0; i < probe.getSize(); i++) matchSel.setIndex(i, i);
    SelectionVector noMatch(probe.getSize());
    idx_t noMatchCount = 0;
    idx_t count = probe.getSize();
    RowOperations::equal(probe, probe.orrify().get(), layout, addresses, matchSel, count, &noMatch, noMatchCount);
    EXPECT_EQ(noMatchCount, 1u);
    EXPECT_EQ(noMatch.getIndex(0), 1u);
}

TEST_F(RowOpNullTest, MultiBatchScatterGatherPreservesNulls) {
    // Stress-test with >5000 rows spanning multiple STANDARD_VECTOR_SIZE batches; every
    // 100th row carries a NULL in col 1.
    const idx_t N = 5300;
    vector<LogicalType> types{PhysicalType::INTEGER, PhysicalType::INTEGER};
    DataChunk chunk;
    chunk.initialize(types);
    chunk.setCapacity(N);
    chunk.resize(N);
    for (idx_t i = 0; i < N; i++) {
        chunk.setValue(0, i, Value((int32_t)i));
        if (i % 100 == 0) chunk.setValue(1, i, Value::null());
        else chunk.setValue(1, i, Value((int32_t)(i * 2)));
    }
    chunk.setCardinality(N);

    RowLayout layout;
    layout.initialize(types);
    Vector addresses = createAddresses(N, layout);
    SelectionVector sel = FlatVector::INCREMENTAL_SELECTION_VECTOR;
    RowOperations::scatter(chunk, chunk.orrify().get(), layout, addresses, rowCollection_, sel, N);

    Vector out(types[1], N);
    RowOperations::gather(addresses, sel, out, sel, N, layout.getOffsets()[1], 1);

    idx_t nullCount = 0;
    for (idx_t i = 0; i < N; i++) {
        if (!out.rowIsValid(i)) {
            nullCount++;
            EXPECT_EQ(i % 100, 0u) << "unexpected NULL at row " << i;
        }
    }
    EXPECT_EQ(nullCount, (N + 99) / 100);
}

// Variable-length scatter path: STRING column with NULLs. Exercises the heap-pointer
// branch in RowOperations::scatter (the `!layout.allConstant()` block) — Option A
// places validity at offset 0 and pushes the heap pointer to flagWidth_, so this
// confirms that shift is wired correctly.
TEST_F(RowOpNullTest, ScatterGatherStringWithNulls) {
    vector<LogicalType> types{PhysicalType::STRING};
    DataChunk chunk;
    chunk.initialize(types);
    chunk.setCapacity(4);
    chunk.resize(4);
    chunk.setValue(0, 0, Value(string{"alpha"}));
    chunk.setValue(0, 1, Value::null());
    chunk.setValue(0, 2, Value(string{"gamma_long_enough_to_not_inline"}));
    chunk.setValue(0, 3, Value::null());
    chunk.setCardinality(4);

    RowLayout layout;
    layout.initialize(types);
    Vector addresses = createAddresses(chunk.getSize(), layout);
    SelectionVector sel = FlatVector::INCREMENTAL_SELECTION_VECTOR;
    RowOperations::scatter(chunk, chunk.orrify().get(), layout, addresses, rowCollection_, sel, chunk.getSize());

    Vector out(types[0], chunk.getSize());
    RowOperations::gather(addresses, sel, out, sel, chunk.getSize(), layout.getOffsets()[0], 0);

    EXPECT_TRUE(out.rowIsValid(0));
    EXPECT_FALSE(out.rowIsValid(1));
    EXPECT_TRUE(out.rowIsValid(2));
    EXPECT_FALSE(out.rowIsValid(3));
    EXPECT_EQ(out.getValue(0).toString(), "alpha");
    EXPECT_EQ(out.getValue(2).toString(), "gamma_long_enough_to_not_inline");
}

// Mixed STRING + INT (non-constant layout): NULLs in both columns at different
// rows. Confirms (a) the heap-pointer offset is correctly shifted past the
// validity prefix, (b) per-column bits map independently, (c) gather of both
// columns reads its own prefix bit.
TEST_F(RowOpNullTest, ScatterGatherMixedStringIntWithNulls) {
    vector<LogicalType> types{PhysicalType::STRING, PhysicalType::INTEGER};
    DataChunk chunk;
    chunk.initialize(types);
    chunk.setCapacity(4);
    chunk.resize(4);
    chunk.setValue(0, 0, Value(string{"row0"}));
    chunk.setValue(1, 0, Value((int32_t)100));
    chunk.setValue(0, 1, Value::null());          // STRING NULL
    chunk.setValue(1, 1, Value((int32_t)200));
    chunk.setValue(0, 2, Value(string{"row2"}));
    chunk.setValue(1, 2, Value::null());          // INT NULL
    chunk.setValue(0, 3, Value::null());          // BOTH NULL
    chunk.setValue(1, 3, Value::null());
    chunk.setCardinality(4);

    RowLayout layout;
    layout.initialize(types);
    Vector addresses = createAddresses(chunk.getSize(), layout);
    SelectionVector sel = FlatVector::INCREMENTAL_SELECTION_VECTOR;
    RowOperations::scatter(chunk, chunk.orrify().get(), layout, addresses, rowCollection_, sel, chunk.getSize());

    Vector strOut(types[0], chunk.getSize());
    Vector intOut(types[1], chunk.getSize());
    RowOperations::gather(addresses, sel, strOut, sel, chunk.getSize(), layout.getOffsets()[0], 0);
    RowOperations::gather(addresses, sel, intOut, sel, chunk.getSize(), layout.getOffsets()[1], 1);

    EXPECT_TRUE(strOut.rowIsValid(0));
    EXPECT_FALSE(strOut.rowIsValid(1));
    EXPECT_TRUE(strOut.rowIsValid(2));
    EXPECT_FALSE(strOut.rowIsValid(3));

    EXPECT_TRUE(intOut.rowIsValid(0));
    EXPECT_TRUE(intOut.rowIsValid(1));
    EXPECT_FALSE(intOut.rowIsValid(2));
    EXPECT_FALSE(intOut.rowIsValid(3));

    EXPECT_EQ(strOut.getValue(0).toString(), "row0");
    EXPECT_EQ(strOut.getValue(2).toString(), "row2");
}

// All columns NULL in a single row → every bit in the prefix byte is cleared
// (0xFF → 0x00 for a single byte covering 3 cols). Gather restores all NULLs.
TEST_F(RowOpNullTest, ScatterAllNullRowClearsEveryPrefixBit) {
    vector<LogicalType> types{PhysicalType::INTEGER, PhysicalType::BIGINT, PhysicalType::INTEGER};
    DataChunk chunk;
    chunk.initialize(types);
    chunk.setCapacity(2);
    chunk.resize(2);
    // Row 0: all cols valid. Row 1: all cols NULL.
    chunk.setValue(0, 0, Value((int32_t)1));
    chunk.setValue(1, 0, Value((int64_t)10));
    chunk.setValue(2, 0, Value((int32_t)100));
    chunk.setValue(0, 1, Value::null());
    chunk.setValue(1, 1, Value::null());
    chunk.setValue(2, 1, Value::null());
    chunk.setCardinality(2);

    RowLayout layout;
    layout.initialize(types);
    Vector addresses = createAddresses(chunk.getSize(), layout);
    SelectionVector sel = FlatVector::INCREMENTAL_SELECTION_VECTOR;
    RowOperations::scatter(chunk, chunk.orrify().get(), layout, addresses, rowCollection_, sel, chunk.getSize());

    for (idx_t c = 0; c < types.size(); c++) {
        Vector out(types[c], chunk.getSize());
        RowOperations::gather(addresses, sel, out, sel, chunk.getSize(), layout.getOffsets()[c], c);
        EXPECT_TRUE(out.rowIsValid(0)) << "col " << c << " row 0 should be valid";
        EXPECT_FALSE(out.rowIsValid(1)) << "col " << c << " row 1 should be NULL";
    }
}

// Mirror of EqualNullOnRowSideOnly: the row in storage is valid, but the probe-side
// column carries a NULL → NULL-excludes drops the match. Pins the col-side branch
// of the per-row null check in RowMatch.cpp's matchOrNull helper.
TEST_F(RowOpNullTest, EqualNullOnColSideOnly) {
    vector<LogicalType> types{PhysicalType::INTEGER};
    // Stored rows: both valid.
    DataChunk stored;
    stored.initialize(types);
    stored.setCapacity(2);
    stored.resize(2);
    stored.setValue(0, 0, Value((int32_t)5));
    stored.setValue(0, 1, Value((int32_t)7));
    stored.setCardinality(2);

    RowLayout layout;
    layout.initialize(types);
    Vector addresses = createAddresses(stored.getSize(), layout);
    SelectionVector sel = FlatVector::INCREMENTAL_SELECTION_VECTOR;
    RowOperations::scatter(stored, stored.orrify().get(), layout, addresses, rowCollection_, sel, stored.getSize());

    // Probe: row 0 valid (=5), row 1 NULL — should not match the stored row 1 (=7).
    DataChunk probe;
    probe.initialize(types);
    probe.setCapacity(2);
    probe.resize(2);
    probe.setValue(0, 0, Value((int32_t)5));
    probe.setValue(0, 1, Value::null());
    probe.setCardinality(2);

    SelectionVector matchSel(probe.getSize());
    for (idx_t i = 0; i < probe.getSize(); i++) matchSel.setIndex(i, i);
    SelectionVector noMatch(probe.getSize());
    idx_t noMatchCount = 0;
    idx_t count = probe.getSize();
    RowOperations::equal(probe, probe.orrify().get(), layout, addresses, matchSel, count, &noMatch, noMatchCount);

    EXPECT_EQ(noMatchCount, 1u);
    EXPECT_EQ(noMatch.getIndex(0), 1u);
}

// Drives the templatedMatchTypeSelection path (HAS_ROW_SEL=true overload) — the
// other equal() overload that takes both chunkSel and rowSel. NULL on either side
// at the same selected index must still drop the match.
TEST_F(RowOpNullTest, EqualWithRowSelTemplateVariantHonorsNull) {
    vector<LogicalType> types{PhysicalType::INTEGER};
    DataChunk stored;
    stored.initialize(types);
    stored.setCapacity(4);
    stored.resize(4);
    for (idx_t i = 0; i < 4; i++) stored.setValue(0, i, Value((int32_t)(i * 10)));
    stored.setValue(0, 2, Value::null());
    stored.setCardinality(4);

    RowLayout layout;
    layout.initialize(types);
    Vector addresses = createAddresses(stored.getSize(), layout);
    SelectionVector incSel = FlatVector::INCREMENTAL_SELECTION_VECTOR;
    RowOperations::scatter(stored, stored.orrify().get(), layout, addresses, rowCollection_, incSel, stored.getSize());

    DataChunk probe;
    probe.initialize(types);
    probe.setCapacity(4);
    probe.resize(4);
    for (idx_t i = 0; i < 4; i++) probe.setValue(0, i, Value((int32_t)(i * 10)));
    probe.setCardinality(4);

    // Selection: probe row k against stored row k for k in {0,1,2,3}.
    SelectionVector chunkSel(4), rowSel(4);
    for (idx_t i = 0; i < 4; i++) {
        chunkSel.setIndex(i, i);
        rowSel.setIndex(i, i);
    }
    SelectionVector noMatch(4);
    idx_t noMatchCount = 0;
    idx_t count = 4;
    RowOperations::equal(probe, probe.orrify().get(), layout, addresses, chunkSel, rowSel, count, &noMatch, noMatchCount);

    // Stored row 2 is NULL → match excluded. Rows 0,1,3 match.
    EXPECT_EQ(noMatchCount, 1u);
}

// Mixed-type equality: probe column is UINTEGER, row column is BIGINT. The
// templatedMatchOpSwitchCommon path collapses both to int64_t for comparison.
// NULL-excludes must work through that mixed-type dispatch too.
TEST_F(RowOpNullTest, EqualMixedTypeCommonCastExcludesNull) {
    vector<LogicalType> storedTypes{PhysicalType::BIGINT};
    vector<LogicalType> probeTypes{PhysicalType::UINTEGER};

    DataChunk stored;
    stored.initialize(storedTypes);
    stored.setCapacity(3);
    stored.resize(3);
    stored.setValue(0, 0, Value((int64_t)10));
    stored.setValue(0, 1, Value::null());
    stored.setValue(0, 2, Value((int64_t)20));
    stored.setCardinality(3);

    RowLayout layout;
    layout.initialize(storedTypes);
    Vector addresses = createAddresses(stored.getSize(), layout);
    SelectionVector sel = FlatVector::INCREMENTAL_SELECTION_VECTOR;
    RowOperations::scatter(stored, stored.orrify().get(), layout, addresses, rowCollection_, sel, stored.getSize());

    DataChunk probe;
    probe.initialize(probeTypes);
    probe.setCapacity(3);
    probe.resize(3);
    probe.setValue(0, 0, Value((uint32_t)10));
    probe.setValue(0, 1, Value((uint32_t)0));   // would collide with NullValue<int64_t> fill if not excluded
    probe.setValue(0, 2, Value((uint32_t)20));
    probe.setCardinality(3);

    SelectionVector matchSel(3);
    for (idx_t i = 0; i < 3; i++) matchSel.setIndex(i, i);
    SelectionVector noMatch(3);
    idx_t noMatchCount = 0;
    idx_t count = 3;
    RowOperations::equal(probe, probe.orrify().get(), layout, addresses, matchSel, count, &noMatch, noMatchCount);

    // Stored row 1 is NULL → match excluded even though probe row 1 = 0 might
    // otherwise compare equal to the defensive NullValue fill.
    EXPECT_EQ(noMatchCount, 1u);
    EXPECT_EQ(noMatch.getIndex(0), 1u);
}

// STRING key with NULL → match excluded under equal() (NULL-excludes).
TEST_F(RowOpNullTest, EqualStringNullKeyExcluded) {
    vector<LogicalType> types{PhysicalType::STRING};
    DataChunk chunk;
    chunk.initialize(types);
    chunk.setCapacity(3);
    chunk.resize(3);
    chunk.setValue(0, 0, Value(string{"alpha"}));
    chunk.setValue(0, 1, Value::null());
    chunk.setValue(0, 2, Value(string{"gamma"}));
    chunk.setCardinality(3);

    RowLayout layout;
    layout.initialize(types);
    Vector addresses = createAddresses(chunk.getSize(), layout);
    SelectionVector sel = FlatVector::INCREMENTAL_SELECTION_VECTOR;
    RowOperations::scatter(chunk, chunk.orrify().get(), layout, addresses, rowCollection_, sel, chunk.getSize());

    SelectionVector matchSel(chunk.getSize());
    for (idx_t i = 0; i < chunk.getSize(); i++) matchSel.setIndex(i, i);
    SelectionVector noMatch(chunk.getSize());
    idx_t noMatchCount = 0;
    idx_t count = chunk.getSize();
    RowOperations::equal(chunk, chunk.orrify().get(), layout, addresses, matchSel, count, &noMatch, noMatchCount);

    EXPECT_EQ(noMatchCount, 1u);
    EXPECT_EQ(noMatch.getIndex(0), 1u);
}

// Multi-column notDistinctFrom: NULLs in different columns of different rows.
// Both-NULL in the SAME column matches; one-NULL doesn't. Confirms the
// per-column independence of the bit check inside matchOrNull.
TEST_F(RowOpNullTest, NotDistinctFromMultiColMixedNulls) {
    vector<LogicalType> types{PhysicalType::INTEGER, PhysicalType::INTEGER};
    DataChunk chunk;
    chunk.initialize(types);
    chunk.setCapacity(3);
    chunk.resize(3);
    // Row 0: (1, NULL)
    chunk.setValue(0, 0, Value((int32_t)1));
    chunk.setValue(1, 0, Value::null());
    // Row 1: (NULL, 2)
    chunk.setValue(0, 1, Value::null());
    chunk.setValue(1, 1, Value((int32_t)2));
    // Row 2: (1, NULL) — identical to row 0 under IS-NOT-DISTINCT-FROM.
    chunk.setValue(0, 2, Value((int32_t)1));
    chunk.setValue(1, 2, Value::null());
    chunk.setCardinality(3);

    RowLayout layout;
    layout.initialize(types);
    Vector addresses = createAddresses(chunk.getSize(), layout);
    SelectionVector sel = FlatVector::INCREMENTAL_SELECTION_VECTOR;
    RowOperations::scatter(chunk, chunk.orrify().get(), layout, addresses, rowCollection_, sel, chunk.getSize());

    // Self-compare under notDistinctFrom — every row matches itself (both NULL columns
    // match per IS-NOT-DISTINCT-FROM; non-NULL columns match per OP). The per-column
    // independence here means row 0 (1, NULL) is NOT confused with row 1 (NULL, 2):
    // col 0 differs (1 vs NULL → no match) on a self-compare-shifted index.
    SelectionVector matchSel(chunk.getSize());
    for (idx_t i = 0; i < chunk.getSize(); i++) matchSel.setIndex(i, i);
    SelectionVector noMatch(chunk.getSize());
    idx_t noMatchCount = 0;
    idx_t count = chunk.getSize();
    RowOperations::notDistinctFrom(chunk, chunk.orrify().get(), layout, addresses, matchSel, count, &noMatch, noMatchCount);
    EXPECT_EQ(noMatchCount, 0u);

    // Cross-compare: probe row 0 (=(1, NULL)) against stored row 1 (=(NULL, 2)).
    // Col 0: probe 1 vs stored NULL → distinct under IS-NOT-DISTINCT-FROM.
    // So no_match should be 1.
    SelectionVector crossSel(1);
    crossSel.setIndex(0, 0);  // probe chunk index 0
    // Steer the comparison to stored row 1 by populating addresses Vector with the
    // address of stored row 1 at index 0.
    Vector storedRow1(LogicalTypeId::ADDRESS, 1);
    auto ptrs = FlatVector::getData<data_ptr_t>(addresses);
    auto rowPtrs = FlatVector::getData<data_ptr_t>(storedRow1);
    rowPtrs[0] = ptrs[1];
    SelectionVector noMatchCross(1);
    idx_t noMatchCountCross = 0;
    idx_t countCross = 1;
    RowOperations::notDistinctFrom(chunk, chunk.orrify().get(), layout, storedRow1, crossSel, countCross, &noMatchCross, noMatchCountCross);
    EXPECT_EQ(noMatchCountCross, 1u);
}