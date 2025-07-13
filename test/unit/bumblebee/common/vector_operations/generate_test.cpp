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

using namespace bumblebee;

TEST(VectorOperationsGenerateSequenceTest, BasicSequenceGeneration) {
    const idx_t count = 5;
    Vector result(ConstantType::INTEGER, count);
    VectorOperations::generateSequence(result, count, 10, 2);
    int32_t* data = FlatVector::getData<int32_t>(result);
    for (idx_t i = 0; i < count; ++i) {
        ASSERT_EQ(data[i], 10 + i * 2);
    }
}

TEST(VectorOperationsGenerateSequenceTest, NegativeIncrementSequence) {
    const idx_t count = 4;
    Vector result(ConstantType::INTEGER, count);
    VectorOperations::generateSequence(result, count, 20, -3);
    int32_t* data = FlatVector::getData<int32_t>(result);
    for (idx_t i = 0; i < count; ++i) {
        ASSERT_EQ(data[i], 20 - i * 3);
    }
}

TEST(VectorOperationsGenerateSequenceTest, SequenceWithSelectionVector) {
    const idx_t count = 3;
    Vector result(ConstantType::INTEGER, 5); // allocate more than needed
    SelectionVector sel(count);
    sel.setIndex(0, 2);
    sel.setIndex(1, 4);
    sel.setIndex(2, 1);

    VectorOperations::generateSequence(result, count, sel, 100, 1);
    auto* data = FlatVector::getData<int32_t>(result);
    std::cout << result.toString(5) << std::endl;

    ASSERT_EQ(data[1], 100 + 1);
    ASSERT_EQ(data[2], 100 + 2);
    ASSERT_EQ(data[4], 100 + 4);
}
