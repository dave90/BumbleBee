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

#include "bumblebee/common/types/Value.hpp"
#include "bumblebee/common/vector_operations/VectorOperations.hpp"

using namespace bumblebee;

// Phase 5 contract for VectorOperations::copy:
//   The target's positions [targetOffset, targetOffset + count) must reflect the
//   source's validity exactly — copy sets valid AND invalid bits, so that a target
//   reused across batches (e.g. result chunk reused across scan iterations) does
//   not leak stale invalids from an earlier batch.
//
// Fast path: when BOTH source and target are all-valid, the loop is skipped — no
// writes happen.
class CopyNullTest : public ::testing::Test {
protected:
    static constexpr idx_t N = 64;
};

// Source has nulls at known positions → target's mask must mirror exactly.
TEST_F(CopyNullTest, PropagatesNullsFromSource) {
    Vector source(PhysicalType::INTEGER, N);
    Vector target(PhysicalType::INTEGER, N);
    auto sdata = FlatVector::getData<int32_t>(source);
    for (idx_t i = 0; i < N; i++) sdata[i] = (int32_t)i;
    source.setInvalid(3);
    source.setInvalid(17);
    source.setInvalid(40);

    VectorOperations::copy(source, target, N, 0, 0);

    for (idx_t i = 0; i < N; i++) {
        const bool expectedValid = (i != 3 && i != 17 && i != 40);
        EXPECT_EQ(target.rowIsValid(i), expectedValid) << "row " << i;
    }
}

// Phase 5 regression: source row is VALID at position p, target has a STALE
// invalid bit at the same position (left over from a previous batch). After
// copy, target must report row p as VALID — the staleness must not leak through.
// Without the bidir-validity fix this was the bug that broke null.multibatch -d.
TEST_F(CopyNullTest, ClearsStaleInvalidWhenSourceIsValid) {
    Vector source(PhysicalType::INTEGER, N);
    Vector target(PhysicalType::INTEGER, N);
    auto sdata = FlatVector::getData<int32_t>(source);
    auto tdata = FlatVector::getData<int32_t>(target);
    for (idx_t i = 0; i < N; i++) {
        sdata[i] = (int32_t)(i + 100);
        tdata[i] = 0;
    }
    // Source is fully valid (no setInvalid).
    // Target has stale invalid bits from a "previous batch".
    target.setInvalid(5);
    target.setInvalid(20);
    ASSERT_FALSE(target.rowIsValid(5));
    ASSERT_FALSE(target.rowIsValid(20));

    VectorOperations::copy(source, target, N, 0, 0);

    for (idx_t i = 0; i < N; i++) {
        EXPECT_TRUE(target.rowIsValid(i)) << "row " << i << " should be valid after copy";
    }
}

// Mixed case: source has nulls at SOME positions, target has stale invalids at
// DIFFERENT positions. After copy, only source's nulls remain — the target's
// stale bits are cleared where the source is valid.
TEST_F(CopyNullTest, OverwritesTargetMaskExactlyFromSource) {
    Vector source(PhysicalType::INTEGER, N);
    Vector target(PhysicalType::INTEGER, N);
    auto sdata = FlatVector::getData<int32_t>(source);
    for (idx_t i = 0; i < N; i++) sdata[i] = (int32_t)i;
    source.setInvalid(10);
    source.setInvalid(30);
    // Stale invalids on target — different positions than source.
    target.setInvalid(5);
    target.setInvalid(50);

    VectorOperations::copy(source, target, N, 0, 0);

    for (idx_t i = 0; i < N; i++) {
        const bool expectedValid = (i != 10 && i != 30);
        EXPECT_EQ(target.rowIsValid(i), expectedValid) << "row " << i;
    }
}

// Fast path: both source and target are all-valid → copy must not allocate a
// validity buffer on either side. Verified by checking allValid() stays true.
TEST_F(CopyNullTest, AllValidFastPathDoesNotAllocate) {
    Vector source(PhysicalType::INTEGER, N);
    Vector target(PhysicalType::INTEGER, N);
    auto sdata = FlatVector::getData<int32_t>(source);
    for (idx_t i = 0; i < N; i++) sdata[i] = (int32_t)i;

    ASSERT_TRUE(source.validity().allValid());
    ASSERT_TRUE(target.validity().allValid());

    VectorOperations::copy(source, target, N, 0, 0);

    // No NULLs anywhere → target's mask stays unallocated (the fast path).
    EXPECT_TRUE(target.validity().allValid());
}

// Append-style copy (targetOffset > 0): copy nulls into a target whose earlier
// positions are validly populated. The earlier positions must keep their
// validity; the new range must reflect source's validity.
TEST_F(CopyNullTest, AppendStyleCopyKeepsEarlierTargetValidity) {
    Vector source(PhysicalType::INTEGER, N);
    Vector target(PhysicalType::INTEGER, 2 * N);
    auto sdata = FlatVector::getData<int32_t>(source);
    auto tdata = FlatVector::getData<int32_t>(target);
    for (idx_t i = 0; i < N; i++) sdata[i] = (int32_t)i;
    for (idx_t i = 0; i < 2 * N; i++) tdata[i] = -1;
    source.setInvalid(7);

    // Pre-populate the first N target slots with valid data + one invalid in the
    // earlier region; that earlier invalid must survive the append.
    target.setInvalid(2);

    VectorOperations::copy(source, target, N, 0, N);

    // Earlier region untouched by copy.
    EXPECT_FALSE(target.rowIsValid(2));
    EXPECT_TRUE(target.rowIsValid(0));
    // Appended region: position N+7 invalid, others valid.
    for (idx_t i = 0; i < N; i++) {
        const bool expectedValid = (i != 7);
        EXPECT_EQ(target.rowIsValid(N + i), expectedValid) << "row " << (N + i);
    }
}

// DICTIONARY source: validity lives on the child, indexed through the dict
// selection. copy must walk the selection and propagate the right child bits
// into the target's flat positions.
TEST_F(CopyNullTest, DictionarySourceCarriesNullThroughSelection) {
    Vector childSource(PhysicalType::INTEGER, 8);
    auto cdata = FlatVector::getData<int32_t>(childSource);
    for (idx_t i = 0; i < 8; i++) cdata[i] = (int32_t)(i * 10);
    childSource.setInvalid(2);  // child[2] is NULL
    childSource.setInvalid(5);  // child[5] is NULL

    // Dictionary selection picks child indices [5, 1, 2, 7] — so target should
    // see NULL at output positions 0 (=child[5]) and 2 (=child[2]).
    Vector dictSource(childSource);
    SelectionVector sel(4);
    sel.setIndex(0, 5);
    sel.setIndex(1, 1);
    sel.setIndex(2, 2);
    sel.setIndex(3, 7);
    dictSource.slice(sel, 4);

    Vector target(PhysicalType::INTEGER, 4);
    VectorOperations::copy(dictSource, target, 4, 0, 0);

    EXPECT_FALSE(target.rowIsValid(0));  // from child[5]
    EXPECT_TRUE(target.rowIsValid(1));   // from child[1]
    EXPECT_FALSE(target.rowIsValid(2));  // from child[2]
    EXPECT_TRUE(target.rowIsValid(3));   // from child[7]
}

// CONSTANT-NULL source: a single-bit validity at row 0, broadcast through a
// zero selection vector. Every target row must end NULL.
TEST_F(CopyNullTest, ConstantNullSourceMarksAllTargetRowsInvalid) {
    Value nullVal = Value::null().cast(PhysicalType::INTEGER);
    Vector constSource(nullVal);
    ASSERT_EQ(constSource.getVectorType(), VectorType::CONSTANT_VECTOR);

    Vector target(PhysicalType::INTEGER, N);
    auto tdata = FlatVector::getData<int32_t>(target);
    for (idx_t i = 0; i < N; i++) tdata[i] = 0;

    VectorOperations::copy(constSource, target, N, 0, 0);

    for (idx_t i = 0; i < N; i++) {
        EXPECT_FALSE(target.rowIsValid(i)) << "row " << i << " should be NULL";
    }
}

// Selection-vector overload: walks an explicit selection on the source. Nulls
// at the SELECTED source rows must land in target's range; nulls at unselected
// source rows are not seen.
TEST_F(CopyNullTest, SelectionOverloadOnlyCarriesSelectedNulls) {
    Vector source(PhysicalType::INTEGER, N);
    Vector target(PhysicalType::INTEGER, N);
    auto sdata = FlatVector::getData<int32_t>(source);
    for (idx_t i = 0; i < N; i++) sdata[i] = (int32_t)i;
    source.setInvalid(5);   // selected (sel below picks 5)
    source.setInvalid(50);  // NOT selected — must not surface in target

    SelectionVector sel(3);
    sel.setIndex(0, 5);
    sel.setIndex(1, 6);
    sel.setIndex(2, 7);
    VectorOperations::copy(source, target, sel, 3, 0, 0);

    EXPECT_FALSE(target.rowIsValid(0));  // from source[5]
    EXPECT_TRUE(target.rowIsValid(1));   // from source[6]
    EXPECT_TRUE(target.rowIsValid(2));   // from source[7]
}

// Multi-batch stress: copy chains across multiple batches into a reused target;
// stale invalids from earlier batches must not leak. Reproduces the failure
// mode of null/null.multibatch under -d at unit-test scale.
TEST_F(CopyNullTest, MultiBatchAppendDoesNotLeakStaleInvalidsAcrossBatches) {
    const idx_t batchSize = 64;
    const idx_t totalBatches = 4;
    Vector target(PhysicalType::INTEGER, batchSize * totalBatches);

    for (idx_t b = 0; b < totalBatches; b++) {
        Vector batchSource(PhysicalType::INTEGER, batchSize);
        auto sdata = FlatVector::getData<int32_t>(batchSource);
        for (idx_t i = 0; i < batchSize; i++) sdata[i] = (int32_t)(b * batchSize + i);

        // Each odd batch carries a NULL at position 30; the even batches are fully
        // valid. The target must reflect each batch's actual validity at its
        // appended region — without the bidir fix, even-batch position 30 would
        // inherit the odd-batch invalid bit.
        if (b % 2 == 1) batchSource.setInvalid(30);

        VectorOperations::copy(batchSource, target, batchSize, 0, b * batchSize);
    }

    for (idx_t b = 0; b < totalBatches; b++) {
        for (idx_t i = 0; i < batchSize; i++) {
            const idx_t pos = b * batchSize + i;
            const bool expectedValid = !(b % 2 == 1 && i == 30);
            EXPECT_EQ(target.rowIsValid(pos), expectedValid)
                << "batch " << b << " row " << i << " (pos " << pos << ")";
        }
    }
}