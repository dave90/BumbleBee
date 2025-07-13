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
#include "bumblebee/common/vector_operations/VectorOperations.h"
#include "bumblebee/common/types/Value.h"
#include "bumblebee/common/Assert.h"

using namespace bumblebee;

class VectorOperationsCopyTest : public ::testing::Test {
protected:
    static constexpr idx_t TEST_COUNT = 100;

    void SetUp() override {
        // Initialize source and target vectors of INTEGER type
        source = std::unique_ptr<Vector> (new Vector(ConstantType::INTEGER, TEST_COUNT));
        target = std::unique_ptr<Vector> (new Vector(ConstantType::INTEGER, TEST_COUNT));

        auto data = FlatVector::getData<int32_t>(*source);
        for (idx_t i = 0; i < TEST_COUNT; ++i) {
            data[i] = i * 10;
        }
    }

    std::unique_ptr<Vector> source;
    std::unique_ptr<Vector> target;
};

TEST_F(VectorOperationsCopyTest, CopyFlatVector) {
    VectorOperations::copy(*source, *target, TEST_COUNT, 0, 0);
    int32_t *data = FlatVector::getData<int32_t>(*target);
    for (idx_t i = 0; i < TEST_COUNT; ++i) {
        ASSERT_EQ(data[i], i * 10);
    }
}

TEST_F(VectorOperationsCopyTest, CopyWithOffset) {
    auto offset = 10;
    VectorOperations::copy(*source, *target, TEST_COUNT, offset, 0);
    int32_t *data = FlatVector::getData<int32_t>(*target);
    for (idx_t i = 0; i < TEST_COUNT - offset; ++i) {
        ASSERT_EQ(data[i], (i + offset) * 10);
    }
}

TEST_F(VectorOperationsCopyTest, CopyWithSelectionVector) {
    SelectionVector sel(5);
    sel.setIndex(0, 4);
    sel.setIndex(1, 3);
    sel.setIndex(2, 2);
    sel.setIndex(3, 1);
    sel.setIndex(4, 0);

    VectorOperations::copy(*source, *target, sel, 5, 0, 0);

    auto data = FlatVector::getData<int32_t>(*target);
    for (idx_t i = 0; i < 5; ++i) {
        std::cout << data[i] << " ";
        ASSERT_EQ(data[i], (4 - i) * 10);
    }
    std::cout << std::endl;
}

TEST_F(VectorOperationsCopyTest, CopyToConstantVector) {
    Vector constantTarget(Value(999));

    VectorOperations::copy(*source, constantTarget, FlatVector::INCREMENTAL_SELECTION_VECTOR, 1, 0, 0);
    constantTarget.setVectorType(VectorType::FLAT_VECTOR);
    int32_t *data = FlatVector::getData<int32_t>(constantTarget);
    ASSERT_EQ(data[0], 0);
}


TEST_F(VectorOperationsCopyTest, CopyFromConstantVector) {
    Vector constantSource(Value(42));

    VectorOperations::copy(constantSource, *target, ConstantVector::ZERO_SELECTION_VECTOR, TEST_COUNT, 0, 0);

    auto data = FlatVector::getData<int32_t>(*target);
    for (idx_t i = 0; i < TEST_COUNT; ++i) {
        ASSERT_EQ(data[i], 42);
    }
}

TEST_F(VectorOperationsCopyTest, CopyFromSequenceVector) {
    Vector sequence(ConstantType::INTEGER, TEST_COUNT);
    sequence.sequence(5, 3);  // values: 5, 8, 11, 14, 17

    VectorOperations::copy(sequence, *target, TEST_COUNT, 0, 0);

    auto data = FlatVector::getData<int32_t>(*target);
    for (idx_t i = 0; i < TEST_COUNT; ++i) {
        ASSERT_EQ(data[i], 5 + i * 3);
    }
}

TEST_F(VectorOperationsCopyTest, CopyFromDictionaryVector) {
    Vector dict_vector(ConstantType::INTEGER, TEST_COUNT);
    auto data = FlatVector::getData<int32_t>(dict_vector);
    for (idx_t i = 0; i < TEST_COUNT; ++i) {
        data[i] = i + 100;
    }
    // reverse the index in the selection
    // 0 -> TEST_COUNT - 1
    // 1 -> TEST_COUNT - 2
    // ...
    // TEST_COUNT = 0
    SelectionVector dict_sel(TEST_COUNT);
    for (idx_t i = 0; i < TEST_COUNT; ++i) dict_sel.setIndex(i, TEST_COUNT - 1 - i);
    dict_vector.slice(dict_sel,TEST_COUNT);

    VectorOperations::copy(dict_vector, *target, TEST_COUNT, 0, 0);
    ASSERT_EQ(target->getVectorType(), VectorType::FLAT_VECTOR);

    auto target_data = FlatVector::getData<int32_t>(*target);
    for (idx_t i = 0; i < TEST_COUNT; ++i) {
        // value should be the reverse of dict_vector
        ASSERT_EQ(target_data[i], 100 + (TEST_COUNT - 1 - i));
    }
}
