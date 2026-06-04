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

#include "bumblebee/common/types/ValidityMask.hpp"
#include "bumblebee/common/types/SelectionVector.hpp"

using namespace bumblebee;

// A freshly constructed mask is all-valid with no backing buffer.
TEST(ValidityMaskTest, FreshMaskIsAllValid) {
    ValidityMask mask;
    EXPECT_TRUE(mask.allValid());
    EXPECT_EQ(mask.data(), nullptr);
    for (idx_t i = 0; i < 128; i++) {
        EXPECT_TRUE(mask.rowIsValid(i));
    }
}

// Setting a bit invalid clears it; rowIsValid reflects the change.
TEST(ValidityMaskTest, SetInvalidClearsBit) {
    ValidityMask mask;
    mask.setInvalid(5);
    EXPECT_FALSE(mask.rowIsValid(5));
    EXPECT_TRUE(mask.rowIsValid(4));
    EXPECT_TRUE(mask.rowIsValid(6));
    // bits across a word boundary
    mask.setInvalid(70);
    EXPECT_FALSE(mask.rowIsValid(70));
    EXPECT_TRUE(mask.rowIsValid(69));
}

// setValid re-enables a previously cleared bit.
TEST(ValidityMaskTest, SetValidRestoresBit) {
    ValidityMask mask;
    mask.setInvalid(9);
    EXPECT_FALSE(mask.rowIsValid(9));
    mask.setValid(9);
    EXPECT_TRUE(mask.rowIsValid(9));
}

// setInvalid lazily allocates an otherwise all-ones buffer.
TEST(ValidityMaskTest, EnsureWritableLazyAllocAllOnes) {
    ValidityMask mask;
    EXPECT_EQ(mask.data(), nullptr);
    mask.ensureWritable();
    ASSERT_NE(mask.data(), nullptr);
    // Buffer must be all-valid right after allocation.
    for (idx_t i = 0; i < STANDARD_VECTOR_SIZE; i++) {
        EXPECT_TRUE(mask.rowIsValid(i));
    }
}

// setAllInvalid / setAllValid toggle the whole mask.
TEST(ValidityMaskTest, SetAllInvalidThenAllValid) {
    ValidityMask mask;
    mask.setAllInvalid(100);
    for (idx_t i = 0; i < 100; i++) {
        EXPECT_FALSE(mask.rowIsValid(i));
    }
    mask.setAllValid();
    EXPECT_TRUE(mask.allValid());
    for (idx_t i = 0; i < 100; i++) {
        EXPECT_TRUE(mask.rowIsValid(i));
    }
}

// checkAllValid is true for a fresh mask, false after a single clear.
TEST(ValidityMaskTest, CheckAllValid) {
    ValidityMask mask;
    EXPECT_TRUE(mask.checkAllValid(STANDARD_VECTOR_SIZE));
    mask.setInvalid(3);
    EXPECT_FALSE(mask.checkAllValid(10));
    // still valid above index 3
    EXPECT_TRUE(mask.rowIsValid(4));
}

// slice copies bits at an offset, preserving relative positions.
TEST(ValidityMaskTest, SlicePreservesBitsAtOffset) {
    ValidityMask src;
    src.setInvalid(5);
    src.setInvalid(7);

    ValidityMask dst;
    dst.slice(src, 4, 8); // copy bits [4,12)
    // src bit 5 -> dst bit 1 ; src bit 7 -> dst bit 3
    EXPECT_TRUE(dst.rowIsValid(0));
    EXPECT_FALSE(dst.rowIsValid(1));
    EXPECT_TRUE(dst.rowIsValid(2));
    EXPECT_FALSE(dst.rowIsValid(3));
    EXPECT_TRUE(dst.rowIsValid(4));
}

// slicing an all-valid mask yields an all-valid mask (no buffer).
TEST(ValidityMaskTest, SliceAllValidCollapses) {
    ValidityMask src;
    ValidityMask dst;
    dst.setInvalid(0); // dirty it first
    dst.slice(src, 2, 8);
    EXPECT_TRUE(dst.allValid());
}

// combine is a logical AND: invalid in either input is invalid in the result.
TEST(ValidityMaskTest, CombineAnds) {
    ValidityMask a;
    a.setInvalid(2);
    ValidityMask b;
    b.setInvalid(5);
    a.combine(b, 16);
    EXPECT_FALSE(a.rowIsValid(2)); // from a
    EXPECT_FALSE(a.rowIsValid(5)); // from b
    EXPECT_TRUE(a.rowIsValid(0));
    EXPECT_TRUE(a.rowIsValid(7));
}

// gatherFrom reads validity through a selection vector.
TEST(ValidityMaskTest, GatherThroughSelection) {
    ValidityMask src;
    src.setInvalid(3);

    SelectionVector sel(4);
    sel.setIndex(0, 0);
    sel.setIndex(1, 3); // null
    sel.setIndex(2, 1);
    sel.setIndex(3, 3); // null

    ValidityMask dst;
    dst.gatherFrom(src, sel, 4);
    EXPECT_TRUE(dst.rowIsValid(0));
    EXPECT_FALSE(dst.rowIsValid(1));
    EXPECT_TRUE(dst.rowIsValid(2));
    EXPECT_FALSE(dst.rowIsValid(3));
}

// Copy constructor shares the underlying buffer (shallow) — referenced vectors
// share their validity, matching the data-sharing model.
TEST(ValidityMaskTest, CopyIsShallowShare) {
    ValidityMask a;
    a.setInvalid(4);
    ValidityMask b(a);
    EXPECT_FALSE(b.rowIsValid(4));
    b.setValid(4);
    EXPECT_TRUE(a.rowIsValid(4)); // shared buffer: change is visible to both
}

// copy() produces an independent deep copy.
TEST(ValidityMaskTest, ExplicitCopyIsDeep) {
    ValidityMask a;
    a.setInvalid(4);
    ValidityMask b = a.copy();
    EXPECT_FALSE(b.rowIsValid(4));
    b.setValid(4);
    EXPECT_FALSE(a.rowIsValid(4)); // original untouched
    EXPECT_TRUE(b.rowIsValid(4));
}

// copy() of an all-valid mask stays all-valid (no buffer).
TEST(ValidityMaskTest, CopyOfAllValidStaysAllValid) {
    ValidityMask a;
    ValidityMask b = a.copy();
    EXPECT_TRUE(b.allValid());
}

// Growing the buffer past its current size preserves already-written bits.
TEST(ValidityMaskTest, GrowPreservesBits) {
    ValidityMask m;
    m.setInvalid(5);                 // first alloc: floored to STANDARD_VECTOR_SIZE
    m.setInvalid(STANDARD_VECTOR_SIZE + 100); // forces a grow + preserve
    EXPECT_FALSE(m.rowIsValid(5));               // low bit preserved across grow
    EXPECT_FALSE(m.rowIsValid(STANDARD_VECTOR_SIZE + 100));
    EXPECT_TRUE(m.rowIsValid(6));
    EXPECT_TRUE(m.rowIsValid(STANDARD_VECTOR_SIZE + 50));
}

// combine with an all-valid other is a no-op (early return).
TEST(ValidityMaskTest, CombineWithAllValidIsNoop) {
    ValidityMask a;
    a.setInvalid(3);
    ValidityMask allValid;
    a.combine(allValid, 16);
    EXPECT_FALSE(a.rowIsValid(3)); // unchanged
    EXPECT_TRUE(a.rowIsValid(0));
}

// gatherFrom an all-valid source leaves the destination all-valid.
TEST(ValidityMaskTest, GatherFromAllValidStaysValid) {
    ValidityMask src;
    SelectionVector sel(4);
    for (idx_t i = 0; i < 4; i++) sel.setIndex(i, i);
    ValidityMask dst;
    dst.setInvalid(0); // dirty first
    dst.gatherFrom(src, sel, 4);
    EXPECT_TRUE(dst.allValid());
}

// slice with offset past the source's covered range yields an all-valid result.
TEST(ValidityMaskTest, SliceOffsetPastSourceIsAllValid) {
    ValidityMask src;
    src.setInvalid(2); // src now covers STANDARD_VECTOR_SIZE bits
    ValidityMask dst;
    dst.slice(src, STANDARD_VECTOR_SIZE + 10, 8); // offset beyond source bits
    for (idx_t i = 0; i < 8; i++) EXPECT_TRUE(dst.rowIsValid(i));
}

// debugConsistent: all-valid (no buffer) is consistent; after setInvalid the buffer
// is owned and large enough for the addressed row, including after a grow.
TEST(ValidityMaskTest, DebugConsistentInvariant) {
    ValidityMask m;
    EXPECT_TRUE(m.debugConsistent(STANDARD_VECTOR_SIZE)); // no buffer => consistent
    m.setInvalid(5);
    EXPECT_TRUE(m.debugConsistent(STANDARD_VECTOR_SIZE));
    m.setInvalid(5000); // grows the buffer past one word
    EXPECT_TRUE(m.debugConsistent(5001));
}