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
#include "bumblebee/common/types/DataChunk.h"
#include "bumblebee/common/types/Value.h"
#include "bumblebee/common/types/SelectionVector.h"
#include "bumblebee/common/vector_operations/VectorOperations.h"

using namespace bumblebee;

/**
 * Fills the given DataChunk with synthetic test data.
 *
 * This function sets the cardinality of the chunk and populates each cell
 * with numeric values computed based on the provided base value, column index, and row index.
 * The values are type-cast to match the type of each column's vector.
 *
 * Value formula: value = base + (column_index * 100 * (row_index + 1))
 *
 * The DataChunk to fill. Must be initialized with appropriate column types (using initialize()).
 * The base value to start from (default is 0).
 */
void fillChunk(DataChunk &chunk, unsigned cardinality, int base = 0) {
    chunk.setCardinality(cardinality);
    for (idx_t i = 0; i < chunk.columnCount(); i++) {
        auto ctype = chunk.data_[i].getType();
        for (idx_t j = 0; j < chunk.getSize(); j++) {
            auto numericValue = base + i * 100 * (j + 1);
            auto v = Value(numericValue);
            chunk.setValue(i, j, v.cast(ctype) );
        }
    }
}

TEST(DataChunkTests, InitializationInt32) {
    DataChunk chunk;
    chunk.initialize({ConstantType::INTEGER, ConstantType::INTEGER});
    EXPECT_EQ(chunk.columnCount(), 2);
    EXPECT_EQ(chunk.getSize(), 0);
}

TEST(DataChunkTests, EmptyInitializationInt32) {
    DataChunk chunk;
    chunk.initializeEmpty({ConstantType::INTEGER, ConstantType::BIGINT, ConstantType::STRING});
    EXPECT_EQ(chunk.columnCount(), 3);
    EXPECT_EQ(chunk.getSize(), 0);
}

TEST(DataChunkTests, SetAndGetValueInt32) {
    DataChunk chunk;
    chunk.initialize({ConstantType::INTEGER, ConstantType::UTINYINT});
    chunk.setCardinality(1);
    auto v = Value((int32_t)42);
    chunk.setValue(0, 0, v);
    EXPECT_EQ(chunk.getValue(0, 0), v);
    v = Value((uint8_t)10);
    chunk.setValue(1, 0, v);
    EXPECT_EQ(chunk.getValue(1, 0), v);
}

TEST(DataChunkTests, ReferenceInt32) {
    DataChunk chunk1;
    chunk1.initialize({ConstantType::INTEGER, ConstantType::UINTEGER});
    fillChunk(chunk1, 10);

    DataChunk chunk2;
    chunk2.initialize({ConstantType::INTEGER, ConstantType::UINTEGER});
    chunk2.reference(chunk1);

    //update chunk1;  should be reflected also in chunk 2
    chunk1.setValue(0,0, Value(12345));
    for (idx_t i = 0; i < chunk2.columnCount(); i++) {
        for (idx_t j = 0; j < chunk2.getSize(); j++) {
            EXPECT_EQ(chunk2.getValue(i, j), chunk1.getValue(i, j));
        }
    }
}

TEST(DataChunkTests, AppendWithoutResizeInt32) {
    auto initCardinalityChunk1 = 10;
    DataChunk chunk1;
    chunk1.initialize({ConstantType::INTEGER, ConstantType::INTEGER});
    fillChunk(chunk1, initCardinalityChunk1);

    DataChunk chunk2;
    chunk2.initialize({ConstantType::INTEGER, ConstantType::INTEGER});
    fillChunk(chunk2, 10);

    // Append chunk2 in chunk1; then check that all the data in chunk1 is present in chunk2
    chunk1.append(chunk2);
    EXPECT_EQ(chunk1.getSize(), 20);
    for (idx_t i = 0; i < chunk2.columnCount(); i++) {
        for (idx_t j = 0; j < chunk2.getSize(); j++) {
            EXPECT_EQ(chunk2.getValue(i, j), chunk1.getValue(i, j+initCardinalityChunk1));
        }
    }
}


TEST(DataChunkTests, AppendWithSelectionVectorInt32) {
    DataChunk chunk1;
    chunk1.initialize({ConstantType::INTEGER, ConstantType::UBIGINT});
    fillChunk(chunk1,10);

    DataChunk chunk2;
    chunk2.initialize({ConstantType::INTEGER, ConstantType::UBIGINT});
    fillChunk(chunk2, 20);

    // selection vector of last 10 numbers in chunk2
    // 0 -> 10
    // 1 -> 11
    // ...
    // 9 -> 19
    SelectionVector sel(10);
    for (idx_t i = 0; i < 10; i++) sel.setIndex(i, i+10);

    chunk1.append(chunk2, true, &sel, 10);
    EXPECT_EQ(chunk1.getSize(), 20);
    for (idx_t i = 0; i < chunk2.columnCount(); i++) {
        for (idx_t j = 10; j < chunk2.getSize(); j++) {
            EXPECT_EQ(chunk2.getValue(i, j), chunk1.getValue(i, j));
        }
    }
}

TEST(DataChunkTests, CopyDataChunkInt32) {
    DataChunk chunk1;
    chunk1.initialize({ConstantType::INTEGER, ConstantType::INTEGER});
    fillChunk(chunk1,100);

    DataChunk chunk2;
    chunk2.initialize({ConstantType::INTEGER, ConstantType::INTEGER});
    fillChunk(chunk2,100, 1000);
    // expected different value in chunk2 and hunk 1
    for (idx_t i = 0; i < chunk2.columnCount(); i++) {
        for (idx_t j = 0; j < chunk2.getSize(); j++) {
            EXPECT_NE(chunk2.getValue(i, j), chunk1.getValue(i, j));
        }
    }

    chunk2.copy(chunk1);
    EXPECT_EQ(chunk2.getSize(), 100);
    // expected same values in chunk1 and chunk2
    for (idx_t i = 0; i < chunk2.columnCount(); i++) {
        for (idx_t j = 0; j < chunk2.getSize(); j++) {
            EXPECT_EQ(chunk2.getValue(i, j), chunk1.getValue(i, j));
        }
    }
}

TEST(DataChunkTests, SliceAndNormalifyInt32) {
    DataChunk chunk;
    chunk.initialize({ConstantType::INTEGER, ConstantType::INTEGER});
    fillChunk(chunk, 20);

    // selection vector of last 10 numbers in chunk2
    // 0 -> 10
    // 1 -> 11
    // ...
    // 9 -> 19
    SelectionVector sel(10);
    for (idx_t i = 0; i < 10; i++) sel.setIndex(i, i + 10);

    chunk.slice(sel, 10);
    chunk.normalify();
    EXPECT_EQ(chunk.getSize(), 10);
    // Expected column 0 all 0
    for (idx_t j = 0; j < chunk.getSize(); j++) {
        EXPECT_EQ(chunk.getValue(0, j), Value(0));
    }
    // Expected column 1 [1100, 1200, 1300, 1400, 1500, 1600, 1700, 1800, 1900, 2000, ]
    for (idx_t j = 0; j < chunk.getSize(); j++) {
        auto valExpected = 1000 + (j+1)*100;
        EXPECT_EQ(chunk.getValue(1, j), Value((int32_t)( valExpected )) ) ;
    }
}

TEST(DataChunkTests, ResetInt32) {
    DataChunk chunk;
    chunk.initialize({ConstantType::BIGINT, ConstantType::BIGINT});
    fillChunk(chunk, 1000);
    chunk.reset();
    EXPECT_EQ(chunk.getSize(), 0);
    EXPECT_EQ(chunk.columnCount(), 2);
}

TEST(DataChunkTests, SplitInt32) {
    DataChunk chunk1;
    chunk1.initialize({ConstantType::INTEGER, ConstantType::INTEGER, ConstantType::INTEGER});
    fillChunk(chunk1, 100);
    // copy the chunk
    DataChunk originalChunk;
    originalChunk.initialize({ConstantType::INTEGER, ConstantType::INTEGER, ConstantType::INTEGER});
    originalChunk.setCardinality(100);
    originalChunk.copy(chunk1);

    DataChunk chunk2;
    // col 0 and 1 remain in chunk1 col 2 in chunk2
    chunk1.split(chunk2, 2);
    EXPECT_EQ(chunk1.columnCount(), 2);
    EXPECT_EQ(chunk2.columnCount(), 1);
    EXPECT_EQ(chunk2.getSize(), originalChunk.getSize());
    EXPECT_EQ(chunk1.getSize(), originalChunk.getSize());
    // check the vector data
    for (idx_t j = 0; j < originalChunk.getSize(); j++) {
        EXPECT_EQ(originalChunk.getValue(0, j), chunk1.getValue(0, j) ) ;
        EXPECT_EQ(originalChunk.getValue(1, j), chunk1.getValue(1, j) ) ;
    }
    for (idx_t j = 0; j < originalChunk.getSize(); j++) {
        EXPECT_EQ(originalChunk.getValue(2, j), chunk2.getValue(0, j) ) ;
    }
}

TEST(DataChunkTests, HashInt32) {
    DataChunk chunk;
    chunk.initialize({ConstantType::INTEGER, ConstantType::UINTEGER});
    fillChunk(chunk,50);

    Vector hash_vec1(ConstantType::UBIGINT);
    chunk.hash(hash_vec1);
    EXPECT_EQ(hash_vec1.getType(), ConstantType::UBIGINT);
    EXPECT_EQ(hash_vec1.getVectorType(), VectorType::FLAT_VECTOR);

    // create a second vector hash it and compare wihth the first, result should be equal
    Vector hash_vec2(ConstantType::UBIGINT);
    chunk.hash(hash_vec2);
    EXPECT_EQ(hash_vec2.getType(), ConstantType::UBIGINT);
    EXPECT_EQ(hash_vec2.getVectorType(), VectorType::FLAT_VECTOR);
    for (idx_t i = 0; i < chunk.getSize(); i++) {
        EXPECT_EQ(hash_vec1.getValue(i), hash_vec2.getValue(i));
    }
    // std::cout << hash_vec.toString(50) << std::endl;
}

TEST(DataChunkTests, OrrifyInt32) {
    DataChunk chunk;
    chunk.initialize({ConstantType::INTEGER, ConstantType::INTEGER});
    fillChunk(chunk,20);

    // selection vector of last 10 numbers in chunk2
    // 0 -> 10
    // 1 -> 11
    // ...
    // 9 -> 19
    SelectionVector sel(10);
    for (idx_t i = 0; i < 10; i++) sel.setIndex(i, i + 10);
    chunk.slice(sel, 10);

    auto data = chunk.orrify();
    EXPECT_NE(data, nullptr);
    for (idx_t i = 0; i < chunk.columnCount(); i++) {
        auto colData = (int32_t*)data[i].data_;
        auto sel_p = data[i].sel_;
        for (idx_t j = 0; j < chunk.getSize(); j++) {
            EXPECT_EQ( colData[ sel_p->getIndex(j) ], chunk.getValue(i, j).getNumericValue<int32_t>() );
        }
    }
}

TEST(DataChunkTests, OrrifyWithSelectionInt32) {
    DataChunk chunk;
    chunk.initialize({ConstantType::INTEGER, ConstantType::INTEGER});
    fillChunk(chunk,10);

    auto data = chunk.orrify();
    EXPECT_NE(data, nullptr);
    for (idx_t i = 0; i < chunk.columnCount(); i++) {
        auto colData = (int32_t*)data[i].data_;
        for (idx_t j = 0; j < chunk.getSize(); j++) {
            EXPECT_EQ( colData[j], chunk.getValue(i, j).getNumericValue<int32_t>() );
        }
    }
}


