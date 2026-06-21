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
#include <algorithm>
#include <numeric>

#include "bumblebee/common/vector_operations/CreateSortKey.hpp"

using namespace bumblebee;

class SortKeysNullTest : public NullTestBase {
protected:
    // Return the row indices in the order they end up after sorting by their encoded key.
    static std::vector<idx_t> sortedIndices(Vector &keys, idx_t size) {
        auto data = FlatVector::getData<string_t>(keys);
        std::vector<idx_t> idxs(size);
        std::iota(idxs.begin(), idxs.end(), 0);
        std::sort(idxs.begin(), idxs.end(), [&](idx_t a, idx_t b) {
            return data[a].getString() < data[b].getString();
        });
        return idxs;
    }
};

// Single ASC int column with one NULL: NULL should sort LAST.
TEST_F(SortKeysNullTest, AscNullsLast) {
    Vector v(PhysicalType::INTEGER, 4);
    v.setValue(0, Value(30));
    v.setValue(1, Value::null());
    v.setValue(2, Value(10));
    v.setValue(3, Value(20));

    Vector keys(PhysicalType::STRING);
    CreateSortKey::createSortKey(v, 4, {OrderType::ASCENDING}, keys);
    auto order = sortedIndices(keys, 4);
    // expected order: [10, 20, 30, NULL] → row indices [2, 3, 0, 1]
    EXPECT_EQ(order, (std::vector<idx_t>{2, 3, 0, 1}));
}

// Single DESC int column with one NULL: NULL should sort FIRST (the natural flip
// of the same encoding; the conventional SQL "NULLS FIRST in DESC" default).
TEST_F(SortKeysNullTest, DescNullsFirst) {
    Vector v(PhysicalType::INTEGER, 4);
    v.setValue(0, Value(30));
    v.setValue(1, Value::null());
    v.setValue(2, Value(10));
    v.setValue(3, Value(20));

    Vector keys(PhysicalType::STRING);
    CreateSortKey::createSortKey(v, 4, {OrderType::DESCENDING}, keys);
    auto order = sortedIndices(keys, 4);
    // expected order: [NULL, 30, 20, 10] → row indices [1, 0, 3, 2]
    EXPECT_EQ(order, (std::vector<idx_t>{1, 0, 3, 2}));
}

// Two-column ASC with NULLs on the secondary key: tie on first col is broken by
// the second col, where NULL sorts after non-null.
TEST_F(SortKeysNullTest, TwoColAscSecondaryNullsLast) {
    // (1, NULL), (1, 5), (1, 2), (2, NULL), (2, 1)
    DataChunk chunk;
    vector<LogicalType> types = {PhysicalType::INTEGER, PhysicalType::INTEGER};
    chunk.initialize(types);
    chunk.setCardinality(5);
    chunk.setValue(0, 0, Value(1));     chunk.setValue(1, 0, Value::null());
    chunk.setValue(0, 1, Value(1));     chunk.setValue(1, 1, Value(5));
    chunk.setValue(0, 2, Value(1));     chunk.setValue(1, 2, Value(2));
    chunk.setValue(0, 3, Value(2));     chunk.setValue(1, 3, Value::null());
    chunk.setValue(0, 4, Value(2));     chunk.setValue(1, 4, Value(1));

    Vector keys(PhysicalType::STRING);
    vector<OrderModifiers> mods = {OrderType::ASCENDING, OrderType::ASCENDING};
    CreateSortKey::createSortKey(chunk, mods, keys);
    auto order = sortedIndices(keys, 5);
    // expected: (1,2), (1,5), (1,NULL), (2,1), (2,NULL)  -> indices 2,1,0,4,3
    EXPECT_EQ(order, (std::vector<idx_t>{2, 1, 0, 4, 3}));
}

// All values NULL — sort is trivially stable and every key encodes to the same bytes.
TEST_F(SortKeysNullTest, AllNullsKeepsRelativeOrder) {
    Vector v(PhysicalType::BIGINT, 3);
    v.setValue(0, Value::null());
    v.setValue(1, Value::null());
    v.setValue(2, Value::null());
    Vector keys(PhysicalType::STRING);
    CreateSortKey::createSortKey(v, 3, {OrderType::ASCENDING}, keys);
    auto data = FlatVector::getData<string_t>(keys);
    // every key should be identical (3 rows × 8 bytes of 0xFF)
    auto k0 = data[0].getString();
    EXPECT_EQ(data[1].getString(), k0);
    EXPECT_EQ(data[2].getString(), k0);
    EXPECT_EQ(k0.size(), sizeof(int64_t));
}

// STRING ASC with one NULL: every key starts with a 1-byte prefix; non-null
// strings sort before NULL.
TEST_F(SortKeysNullTest, StringAscNullsLast) {
    Vector v(PhysicalType::STRING, 4);
    v.setValue(0, Value("banana"));
    v.setValue(1, Value::null());
    v.setValue(2, Value("apple"));
    v.setValue(3, Value("cherry"));

    Vector keys(PhysicalType::STRING);
    CreateSortKey::createSortKey(v, 4, {OrderType::ASCENDING}, keys);
    auto order = sortedIndices(keys, 4);
    // expected: apple, banana, cherry, NULL -> indices 2, 0, 3, 1
    EXPECT_EQ(order, (std::vector<idx_t>{2, 0, 3, 1}));
}

// STRING DESC mirrors numeric DESC: NULLs naturally land first under the flip.
TEST_F(SortKeysNullTest, StringDescNullsFirst) {
    Vector v(PhysicalType::STRING, 4);
    v.setValue(0, Value("banana"));
    v.setValue(1, Value::null());
    v.setValue(2, Value("apple"));
    v.setValue(3, Value("cherry"));

    Vector keys(PhysicalType::STRING);
    CreateSortKey::createSortKey(v, 4, {OrderType::DESCENDING}, keys);
    auto order = sortedIndices(keys, 4);
    // expected: NULL, cherry, banana, apple -> indices 1, 3, 0, 2
    EXPECT_EQ(order, (std::vector<idx_t>{1, 3, 0, 2}));
}

// The byte-collision corner case: a string whose first encoded byte happens to
// be the maximum (input byte 0xFE shifts to 0xFF). Prefix scheme keeps it
// strictly less than NULL because non-null keys all share the lower prefix.
TEST_F(SortKeysNullTest, StringWithHighByteStillSortsBeforeNull) {
    Vector v(PhysicalType::STRING, 2);
    char buf[2] = {(char)0xFE, 0};
    v.setValue(0, Value(buf));
    v.setValue(1, Value::null());

    Vector keys(PhysicalType::STRING);
    CreateSortKey::createSortKey(v, 2, {OrderType::ASCENDING}, keys);
    auto order = sortedIndices(keys, 2);
    EXPECT_EQ(order, (std::vector<idx_t>{0, 1}));
}

// Dictionary input: the generic sort path orrifies the vector. Verify NULLs
// reached through the selection are encoded as the null marker (sort last in ASC).
TEST_F(SortKeysNullTest, DictionaryInputAscNullsLast) {
    Vector base(PhysicalType::INTEGER, 5);
    base.setValue(0, Value(30));
    base.setValue(1, Value(10));
    base.setValue(2, Value::null());
    base.setValue(3, Value(20));
    base.setValue(4, Value(40));
    SelectionVector sel(4);
    sel.setIndex(0, 0); // 30
    sel.setIndex(1, 1); // 10
    sel.setIndex(2, 2); // NULL
    sel.setIndex(3, 3); // 20
    Vector dict(base, sel, 4);

    Vector keys(PhysicalType::STRING);
    CreateSortKey::createSortKey(dict, 4, {OrderType::ASCENDING}, keys);
    auto order = sortedIndices(keys, 4);
    // expected: 10, 20, 30, NULL -> indices in `dict` are 1, 3, 0, 2
    EXPECT_EQ(order, (std::vector<idx_t>{1, 3, 0, 2}));
}

// Constant-vector NULL: a single null constant broadcast across N rows must produce
// keys that all sort identically.
TEST_F(SortKeysNullTest, ConstantNullVectorAllEqualKeys) {
    Vector constNull(Value::null());
    Vector keys(PhysicalType::STRING);
    CreateSortKey::createSortKey(constNull, 5, {OrderType::ASCENDING}, keys);
    auto data = FlatVector::getData<string_t>(keys);
    for (idx_t i = 1; i < 5; i++) {
        EXPECT_EQ(data[i].getString(), data[0].getString());
    }
}
