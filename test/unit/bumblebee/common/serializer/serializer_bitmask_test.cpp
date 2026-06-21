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

#include "bumblebee/common/serializer/BufferedSerializer.hpp"
#include "bumblebee/common/serializer/BufferedDeserializer.hpp"
#include "bumblebee/common/types/ValidityMask.hpp"

using namespace bumblebee;

// All-valid mask round-trips as all-valid; the on-disk payload is a single byte
// (the allValid flag) — no packed mask bytes are written when there are no nulls.
TEST(SerializerBitmaskTest, AllValidRoundTrip) {
    ValidityMask mask;
    BufferedSerializer ser;
    ser.writeBitmask(mask, 128);
    // Exactly one byte written: the allValid flag (=1).
    EXPECT_EQ(ser.blob_.size_, 1u);

    BufferedDeserializer des(ser);
    ValidityMask out;
    des.readBitmask(out, 128);
    EXPECT_TRUE(out.allValid());
    for (idx_t i = 0; i < 128; i++) {
        EXPECT_TRUE(out.rowIsValid(i));
    }
}

// Sparse nulls round-trip exactly. Both the cleared and the set bits survive.
TEST(SerializerBitmaskTest, SparseNullsRoundTrip) {
    ValidityMask mask;
    const idx_t count = 100;
    // Clear an irregular pattern that crosses byte boundaries.
    for (idx_t i : {3u, 7u, 8u, 9u, 15u, 16u, 63u, 64u, 65u, 99u}) {
        mask.setInvalid(i);
    }

    BufferedSerializer ser;
    ser.writeBitmask(mask, count);
    // 1 flag byte + ceil(100/8)=13 packed bytes.
    EXPECT_EQ(ser.blob_.size_, 1u + 13u);

    BufferedDeserializer des(ser);
    ValidityMask out;
    des.readBitmask(out, count);
    EXPECT_FALSE(out.allValid());
    for (idx_t i = 0; i < count; i++) {
        bool expected = mask.rowIsValid(i);
        EXPECT_EQ(out.rowIsValid(i), expected) << "row " << i;
    }
}

// Multi-word masks (>5000 rows, > STANDARD_VECTOR_SIZE) round-trip — exercises
// lazy buffer growth on the read side and multi-byte writes on the write side.
TEST(SerializerBitmaskTest, MultiWordRoundTrip) {
    ValidityMask mask;
    const idx_t count = 5300;  // > STANDARD_VECTOR_SIZE, > one uint64_t word
    // Null every 17th row and a chunk at the end.
    for (idx_t i = 0; i < count; i++) {
        if (i % 17 == 0) mask.setInvalid(i);
    }
    for (idx_t i = 5200; i < count; i++) {
        mask.setInvalid(i);
    }

    BufferedSerializer ser;
    ser.writeBitmask(mask, count);
    EXPECT_EQ(ser.blob_.size_, 1u + (count + 7u) / 8u);

    BufferedDeserializer des(ser);
    ValidityMask out;
    des.readBitmask(out, count);
    EXPECT_FALSE(out.allValid());
    for (idx_t i = 0; i < count; i++) {
        EXPECT_EQ(out.rowIsValid(i), mask.rowIsValid(i)) << "row " << i;
    }
}

// All-invalid round-trips: every bit cleared, count rows.
TEST(SerializerBitmaskTest, AllInvalidRoundTrip) {
    ValidityMask mask;
    const idx_t count = 64;
    mask.setAllInvalid(count);

    BufferedSerializer ser;
    ser.writeBitmask(mask, count);
    EXPECT_EQ(ser.blob_.size_, 1u + 8u);

    BufferedDeserializer des(ser);
    ValidityMask out;
    des.readBitmask(out, count);
    EXPECT_FALSE(out.allValid());
    for (idx_t i = 0; i < count; i++) {
        EXPECT_FALSE(out.rowIsValid(i));
    }
}

// The bitmask can be embedded inside a larger blob — surrounding writes are not
// disturbed, and the deserializer reads bytes in the order they were written.
TEST(SerializerBitmaskTest, EmbeddedInLargerPayload) {
    ValidityMask mask;
    mask.setInvalid(2);
    mask.setInvalid(5);
    const idx_t count = 8;

    BufferedSerializer ser;
    ser.write<uint32_t>(0xCAFEBABE);
    ser.writeBitmask(mask, count);
    ser.write<uint32_t>(0xDEADBEEF);

    BufferedDeserializer des(ser);
    EXPECT_EQ(des.read<uint32_t>(), 0xCAFEBABE);
    ValidityMask out;
    des.readBitmask(out, count);
    EXPECT_FALSE(out.rowIsValid(2));
    EXPECT_FALSE(out.rowIsValid(5));
    for (idx_t i : {0u, 1u, 3u, 4u, 6u, 7u}) {
        EXPECT_TRUE(out.rowIsValid(i));
    }
    EXPECT_EQ(des.read<uint32_t>(), 0xDEADBEEF);
}

// count==0 writes only the flag byte (no payload either way), and reads back
// as all-valid.
TEST(SerializerBitmaskTest, ZeroCount) {
    ValidityMask mask;
    BufferedSerializer ser;
    ser.writeBitmask(mask, 0);
    EXPECT_EQ(ser.blob_.size_, 1u);

    BufferedDeserializer des(ser);
    ValidityMask out;
    out.setInvalid(0);  // mark something so we can detect reset
    des.readBitmask(out, 0);
    EXPECT_TRUE(out.allValid());
}