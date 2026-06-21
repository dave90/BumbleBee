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

#include "bumblebee/common/vector_operations/VectorOperations.hpp"

using namespace bumblebee;

class VectorNullTest : public NullTestBase {};

// ---------- FLAT: set / get / toString ----------

TEST_F(VectorNullTest, FlatSetGetNull) {
    Vector v = createVectorWithNulls(PhysicalType::INTEGER, 8, {2, 5});
    EXPECT_TRUE(isNull(v, 2));
    EXPECT_TRUE(isNull(v, 5));
    EXPECT_FALSE(isNull(v, 0));
    EXPECT_TRUE(v.getValue(2).isNull());
    EXPECT_FALSE(v.getValue(0).isNull());
    EXPECT_EQ(v.getValue(2).toString(), "NULL");
}

TEST_F(VectorNullTest, SetNullThenOverwriteNonNull) {
    Vector v(PhysicalType::INTEGER, 4);
    v.setValue(1, Value::null());
    EXPECT_TRUE(isNull(v, 1));
    v.setValue(1, Value(99));
    EXPECT_FALSE(isNull(v, 1));
    EXPECT_EQ(v.getValue(1), Value(99));
}

TEST_F(VectorNullTest, ToStringShowsNull) {
    Vector v = createVectorWithNulls(PhysicalType::INTEGER, 3, {1});
    auto s = v.toString(3);
    EXPECT_NE(s.find("NULL"), std::string::npos);
}

// ---------- reference ----------

TEST_F(VectorNullTest, ReferenceSharesNull) {
    Vector v = createVectorWithNulls(PhysicalType::INTEGER, 6, {3});
    Vector ref(v); // copy ctor -> reference
    EXPECT_TRUE(isNull(ref, 3));
    EXPECT_FALSE(isNull(ref, 2));
}

// ---------- slice(offset) ----------

TEST_F(VectorNullTest, SliceOffsetShiftsNull) {
    Vector v = createVectorWithNulls(PhysicalType::INTEGER, 8, {3});
    Vector sliced(v, 2); // offset 2: row 3 -> row 1
    EXPECT_TRUE(isNull(sliced, 1));
    EXPECT_FALSE(isNull(sliced, 0));
    EXPECT_TRUE(sliced.getValue(1).isNull());
    // original untouched
    EXPECT_TRUE(isNull(v, 3));
}

// ---------- slice(sel) -> DICTIONARY ----------

TEST_F(VectorNullTest, SliceSelectionReadsNullThroughChild) {
    Vector v = createVectorWithNulls(PhysicalType::INTEGER, 6, {1, 4});
    SelectionVector sel(3);
    sel.setIndex(0, 1); // null
    sel.setIndex(1, 2);
    sel.setIndex(2, 4); // null
    Vector dict(v, sel, 3);
    ASSERT_EQ(dict.getVectorType(), VectorType::DICTIONARY_VECTOR);
    EXPECT_TRUE(isNull(dict, 0));
    EXPECT_FALSE(isNull(dict, 1));
    EXPECT_TRUE(isNull(dict, 2));
    EXPECT_TRUE(dict.getValue(0).isNull());
}

// ---------- normalify ----------

TEST_F(VectorNullTest, NormalifyDictionaryPreservesNull) {
    Vector v = createVectorWithNulls(PhysicalType::INTEGER, 6, {0, 3});
    SelectionVector sel(3);
    sel.setIndex(0, 3); // null
    sel.setIndex(1, 1);
    sel.setIndex(2, 0); // null
    Vector dict(v, sel, 3);
    dict.normalify(3);
    ASSERT_EQ(dict.getVectorType(), VectorType::FLAT_VECTOR);
    EXPECT_TRUE(isNull(dict, 0));
    EXPECT_FALSE(isNull(dict, 1));
    EXPECT_TRUE(isNull(dict, 2));
}

TEST_F(VectorNullTest, NormalifyNullConstant) {
    Vector v(Value::null());
    v.setVectorType(VectorType::CONSTANT_VECTOR);
    // Value::null() default-types to INTEGER; constant carries one validity bit.
    EXPECT_TRUE(ConstantVector::isNull(v));
    v.normalify(5);
    ASSERT_EQ(v.getVectorType(), VectorType::FLAT_VECTOR);
    for (idx_t i = 0; i < 5; i++) {
        EXPECT_TRUE(isNull(v, i));
    }
}

TEST_F(VectorNullTest, NormalifyNonNullConstantStaysValid) {
    Vector v(Value(7));
    v.normalify(5);
    for (idx_t i = 0; i < 5; i++) {
        EXPECT_FALSE(isNull(v, i));
        EXPECT_EQ(v.getValue(i), Value(7));
    }
}

// ---------- orrify ----------

TEST_F(VectorNullTest, OrrifyFlatCarriesValidity) {
    Vector v = createVectorWithNulls(PhysicalType::INTEGER, 5, {2});
    VectorData vdata;
    v.orrify(5, vdata);
    ASSERT_NE(vdata.validity_, nullptr);
    EXPECT_FALSE(vdata.validity_->rowIsValid(vdata.sel_->getIndex(2)));
    EXPECT_TRUE(vdata.validity_->rowIsValid(vdata.sel_->getIndex(0)));
}

TEST_F(VectorNullTest, OrrifyDictionaryCarriesValidity) {
    Vector v = createVectorWithNulls(PhysicalType::INTEGER, 6, {4});
    SelectionVector sel(3);
    sel.setIndex(0, 0);
    sel.setIndex(1, 4); // null
    sel.setIndex(2, 2);
    Vector dict(v, sel, 3);
    VectorData vdata;
    dict.orrify(3, vdata);
    ASSERT_NE(vdata.validity_, nullptr);
    EXPECT_TRUE(vdata.validity_->rowIsValid(vdata.sel_->getIndex(0)));
    EXPECT_FALSE(vdata.validity_->rowIsValid(vdata.sel_->getIndex(1)));
    EXPECT_TRUE(vdata.validity_->rowIsValid(vdata.sel_->getIndex(2)));
}

// ---------- SEQUENCE / SEQUENCE_CIRCULAR: always valid ----------

TEST_F(VectorNullTest, SequenceAlwaysValid) {
    Vector v(PhysicalType::BIGINT);
    v.sequence(5, 2);
    for (idx_t i = 0; i < 10; i++) EXPECT_FALSE(isNull(v, i));
}

TEST_F(VectorNullTest, CircularSequenceAlwaysValid) {
    Vector v(PhysicalType::BIGINT);
    v.sequence(5, 0, 1, 10);
    for (idx_t i = 0; i < 10; i++) EXPECT_FALSE(isNull(v, i));
}

// ---------- STRING null (writeNullFill string path) ----------

TEST_F(VectorNullTest, StringNull) {
    Vector v(PhysicalType::STRING, 4);
    v.setValue(0, Value("a"));
    v.setValue(1, Value::null());
    v.setValue(2, Value("c"));
    EXPECT_FALSE(isNull(v, 0));
    EXPECT_TRUE(isNull(v, 1));
    EXPECT_TRUE(v.getValue(1).isNull());
    EXPECT_EQ(v.getValue(1).toString(), "NULL");
    EXPECT_EQ(v.getValue(2), Value("c"));
}

// ---------- CONSTANT vector get / setNull ----------

TEST_F(VectorNullTest, ConstantNullGetValue) {
    Vector v(Value::null());
    ASSERT_EQ(v.getVectorType(), VectorType::CONSTANT_VECTOR);
    EXPECT_TRUE(ConstantVector::isNull(v));
    EXPECT_TRUE(v.getValue(0).isNull());
    // a constant null reports null for every logical row
    EXPECT_TRUE(isNull(v, 3));
}

TEST_F(VectorNullTest, ConstantSetNullToggles) {
    Vector v(Value(7));
    EXPECT_FALSE(ConstantVector::isNull(v));
    EXPECT_EQ(v.getValue(0), Value(7));
    ConstantVector::setNull(v, true);
    EXPECT_TRUE(ConstantVector::isNull(v));
    EXPECT_TRUE(v.getValue(0).isNull());
    ConstantVector::setNull(v, false);
    EXPECT_FALSE(ConstantVector::isNull(v));
}

// ---------- setValue(NULL) dispatched through a DICTIONARY to the child ----------

TEST_F(VectorNullTest, SetNullOnDictionaryDispatchesToChild) {
    Vector v = createVectorWithNulls(PhysicalType::INTEGER, 6, {}); // no nulls yet
    SelectionVector sel(3);
    sel.setIndex(0, 4);
    sel.setIndex(1, 1);
    sel.setIndex(2, 2);
    Vector dict(v, sel, 3);
    ASSERT_EQ(dict.getVectorType(), VectorType::DICTIONARY_VECTOR);
    // setValue dispatches through the selection into the dictionary's child mask.
    dict.setValue(0, Value::null()); // dict row 0 -> child row 4
    EXPECT_TRUE(isNull(dict, 0));
    EXPECT_FALSE(isNull(dict, 1));
    EXPECT_TRUE(dict.getValue(0).isNull());
    EXPECT_FALSE(dict.getValue(1).isNull());
    // Note: when the source mask was all-valid (no buffer), the dictionary's child
    // owns an independent mask, so this does NOT retro-mark the original `v` — that
    // is the intended zero-overhead all-valid behavior, not cross-view propagation.
}

// ---------- move ctor preserves nulls ----------

TEST_F(VectorNullTest, MoveCtorPreservesNull) {
    Vector a = createVectorWithNulls(PhysicalType::INTEGER, 5, {2});
    Vector b(std::move(a));
    EXPECT_TRUE(isNull(b, 2));
    EXPECT_FALSE(isNull(b, 0));
}

// ---------- resize grows the mask, preserving nulls ----------

TEST_F(VectorNullTest, ResizeGrowsMaskPreservingNull) {
    Vector v(PhysicalType::INTEGER, 4);
    for (idx_t i = 0; i < 4; i++) v.setValue(i, Value((int32_t)i));
    v.setValue(2, Value::null());
    v.resize(4, 2000); // grows past one mask word block
    v.setValue(1500, Value(123));
    EXPECT_TRUE(isNull(v, 2));      // preserved
    EXPECT_FALSE(isNull(v, 1500));  // newly written row is valid
    EXPECT_EQ(v.getValue(1500), Value(123));
}

// ---------- VectorOperations::copy carries nulls from non-flat sources ----------

TEST_F(VectorNullTest, CopyFromDictionarySourceCarriesNull) {
    Vector src = createVectorWithNulls(PhysicalType::INTEGER, 6, {4});
    SelectionVector sel(3);
    sel.setIndex(0, 0);
    sel.setIndex(1, 4); // null
    sel.setIndex(2, 2);
    Vector dictSrc(src, sel, 3);

    Vector target(PhysicalType::INTEGER, STANDARD_VECTOR_SIZE);
    VectorOperations::copy(dictSrc, target, 3, 0, 0);
    EXPECT_FALSE(isNull(target, 0));
    EXPECT_TRUE(isNull(target, 1));
    EXPECT_FALSE(isNull(target, 2));
}

TEST_F(VectorNullTest, CopyFromConstantNullSourceCarriesNull) {
    Vector constNull(Value::null());
    Vector target(PhysicalType::INTEGER, STANDARD_VECTOR_SIZE);
    VectorOperations::copy(constNull, target, 5, 0, 0);
    for (idx_t i = 0; i < 5; i++) EXPECT_TRUE(isNull(target, i));
}

// ---------- multi-batch (>5000 rows) ----------

TEST_F(VectorNullTest, MultiBatchRandomNulls) {
    const idx_t count = 6000;
    auto positions = randomNullPlacement(count, 0.1, 123);
    Vector v(PhysicalType::BIGINT, count);
    for (idx_t i = 0; i < count; i++) v.setValue(i, Value((int64_t)i));
    for (auto p : positions) v.setValue(p, Value::null());

    std::vector<bool> expectedNull(count, false);
    for (auto p : positions) expectedNull[p] = true;
    for (idx_t i = 0; i < count; i++) {
        EXPECT_EQ(isNull(v, i), expectedNull[i]) << "row " << i;
    }
}