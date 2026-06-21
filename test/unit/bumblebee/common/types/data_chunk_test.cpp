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
#include "bumblebee/common/types/DataChunk.hpp"
#include "bumblebee/common/types/Value.hpp"
#include "bumblebee/common/types/SelectionVector.hpp"
#include "bumblebee/common/vector_operations/VectorOperations.hpp"

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
    chunk.initialize((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::INTEGER});
    EXPECT_EQ(chunk.columnCount(), 2);
    EXPECT_EQ(chunk.getSize(), 0);
}

TEST(DataChunkTests, EmptyInitializationInt32) {
    DataChunk chunk;
    chunk.initializeEmpty((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::BIGINT, PhysicalType::STRING});
    EXPECT_EQ(chunk.columnCount(), 3);
    EXPECT_EQ(chunk.getSize(), 0);
}

TEST(DataChunkTests, SetAndGetValueInt32) {
    DataChunk chunk;
    chunk.initialize((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::UTINYINT});
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
    chunk1.initialize((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::UINTEGER});
    fillChunk(chunk1, 10);

    DataChunk chunk2;
    chunk2.initialize((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::UINTEGER});
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
    chunk1.initialize((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::INTEGER});
    fillChunk(chunk1, initCardinalityChunk1);

    DataChunk chunk2;
    chunk2.initialize((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::INTEGER});
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
    chunk1.initialize((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::UBIGINT});
    fillChunk(chunk1,10);

    DataChunk chunk2;
    chunk2.initialize((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::UBIGINT});
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
    chunk1.initialize((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::INTEGER});
    fillChunk(chunk1,100);

    DataChunk chunk2;
    chunk2.initialize((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::INTEGER});
    fillChunk(chunk2,100, 1000);
    // expected different value in chunk2 and hunk 1
    for (idx_t i = 0; i < chunk2.columnCount(); i++) {
        for (idx_t j = 0; j < chunk2.getSize(); j++) {
            EXPECT_NE(chunk2.getValue(i, j), chunk1.getValue(i, j));
        }
    }

    chunk1.setCardinality(0);
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
    chunk.initialize((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::INTEGER});
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
    chunk.initialize((vector<PhysicalType>){PhysicalType::BIGINT, PhysicalType::BIGINT});
    fillChunk(chunk, 1000);
    chunk.reset();
    EXPECT_EQ(chunk.getSize(), 0);
    EXPECT_EQ(chunk.columnCount(), 2);
}

TEST(DataChunkTests, SplitInt32) {
    DataChunk chunk1;
    chunk1.initialize((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::INTEGER, PhysicalType::INTEGER});
    // copy the chunk
    DataChunk originalChunk;
    originalChunk.initialize((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::INTEGER, PhysicalType::INTEGER});
    originalChunk.setCardinality(100);
    fillChunk(originalChunk, 100);
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
    chunk.initialize((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::UINTEGER});
    fillChunk(chunk,50);

    Vector hash_vec1(LogicalTypeId::HASH);
    chunk.hash(hash_vec1);
    EXPECT_EQ(hash_vec1.getType(), PhysicalType::UBIGINT);
    EXPECT_EQ(hash_vec1.getVectorType(), VectorType::FLAT_VECTOR);

    // create a second vector hash it and compare wihth the first, result should be equal
    Vector hash_vec2(LogicalTypeId::HASH);
    chunk.hash(hash_vec2);
    EXPECT_EQ(hash_vec2.getType(), PhysicalType::UBIGINT);
    EXPECT_EQ(hash_vec2.getVectorType(), VectorType::FLAT_VECTOR);
    for (idx_t i = 0; i < chunk.getSize(); i++) {
        EXPECT_EQ(hash_vec1.getValue(i), hash_vec2.getValue(i));
    }
    // std::cout << hash_vec.toString(50) << std::endl;
}

TEST(DataChunkTests, OrrifyInt32) {
    DataChunk chunk;
    chunk.initialize((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::INTEGER});
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
    chunk.initialize((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::INTEGER});
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

// ---------- NULL propagation across DataChunk movement ----------

TEST(DataChunkNullTests, AppendCarriesNull) {
    DataChunk chunk1;
    chunk1.initialize((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::INTEGER});
    fillChunk(chunk1, 10);

    DataChunk chunk2;
    chunk2.initialize((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::INTEGER});
    fillChunk(chunk2, 10);
    chunk2.setValue(1, 4, Value::null()); // null in col 1, row 4

    chunk1.append(chunk2);
    EXPECT_EQ(chunk1.getSize(), 20);
    // null landed at row 10+4 in chunk1
    EXPECT_TRUE(chunk1.getValue(1, 14).isNull());
    EXPECT_FALSE(chunk1.getValue(1, 13).isNull());
}

TEST(DataChunkNullTests, AppendWithSelectionCarriesNull) {
    DataChunk chunk1;
    chunk1.initialize((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::INTEGER});
    fillChunk(chunk1, 10);

    DataChunk chunk2;
    chunk2.initialize((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::INTEGER});
    fillChunk(chunk2, 20);
    chunk2.setValue(0, 15, Value::null()); // selected below

    SelectionVector sel(10);
    for (idx_t i = 0; i < 10; i++) sel.setIndex(i, i + 10); // picks rows 10..19

    chunk1.append(chunk2, true, &sel, 10);
    EXPECT_EQ(chunk1.getSize(), 20);
    // source row 15 maps to sel position 5 -> target row 10+5 = 15
    EXPECT_TRUE(chunk1.getValue(0, 15).isNull());
    EXPECT_FALSE(chunk1.getValue(0, 14).isNull());
}

TEST(DataChunkNullTests, CopyCarriesNull) {
    DataChunk src;
    src.initialize((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::INTEGER});
    fillChunk(src, 50);
    src.setValue(0, 7, Value::null());

    DataChunk dst;
    dst.initialize((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::INTEGER});
    dst.setCardinality(0);
    src.copy(dst);
    EXPECT_EQ(dst.getSize(), 50);
    EXPECT_TRUE(dst.getValue(0, 7).isNull());
    EXPECT_FALSE(dst.getValue(0, 6).isNull());
}

TEST(DataChunkNullTests, SliceReadsNull) {
    DataChunk chunk;
    chunk.initialize((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::INTEGER});
    fillChunk(chunk, 20);
    chunk.setValue(1, 13, Value::null());

    SelectionVector sel(10);
    for (idx_t i = 0; i < 10; i++) sel.setIndex(i, i + 10); // rows 10..19

    chunk.slice(sel, 10);
    // source row 13 -> sliced position 3
    EXPECT_TRUE(chunk.getValue(1, 3).isNull());
    EXPECT_FALSE(chunk.getValue(1, 2).isNull());
}

TEST(DataChunkNullTests, ResetClearsNull) {
    DataChunk chunk;
    chunk.initialize((vector<PhysicalType>){PhysicalType::BIGINT, PhysicalType::BIGINT});
    fillChunk(chunk, 100);
    chunk.setValue(0, 3, Value::null());
    EXPECT_TRUE(chunk.getValue(0, 3).isNull());

    chunk.reset();
    fillChunk(chunk, 100);
    // after reset+refill the previously-null slot is a normal value again
    EXPECT_FALSE(chunk.getValue(0, 3).isNull());
}

TEST(DataChunkNullTests, CastCarriesNull) {
    DataChunk chunk;
    chunk.initialize((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::INTEGER});
    fillChunk(chunk, 30);
    chunk.setValue(0, 9, Value::null());

    chunk.cast((vector<LogicalType>){PhysicalType::BIGINT, PhysicalType::INTEGER});
    EXPECT_EQ(chunk.data_[0].getType(), PhysicalType::BIGINT);
    EXPECT_TRUE(chunk.getValue(0, 9).isNull());
    EXPECT_FALSE(chunk.getValue(0, 8).isNull());
}

// cast(DataChunk& result) overload (writes into a pre-typed result chunk)
TEST(DataChunkNullTests, CastIntoResultCarriesNull) {
    DataChunk src;
    src.initialize((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::INTEGER});
    fillChunk(src, 30);
    src.setValue(0, 9, Value::null());

    DataChunk result;
    result.initialize((vector<PhysicalType>){PhysicalType::BIGINT, PhysicalType::INTEGER});
    result.setCardinality(30);
    src.cast(result);
    EXPECT_TRUE(result.getValue(0, 9).isNull());
    EXPECT_FALSE(result.getValue(0, 8).isNull());
}

// copy(other, sel, sourceCount, offset) variant carries nulls
TEST(DataChunkNullTests, CopyWithSelectionCarriesNull) {
    DataChunk src;
    src.initialize((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::INTEGER});
    fillChunk(src, 20);
    src.setValue(1, 7, Value::null());

    SelectionVector sel(20);
    for (idx_t i = 0; i < 20; i++) sel.setIndex(i, i);

    DataChunk dst;
    dst.initialize((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::INTEGER});
    src.copy(dst, sel, 20, 0);
    EXPECT_EQ(dst.getSize(), 20);
    EXPECT_TRUE(dst.getValue(1, 7).isNull());
    EXPECT_FALSE(dst.getValue(1, 6).isNull());
}

// append that forces a resize must preserve the target's existing nulls
TEST(DataChunkNullTests, AppendWithResizePreservesNull) {
    DataChunk chunk1;
    chunk1.initialize((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::INTEGER});
    fillChunk(chunk1, 1000);
    chunk1.setValue(0, 5, Value::null());

    DataChunk chunk2;
    chunk2.initialize((vector<PhysicalType>){PhysicalType::INTEGER, PhysicalType::INTEGER});
    fillChunk(chunk2, 300);
    chunk2.setValue(0, 1, Value::null());

    chunk1.append(chunk2, true); // 1000 + 300 = 1300 > capacity 1024 -> resize
    EXPECT_EQ(chunk1.getSize(), 1300);
    EXPECT_TRUE(chunk1.getValue(0, 5).isNull());      // target's null survived resize
    EXPECT_TRUE(chunk1.getValue(0, 1000 + 1).isNull()); // appended null landed
    EXPECT_FALSE(chunk1.getValue(0, 999).isNull());
}


