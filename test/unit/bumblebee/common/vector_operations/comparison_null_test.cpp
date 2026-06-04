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
#include "../../NullTestBase.hpp"

#include <gtest/gtest.h>

#include "bumblebee/common/types/Decimal.hpp"
#include "bumblebee/common/vector_operations/VectorOperations.hpp"

using namespace bumblebee;

class ComparisonNullTest : public NullTestBase {};

// Helper: collect trueSel indices into a sorted vector for easy assertion.
static std::vector<idx_t> toVec(const SelectionVector &sel, idx_t n) {
    std::vector<idx_t> out;
    for (idx_t i = 0; i < n; i++) out.push_back(sel.getIndex(i));
    std::sort(out.begin(), out.end());
    return out;
}

// equals(left, right, sel, count, trueSel) — NULL rows must not appear in trueSel.
TEST_F(ComparisonNullTest, EqualsExcludesNullFromTrueSel) {
    // left:  [10, 20, 30, NULL, 50]
    // right: [10, 25, 30, 40,   NULL]
    // expected matches: row 0 (10==10), row 2 (30==30); rows 3 and 4 are NULL on at least one side
    Vector left = createVectorWithNulls(PhysicalType::INTEGER, 5, {3});
    Vector right(PhysicalType::INTEGER, 5);
    right.setValue(0, Value((int32_t)10));
    right.setValue(1, Value((int32_t)25));
    right.setValue(2, Value((int32_t)30));
    right.setValue(3, Value((int32_t)40));
    right.setValue(4, Value::null());
    // overwrite left col 0/2 to match
    left.setValue(0, Value((int32_t)10));
    left.setValue(2, Value((int32_t)30));

    SelectionVector trueSel(STANDARD_VECTOR_SIZE);
    auto trueCount = VectorOperations::equals(left, right, nullptr, 5, &trueSel);
    EXPECT_EQ(trueCount, 2u);
    auto idxs = toVec(trueSel, trueCount);
    EXPECT_EQ(idxs, (std::vector<idx_t>{0, 2}));
}

// The 7-arg variant returns both trueSel and falseSel; UNKNOWN rows go to falseSel
// so the OR-eval loop in PhysicalExpression can hand them to the next OR branch.
TEST_F(ComparisonNullTest, EqualsUnknownGoesToFalseSelNotTrueSel) {
    // a = [1, 2, NULL, 4]; b = [1, 5, 3, NULL]
    Vector a(PhysicalType::INTEGER, 4);
    a.setValue(0, Value(1));
    a.setValue(1, Value(2));
    a.setValue(2, Value::null());
    a.setValue(3, Value(4));
    Vector b(PhysicalType::INTEGER, 4);
    b.setValue(0, Value(1));
    b.setValue(1, Value(5));
    b.setValue(2, Value(3));
    b.setValue(3, Value::null());

    SelectionVector trueSel(STANDARD_VECTOR_SIZE);
    SelectionVector falseSel(STANDARD_VECTOR_SIZE);
    idx_t falseCount = 0;
    auto trueCount = VectorOperations::equals(a, b, nullptr, 4, &trueSel, &falseSel, falseCount);
    // Only row 0 matches; the rest (including the two NULL rows) belong to falseSel
    // so that the OR-eval invariant `trueCount + falseCount == count` holds.
    EXPECT_EQ(trueCount, 1u);
    EXPECT_EQ(falseCount, 3u);
    EXPECT_EQ(trueSel.getIndex(0), 0u);
    auto falseIdxs = toVec(falseSel, falseCount);
    EXPECT_EQ(falseIdxs, (std::vector<idx_t>{1, 2, 3}));
}

// `a = 5 OR b = 10` with a NULL on the left side of A but a TRUE on B: simulate
// the OR-eval pattern that PhysicalExpression uses — iter 1 produces falseSel
// that still contains the NULL row, iter 2 evaluates B on it and matches.
TEST_F(ComparisonNullTest, OrEvalKeepsNullRowAliveForNextBranch) {
    // a column: [NULL, 5,   1]
    // b column: [10,   20,  10]
    Vector a(PhysicalType::INTEGER, 3);
    a.setValue(0, Value::null());
    a.setValue(1, Value(5));
    a.setValue(2, Value(1));
    Vector b(PhysicalType::INTEGER, 3);
    b.setValue(0, Value(10));
    b.setValue(1, Value(20));
    b.setValue(2, Value(10));
    Vector five(Value(5));
    Vector ten(Value(10));

    // iter 1: a == 5
    SelectionVector sel = FlatVector::INCREMENTAL_SELECTION_VECTOR;
    SelectionVector trueSel(STANDARD_VECTOR_SIZE);
    SelectionVector falseSel(STANDARD_VECTOR_SIZE);
    idx_t falseCount = 0;
    idx_t count = 3;
    auto trueCount = VectorOperations::equals(a, five, &sel, count, &trueSel, &falseSel, falseCount);
    // row 1 matches; rows 0 (NULL) and 2 do not -> both in falseSel
    EXPECT_EQ(trueCount, 1u);
    EXPECT_EQ(falseCount, 2u);

    // iter 2: b == 10 on the falseSel rows
    SelectionVector trueSel2(STANDARD_VECTOR_SIZE);
    SelectionVector falseSel2(STANDARD_VECTOR_SIZE);
    idx_t falseCount2 = 0;
    auto trueCount2 = VectorOperations::equals(b, ten, &falseSel, falseCount, &trueSel2, &falseSel2, falseCount2);
    // row 0 (b=10) and row 2 (b=10) both match — the NULL on `a` did not eliminate row 0
    EXPECT_EQ(trueCount2, 2u);
    auto matched = toVec(trueSel2, trueCount2);
    EXPECT_EQ(matched, (std::vector<idx_t>{0, 2}));
}

// Greater-than with NULL: NULL > x is UNKNOWN
TEST_F(ComparisonNullTest, GreaterThanExcludesNull) {
    Vector v = createVectorWithNulls(PhysicalType::INTEGER, 5, {1, 3}); // values 1,_,3,_,5 (NULL at 1 and 3)
    Vector two(Value((int32_t)2));
    SelectionVector trueSel(STANDARD_VECTOR_SIZE);
    auto trueCount = VectorOperations::greaterThan(v, two, nullptr, 5, &trueSel);
    // values: 1 (no), 2 (no - actually val=2 from create, then >2 false), 3 (yes), 4 (NULL), 5 (yes)
    // createVectorWithNulls fills v[i] = i+1, then nulls positions; so v = [1,NULL,3,NULL,5]
    // > 2 yields: false, NULL, true, NULL, true -> trueSel = {2,4}
    EXPECT_EQ(trueCount, 2u);
    auto idxs = toVec(trueSel, trueCount);
    EXPECT_EQ(idxs, (std::vector<idx_t>{2, 4}));
}

// A NULL constant on either side propagates: no row passes.
TEST_F(ComparisonNullTest, NullConstantOperandFiltersEverything) {
    Vector v = createVectorWithNulls(PhysicalType::INTEGER, 4, {});
    Vector nullConst(Value::null());

    SelectionVector trueSel(STANDARD_VECTOR_SIZE);
    SelectionVector falseSel(STANDARD_VECTOR_SIZE);
    idx_t falseCount = 0;
    auto trueCount = VectorOperations::equals(v, nullConst, nullptr, 4, &trueSel, &falseSel, falseCount);
    EXPECT_EQ(trueCount, 0u);
    EXPECT_EQ(falseCount, 4u); // all rows survive into falseSel (UNKNOWN)
}

// Multi-batch (>5000 rows) to exercise the SIMD-friendly fast path with sparse nulls.
TEST_F(ComparisonNullTest, MultiBatchSparseNulls) {
    const idx_t count = 6000;
    auto positions = randomNullPlacement(count, 0.05, 7);
    Vector left(PhysicalType::BIGINT, count);
    Vector right(PhysicalType::BIGINT, count);
    for (idx_t i = 0; i < count; i++) {
        left.setValue(i, Value((int64_t)i));
        right.setValue(i, Value((int64_t)i)); // equal everywhere
    }
    for (auto p : positions) {
        // null on the left only — they'd otherwise compare equal
        left.setValue(p, Value::null());
    }

    SelectionVector trueSel(count + 16);
    auto trueCount = VectorOperations::equals(left, right, nullptr, count, &trueSel);
    EXPECT_EQ(trueCount, count - positions.size());
}

// IS NOT DISTINCT FROM: two NULLs match; a NULL and a non-NULL do not.
TEST_F(ComparisonNullTest, NotDistinctFromTreatsBothNullAsEqual) {
    Vector a(PhysicalType::INTEGER, 4);
    a.setValue(0, Value(1));
    a.setValue(1, Value::null());
    a.setValue(2, Value::null());
    a.setValue(3, Value(7));
    Vector b(PhysicalType::INTEGER, 4);
    b.setValue(0, Value(1));         // equal
    b.setValue(1, Value::null());    // both NULL -> matched under IS NOT DISTINCT FROM
    b.setValue(2, Value(2));         // NULL vs 2 -> distinct
    b.setValue(3, Value(8));         // different non-null values -> distinct

    SelectionVector trueSel(STANDARD_VECTOR_SIZE);
    auto matched = VectorOperations::notDistinctFrom(a, b, nullptr, 4, &trueSel);
    auto idxs = toVec(trueSel, matched);
    EXPECT_EQ(idxs, (std::vector<idx_t>{0, 1}));

    // distinctFrom is the inverse over the same input.
    SelectionVector distinctSel(STANDARD_VECTOR_SIZE);
    auto distinctCount = VectorOperations::distinctFrom(a, b, nullptr, 4, &distinctSel);
    auto distinctIdxs = toVec(distinctSel, distinctCount);
    EXPECT_EQ(distinctIdxs, (std::vector<idx_t>{2, 3}));
}

// IS NULL / IS NOT NULL: pure mask reads, independent of values.
TEST_F(ComparisonNullTest, IsNullAndIsNotNull) {
    Vector v = createVectorWithNulls(PhysicalType::BIGINT, 6, {1, 4});

    SelectionVector trueSel(STANDARD_VECTOR_SIZE);
    auto nullCount = VectorOperations::isNull(v, nullptr, 6, &trueSel);
    auto nullIdxs = toVec(trueSel, nullCount);
    EXPECT_EQ(nullIdxs, (std::vector<idx_t>{1, 4}));

    SelectionVector trueSel2(STANDARD_VECTOR_SIZE);
    auto notNullCount = VectorOperations::isNotNull(v, nullptr, 6, &trueSel2);
    auto notNullIdxs = toVec(trueSel2, notNullCount);
    EXPECT_EQ(notNullIdxs, (std::vector<idx_t>{0, 2, 3, 5}));
}

// Dictionary input: the generic select path orrifies the vector and reads
// validity through the selection. A null behind the selection must still
// be excluded from trueSel.
TEST_F(ComparisonNullTest, EqualsOverDictionaryInput) {
    Vector v = createVectorWithNulls(PhysicalType::INTEGER, 6, {2});
    v.setValue(0, Value(10));
    v.setValue(1, Value(20));
    // v[2] is NULL
    v.setValue(3, Value(30));
    v.setValue(4, Value(40));
    v.setValue(5, Value(50));
    SelectionVector sel(4);
    sel.setIndex(0, 0);  // 10
    sel.setIndex(1, 2);  // NULL
    sel.setIndex(2, 3);  // 30
    sel.setIndex(3, 4);  // 40
    Vector dict(v, sel, 4);
    ASSERT_EQ(dict.getVectorType(), VectorType::DICTIONARY_VECTOR);

    Vector ten(Value(10));
    SelectionVector trueSel(STANDARD_VECTOR_SIZE);
    auto matched = VectorOperations::equals(dict, ten, nullptr, 4, &trueSel);
    EXPECT_EQ(matched, 1u);
    EXPECT_EQ(trueSel.getIndex(0), 0u);
}

// Decimal path: a NULL operand under the cast-and-compare path is still excluded.
TEST_F(ComparisonNullTest, DecimalComparisonHonorsNull) {
    // Build a DECIMAL(10,2) vector with one NULL row, compare against an INTEGER constant.
    LogicalType decType = LogicalType::createDecimal(10, 2);
    Vector dec(decType, 4);
    // setValue casts a numeric Value into the decimal physical storage.
    dec.setValue(0, Value((int64_t)100).cast(decType.getPhysicalType()));   // 1.00
    dec.setValue(1, Value((int64_t)200).cast(decType.getPhysicalType()));   // 2.00
    dec.setValue(2, Value::null());
    dec.setValue(3, Value((int64_t)300).cast(decType.getPhysicalType()));   // 3.00

    Vector two(Value((int32_t)2));
    SelectionVector trueSel(STANDARD_VECTOR_SIZE);
    auto trueCount = VectorOperations::greaterThan(dec, two, nullptr, 4, &trueSel);
    // > 2.00 matches row 3 only (3.00). Row 2 is NULL — must be excluded.
    EXPECT_EQ(trueCount, 1u);
    EXPECT_EQ(trueSel.getIndex(0), 3u);
}