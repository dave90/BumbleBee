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
#include "gtest/gtest.h"
#include "bumblebee/common/types/Vector.h"
#include "bumblebee/common/types/Value.h"
#include "bumblebee/common/TypeDefs.h"

using namespace bumblebee;

// ---------- Test construction ----------

TEST(VectorTest, ConstructFromValue) {
    Value val(42);
    Vector vec(val);
    EXPECT_EQ(vec.getVectorType(), VectorType::CONSTANT_VECTOR);
    EXPECT_EQ(vec.getValue(0), val);
}

TEST(VectorTest, ConstructFlatVectorZeroInit) {
    Vector vec(ConstantType::INTEGER, true, true, 10);
    EXPECT_EQ(vec.getVectorType(), VectorType::FLAT_VECTOR);
    for (idx_t i = 0; i < 10; ++i) {
        EXPECT_EQ(vec.getValue(i), Value(0));
    }
}

TEST(VectorTest, ConstructSequenceVector) {
    Vector vec(ConstantType::BIGINT);
    vec.sequence(5, 2);
    EXPECT_EQ(vec.getVectorType(), VectorType::SEQUENCE_VECTOR);
    for (idx_t i = 0; i < 5; ++i) {
        EXPECT_EQ(vec.getValue(i), Value(5 + 2 * i));
    }
}

TEST(VectorTest, ConstructCircularSequenceVector) {
    Vector vec(ConstantType::BIGINT);
    vec.sequence(5,0, 1, 10); // 5 6 7 8 9 10 5 6 7 8 ...
    EXPECT_EQ(vec.getVectorType(), VectorType::SEQUENCE_CIRCULAR_VECTOR);
    std::cout <<vec.toString(10) << std::endl;
    for (idx_t i = 0; i < 10; ++i) {
        EXPECT_EQ(vec.getValue(i), Value((int64_t)( 5 + ( i % 6 ))));
    }
}

TEST(VectorTest, ConstructCircularSequenceVectorWithOffsetAndStride) {
    Vector vec(ConstantType::BIGINT);
    vec.sequence(5,4, 2, 10); // 7 7 8 8 9 9 10 10 5 5 6 6 ....
    EXPECT_EQ(vec.getVectorType(), VectorType::SEQUENCE_CIRCULAR_VECTOR);
    std::cout <<vec.toString(10) << std::endl;
    for (idx_t i = 0; i < 10; ++i) {
        EXPECT_EQ(vec.getValue(i), Value((int64_t)( 5  + (i +4) / 2 % 6 ) ));
    }
}

TEST(VectorTest, ConstructNegativeCircularSequenceVector) {
    Vector vec(ConstantType::BIGINT);
    vec.sequence(-4, 0, 3, 4); // -4 -4 -4 -3 -3 -3 -2 -2 -2 -1 -1 -1 ....
    EXPECT_EQ(vec.getVectorType(), VectorType::SEQUENCE_CIRCULAR_VECTOR);
    std::cout <<vec.toString(20) << std::endl;
    for (idx_t i = 0; i < 100; ++i) {
        auto val = (int64_t)(-4 + (i / 3 % 9));
        EXPECT_EQ(vec.getValue(i), Value(val));
    }
}

TEST(VectorTest, MoveConstructor) {
    Vector vec1(ConstantType::INTEGER, true);
    vec1.setValue(0, Value(123));
    Vector vec2(std::move(vec1));
    EXPECT_EQ(vec2.getValue(0), Value(123));
}

// ---------- Test slice & reference ----------

TEST(VectorTest, SliceFlatVectorOffset) {
    Vector vec(ConstantType::INTEGER);
    for (idx_t i = 0; i < 5; ++i) {
        vec.setValue(i, Value(int32_t(i)));
    }
    Vector sliced(vec, 2);
    EXPECT_EQ(sliced.getValue(0), Value(2));
}

TEST(VectorTest, SliceWithSelectionVector) {
    Vector vec(ConstantType::INTEGER);
    for (idx_t i = 0; i < 10; ++i) {
        vec.setValue(i, Value(int32_t(i * 10)));
    }

    SelectionVector sel(3);
    sel.setIndex(0, 1);
    sel.setIndex(1, 2);
    sel.setIndex(2, 3);

    Vector dictVec(vec, sel, 2);
    EXPECT_EQ(dictVec.getVectorType(), VectorType::DICTIONARY_VECTOR);
    EXPECT_EQ(dictVec.getValue(0), Value(10));
    EXPECT_EQ(dictVec.getValue(1), Value(20));
}


TEST(VectorTest, SliceOfSliceSelectionVector) {
    Vector vec(ConstantType::INTEGER);
    for (idx_t i = 0; i < 10; ++i) {
        vec.setValue(i, Value(int32_t(i * 10)));
    }

    {
        SelectionVector sel1(5);
        sel1.setIndex(0, 3);
        sel1.setIndex(1, 4);
        sel1.setIndex(2, 5);
        sel1.setIndex(3, 7);
        sel1.setIndex(4, 8);
        vec.slice(sel1, 5);
    }
    EXPECT_EQ(vec.getVectorType(), VectorType::DICTIONARY_VECTOR);
    EXPECT_EQ(vec.getValue(0).getNumericValue<int32_t>(),  30);
    EXPECT_EQ(vec.getValue(1).getNumericValue<int32_t>(),  40);
    EXPECT_EQ(vec.getValue(2).getNumericValue<int32_t>(),  50);
    EXPECT_EQ(vec.getValue(3).getNumericValue<int32_t>(),  70);
    EXPECT_EQ(vec.getValue(4).getNumericValue<int32_t>(),  80);
    std::cout << vec.toString(5) << std::endl;

    {
        SelectionVector sel2(3);
        sel2.setIndex(0, 2);
        sel2.setIndex(1, 3);
        sel2.setIndex(2, 4);
        vec.slice(sel2, 3);
    }
    std::cout << vec.toString(3) << std::endl;
    EXPECT_EQ(vec.getVectorType(), VectorType::DICTIONARY_VECTOR);
    EXPECT_EQ(vec.getValue(0).getNumericValue<int32_t>(),  50);
    EXPECT_EQ(vec.getValue(1).getNumericValue<int32_t>(),  70);
    EXPECT_EQ(vec.getValue(2).getNumericValue<int32_t>(),  80);


}

// ---------- Test value setting and retrieval ----------

TEST(VectorTest, GetSetValuesInteger) {
    Vector vec(ConstantType::INTEGER);
    vec.setValue(5, Value(77));
    EXPECT_EQ(vec.getValue(5), Value(77));
}

TEST(VectorTest, GetSetValuesString) {
    Vector vec(ConstantType::STRING);
    vec.setValue(0, Value("hello"));
    EXPECT_EQ(vec.getValue(0), Value("hello"));
}

// ---------- Test flattening ----------

TEST(VectorTest, NormalifyConstantVector) {
    Vector vec(Value(42));
    vec.normalify(3);
    EXPECT_EQ(vec.getVectorType(), VectorType::FLAT_VECTOR);
    for (idx_t i = 0; i < 3; ++i) {
        EXPECT_EQ(vec.getValue(i), Value(42));
    }
}


TEST(VectorTest, NormalifyStringConstantVector) {
    Vector vec(Value("MIAO"));
    vec.normalify(3);
    EXPECT_EQ(vec.getVectorType(), VectorType::FLAT_VECTOR);
    for (idx_t i = 0; i < 3; ++i) {
        EXPECT_EQ(vec.getValue(i), Value("MIAO"));
    }
}

TEST(VectorTest, NormalifyDictionaryVector) {
    // Create a flat vector with 5 values
    Vector baseVec(ConstantType::INTEGER, 5);
    for (idx_t i = 0; i < 5; ++i) {
        baseVec.setValue(i, Value(int32_t(i * 10)));
    }

    // Create selection vector selecting indexes [2, 0]
    SelectionVector sel(2);
    sel.setIndex(0, 2);
    sel.setIndex(1, 3);

    // Create dictionary vector from flat + selection
    Vector dictVec(baseVec, sel, 2);
    EXPECT_EQ(dictVec.getVectorType(), VectorType::DICTIONARY_VECTOR);

    // Normalify the dictionary vector
    dictVec.normalify(2);

    EXPECT_EQ(dictVec.getVectorType(), VectorType::FLAT_VECTOR);
    EXPECT_EQ(dictVec.getValue(0), Value(20));
    EXPECT_EQ(dictVec.getValue(1), Value(30));
}

TEST(VectorTest, NormalifySequenceVector) {
    Vector vec(ConstantType::BIGINT);
    vec.sequence(10, 3);
    vec.normalify(4);

    EXPECT_EQ(vec.getVectorType(), VectorType::FLAT_VECTOR);
    EXPECT_EQ(vec.getValue(0), Value((int64_t)10));
    EXPECT_EQ(vec.getValue(1), Value((int64_t)13));
    EXPECT_EQ(vec.getValue(2), Value((int64_t)16));
    EXPECT_EQ(vec.getValue(3), Value((int64_t)19));
}

TEST(VectorTest, NormalifyCircularSequenceVector) {
    Vector vec(ConstantType::BIGINT);
    vec.sequence(10, 0, 1,20); // 10 11 12 13 14 15 16 17 18 19 20 10 11 12 .....
    std::cout << vec.toString(20) << std::endl;
    vec.normalify(20);
    std::cout << vec.toString(20) << std::endl;

    EXPECT_EQ(vec.getVectorType(), VectorType::FLAT_VECTOR);
    EXPECT_EQ(vec.getValue(0), Value((int64_t)10));
    EXPECT_EQ(vec.getValue(1), Value((int64_t)11));
    EXPECT_EQ(vec.getValue(2), Value((int64_t)12));
    EXPECT_EQ(vec.getValue(10), Value((int64_t)20));
    EXPECT_EQ(vec.getValue(11), Value((int64_t)10));
}

TEST(VectorTest, NormalifyCircularSequenceVectorWithOffset) {
    Vector vec(ConstantType::BIGINT);
    vec.sequence(10, 10, 1,20); // 20 10 11 12 13 ...
    std::cout << vec.toString(15) << std::endl;
    vec.normalify(15);
    std::cout << vec.toString(15) << std::endl;

    EXPECT_EQ(vec.getVectorType(), VectorType::FLAT_VECTOR);
    EXPECT_EQ(vec.getValue(0), Value((int64_t)20));
    EXPECT_EQ(vec.getValue(1), Value((int64_t)10));
    EXPECT_EQ(vec.getValue(2), Value((int64_t)11));
    EXPECT_EQ(vec.getValue(12), Value((int64_t)10));
}

TEST(VectorTest, NormalifySequenceWithSelection) {
    Vector vec(ConstantType::BIGINT);
    vec.sequence(100, 5);

    SelectionVector sel(3);
    sel.setIndex(0, 0);
    sel.setIndex(1, 2);
    sel.setIndex(2, 4);
    vec.normalify(sel, 3);
    // std::cout << vec.toString(5) << std::endl;

    EXPECT_EQ(vec.getVectorType(), VectorType::FLAT_VECTOR);
    auto vv = vec.getValue(0);
    EXPECT_EQ(vec.getValue(sel.getIndex(0)), Value((int64_t)100));   // 100 + 5*0
    EXPECT_EQ(vec.getValue(sel.getIndex(1)), Value((int64_t)110));   // 100 + 5*2
    EXPECT_EQ(vec.getValue(sel.getIndex(2)), Value((int64_t)120));   // 100 + 5*4
}

TEST(VectorTest, SliceAndNormalifyCircularSequenceWithOffset) {
    Vector vec(ConstantType::BIGINT);
    vec.sequence(-3, 5, 3, -1);
    // -2 -1 -1 -1 -3 -3 -3 -2 -2 -2

    SelectionVector sel(4);
    sel.setIndex(0, 0);
    sel.setIndex(1, 1);
    sel.setIndex(2, 4);
    sel.setIndex(3, 7);
    std::cout << vec.toString(10) << std::endl;
    vec.slice(sel, 4);
    std::cout << vec.toString(4) << std::endl;
    vec.normalify( 4);
    std::cout << vec.toString(4) << std::endl;


    EXPECT_EQ(vec.getVectorType(), VectorType::FLAT_VECTOR);
    EXPECT_EQ(vec.getValue(0), Value((int64_t)-2));
    EXPECT_EQ(vec.getValue(1), Value((int64_t)-1));
    EXPECT_EQ(vec.getValue(2), Value((int64_t)-3));
    EXPECT_EQ(vec.getValue(3), Value((int64_t)-2));
}

TEST(VectorTest, OrrifyFlatVector) {
    Vector vec(ConstantType::INTEGER, 10);
    for (idx_t i = 0; i < 3; ++i) {
        vec.setValue(i, Value(int32_t(i)));
    }

    VectorData vdata;
    vec.orrify(3, vdata);
    EXPECT_NE(vdata.data_, nullptr);
    EXPECT_NE(vdata.sel_, nullptr);
    // Expected fail copy not implemented
    for (idx_t i = 0; i < 3; ++i) {
        EXPECT_EQ( ((int32_t*)vdata.data_)[i], i);
    }
}

TEST(VectorTest, OrrifyConstVector) {

    Vector vec(Value((int32_t)100));

    VectorData vdata;
    vec.orrify(10, vdata);

    EXPECT_NE(vdata.data_, nullptr);
    EXPECT_NE(vdata.sel_, nullptr);
    auto data = vdata.data_;
    auto sel = vdata.sel_->getData();
    for (idx_t i = 0; i < 3; ++i) {
        EXPECT_EQ(((int32_t*)data)[sel[i]], 100);
    }
}

TEST(VectorTest, OrrifyDictionaryVector) {
    // Create base vector with 3 values
    Vector baseVec(ConstantType::INTEGER);
    baseVec.setValue(0, Value(100));
    baseVec.setValue(1, Value(200));
    baseVec.setValue(2, Value(300));

    // Create selection vector: [2, 0]
    SelectionVector sel(2);
    sel.setIndex(0, 2);
    sel.setIndex(1, 0);

    // Create dictionary vector
    Vector dictVec(baseVec, sel, 2);

    // Call orrify
    VectorData vdata;
    dictVec.orrify(2, vdata);

    ASSERT_NE(vdata.data_, nullptr);
    ASSERT_NE(vdata.sel_, nullptr);
    EXPECT_EQ(vdata.sel_->getIndex(0), 2);
    EXPECT_EQ(vdata.sel_->getIndex(1), 0);

    const int32_t* data = reinterpret_cast<const int32_t*>(vdata.data_);
    EXPECT_EQ(data[2], 300);
    EXPECT_EQ(data[0], 100);
}

TEST(VectorTest, OrrifyFlatVectorContents) {
    Vector vec(ConstantType::INTEGER);
    vec.setValue(0, Value(11));
    vec.setValue(1, Value(22));
    vec.setValue(2, Value(33));

    VectorData vdata;
    vec.orrify(3, vdata);

    ASSERT_NE(vdata.data_, nullptr);
    ASSERT_NE(vdata.sel_, nullptr);
    EXPECT_EQ(((int32_t*)vdata.data_)[vdata.sel_->getIndex(0)], 11);
    EXPECT_EQ(((int32_t*)vdata.data_)[vdata.sel_->getIndex(1)], 22);
    EXPECT_EQ(((int32_t*)vdata.data_)[vdata.sel_->getIndex(2)], 33);
}

TEST(VectorTest, OrrifyDictionaryWithSequenceChild) {
    // Create a SEQUENCE_VECTOR as the child
    Vector seqVec(ConstantType::BIGINT);
    seqVec.sequence(100, 10);  // Values: 100, 110, 120, ...

    // Create a selection vector referencing indices in the sequence
    SelectionVector sel(3);
    sel.setIndex(0, 0);  // 100
    sel.setIndex(1, 2);  // 120
    sel.setIndex(2, 4);  // 140

    // Wrap the sequence vector into a dictionary vector
    Vector dictVec(seqVec, sel, 3);
    EXPECT_EQ(dictVec.getVectorType(), VectorType::DICTIONARY_VECTOR);

    // Now orrify the dictionary vector
    VectorData vdata;
    dictVec.orrify(3, vdata);

    // Ensure data and selection vector are not null
    ASSERT_NE(vdata.data_, nullptr);
    ASSERT_NE(vdata.sel_, nullptr);

    const int64_t* data = reinterpret_cast<const int64_t*>(vdata.data_);
    const sel_t* indices = vdata.sel_->getData();

    // Validate the orrified values
    EXPECT_EQ(data[indices[0]], 100);  // index 0 of sequence
    EXPECT_EQ(data[indices[1]], 120);  // index 2 of sequence
    EXPECT_EQ(data[indices[2]], 140);  // index 4 of sequence
}


TEST(VectorTest, OrrifyConstStringVector) {
    Vector vec(Value("buzz"));

    VectorData vdata;
    vec.orrify(4, vdata);

    ASSERT_NE(vdata.data_, nullptr);
    ASSERT_NE(vdata.sel_, nullptr);
    auto sel = vdata.sel_->getData();
    auto data = (string_t*)vdata.data_;
    for (idx_t i = 0; i < 4; ++i) {
        EXPECT_EQ(data[sel[i]].getString(), "buzz");
    }
}


// ---------- Test auxiliary data ----------

TEST(VectorTest, StringVectorHeapReference) {
    Vector vec(ConstantType::STRING);
    string_t s = StringVector::addString(vec, "test");
    EXPECT_EQ(std::string(s.getDataUnsafe()), "test");
}

// ---------- Test resize ----------

TEST(VectorTest, ResizeData) {
    Vector vec(ConstantType::INTEGER );
    vec.setValue(0, Value(10));
    vec.resize(1, 3);
    vec.setValue(1, Value(20));
    EXPECT_EQ(vec.getValue(0), Value(10));
    EXPECT_EQ(vec.getValue(1), Value(20));
}
