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
#include "bumblebee/common/types/ChunkCollection.hpp"
#include "bumblebee/common/types/Value.hpp"

using namespace bumblebee;

class ChunkCollectionTest : public ::testing::Test {
// Creates and returns a DataChunk initialized with a predefined set of column types: INTEGER, UINTEGER, and BIGINT.
// For each of the count rows, it populates the columns with incrementing values:
// Column 0 (INTEGER): sequential int32_t values starting from 0.
// Column 1 (UINTEGER): uint32_t values equal to i * 10.
// Column 2 (BIGINT): int64_t values equal to i * 100.
// The function sets the cardinality of the chunk to count and returns it.
// This utility is primarily used for generating consistent and type-diverse data for testing the ChunkCollection class.
protected:
    ChunkCollection collection;

    vector<ConstantType> testTypes{ConstantType::INTEGER, ConstantType::UINTEGER, ConstantType::BIGINT};

    DataChunk createChunkWithValue( idx_t count = 1) {
        DataChunk chunk;
        chunk.initialize(testTypes);
        chunk.setCardinality(count);
        for (idx_t i = 0; i < count; ++i) {
            chunk.setValue(0, i, Value((int32_t) i));
            chunk.setValue(1, i, Value((uint32_t) i * 10));
            chunk.setValue(2, i, Value((int64_t) i * 100));
        }
        chunk.setCardinality(count);
        return chunk;
    }
};

TEST_F(ChunkCollectionTest, InitialState) {
    EXPECT_EQ(collection.getCount(), 0);
    EXPECT_EQ(collection.columnCount(), 0);
    EXPECT_EQ(collection.chunkCount(), 0);
    EXPECT_TRUE(collection.getTypes().empty());
    EXPECT_EQ(collection.toString(), "ChunkCollection [ 0 ]");
}

TEST_F(ChunkCollectionTest, AppendSingleChunk) {
    DataChunk chunk = createChunkWithValue(3);
    collection.append(chunk);

    EXPECT_EQ(collection.getCount(), 3);
    EXPECT_EQ(collection.columnCount(), 3);
    EXPECT_EQ(collection.chunkCount(), 1);
    std::cout << collection.toString() << std::endl;
    for (auto i=0;i<3;++i) {
        EXPECT_EQ(collection.getValue(0, i), Value( (int32_t) i ));
        EXPECT_EQ(collection.getValue(1, i), Value((uint32_t)i * 10));
        EXPECT_EQ(collection.getValue(2, i), Value((int64_t)i * 100));
    }
}

TEST_F(ChunkCollectionTest, AppendMultipleChunks) {
    for (int i = 0; i < 2; ++i) {
        DataChunk chunk = createChunkWithValue( 2);
        collection.append(chunk);
    }
    EXPECT_EQ(collection.getCount(), 4);
    EXPECT_EQ(collection.chunkCount(), 1);
}


TEST_F(ChunkCollectionTest, AppendOnEmptyCollection) {
    auto chunk1 = createChunkWithValue(3);
    auto chunk2 = createChunkWithValue(3);
    auto chunk3 = createChunkWithValue(3);

    ChunkCollection other;
    other.append(chunk1);
    other.append(chunk2);
    other.append(chunk3);
    collection.append(other);

    EXPECT_EQ(collection.getCount(), 3*3);
    EXPECT_EQ(collection.chunks().size(), 1);
}

TEST_F(ChunkCollectionTest, AppendCollection) {
    auto chunk1 = createChunkWithValue(3);
    auto chunk2 = createChunkWithValue(3);
    auto chunk3 = createChunkWithValue(3);

    collection.append(chunk1);
    ChunkCollection other;
    other.append(chunk2);
    other.append(chunk3);

    collection.append(other);

    EXPECT_EQ(collection.getCount(), 3*3);
    EXPECT_EQ(collection.chunks().size(), 1);
}

TEST_F(ChunkCollectionTest, MergeOnEmptyCollections) {
    auto chunk = createChunkWithValue(2);
    ChunkCollection other;
    other.append(chunk);
    collection.merge(other);

    EXPECT_EQ(collection.getCount(), 2);
    EXPECT_EQ(collection.getTypes().size(), 3);
    EXPECT_EQ(collection.getValue(0, 1), Value(1));
}

TEST_F(ChunkCollectionTest, MergeCollections) {
    auto chunk1 = createChunkWithValue(2);
    auto chunk2 = createChunkWithValue(2);
    ChunkCollection other;
    other.append(chunk1);
    collection.append(chunk2);

    collection.merge(other);

    EXPECT_EQ(collection.getCount(), 4);
    EXPECT_EQ(collection.getTypes().size(), 3);
    EXPECT_EQ(collection.getValue(1,0), collection.getValue(1,2));
    EXPECT_EQ(collection.getValue(1,1), collection.getValue(1,3));
}

TEST_F(ChunkCollectionTest, FuseCollections) {
    auto chunk1 = createChunkWithValue(2);
    auto chunk2 = createChunkWithValue(2);
    ChunkCollection other;
    other.append(chunk1);
    collection.append(chunk2);

    collection.fuse(other);
    std::cout << collection.toString() << std::endl;

    EXPECT_EQ(collection.columnCount(), 6);
    EXPECT_EQ(collection.getCount(), 2);
    EXPECT_EQ(collection.getValue(0,1), collection.getValue(3,1));
    EXPECT_EQ(collection.getValue(2,1), collection.getValue(5,1));

}

TEST_F(ChunkCollectionTest, Reset) {
    auto chunk = createChunkWithValue(2);
    collection.append(chunk);
    collection.reset();
    EXPECT_EQ(collection.getCount(), 0);
    EXPECT_EQ(collection.chunkCount(), 0);
    EXPECT_EQ(collection.columnCount(), 0);
}

TEST_F(ChunkCollectionTest, SetAndGetValue) {
    auto chunk1 = createChunkWithValue(STANDARD_VECTOR_SIZE);
    auto chunk2 = createChunkWithValue(STANDARD_VECTOR_SIZE);
    auto chunk3 = createChunkWithValue(STANDARD_VECTOR_SIZE);
    collection.append(chunk1);
    collection.append(chunk2);
    collection.append(chunk3);
    collection.setValue(0, 0, Value(123));
    collection.setValue(0, STANDARD_VECTOR_SIZE + 10, Value(123));
    EXPECT_EQ(collection.getValue(0, 0), Value(123));
    EXPECT_EQ(collection.getValue(0, STANDARD_VECTOR_SIZE + 10), Value(123));
}

TEST_F(ChunkCollectionTest, FetchChunk) {
    auto chunk1 = createChunkWithValue(STANDARD_VECTOR_SIZE);
    auto chunk2 = createChunkWithValue(STANDARD_VECTOR_SIZE);
    collection.append(chunk1);
    collection.append(chunk2);
    auto chunk = collection.fetch();
    EXPECT_TRUE(chunk != nullptr);
    EXPECT_EQ(collection.chunkCount(), 1);
    EXPECT_EQ(collection.getCount(), STANDARD_VECTOR_SIZE);
    chunk = collection.fetch();
    EXPECT_TRUE(chunk != nullptr);
    EXPECT_EQ(collection.chunkCount(), 0);
    EXPECT_EQ(collection.getCount(), 0);
}

TEST_F(ChunkCollectionTest, EqualsMethod) {
    auto chunk1 = createChunkWithValue(2);
    auto chunk2 = createChunkWithValue(2);
    ChunkCollection other;
    collection.append(chunk1);
    other.append(chunk2);
    EXPECT_TRUE(collection.equals(other));
    other.setValue(0, 1, Value(99));
    EXPECT_FALSE(collection.equals(other));
}

TEST_F(ChunkCollectionTest, CopyCell) {
    auto chunk1 = createChunkWithValue(10);
    collection.append(chunk1);
    std::cout << collection.toString() << std::endl;
    Vector target(ConstantType::UINTEGER);
    collection.copyCell(1, 5, target, 0);
    std::cout<<target.toString(10)<<std::endl;
    EXPECT_EQ(target.getValue(0), Value((uint32_t)50));
}


