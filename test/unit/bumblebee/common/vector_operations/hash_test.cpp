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
#include "bumblebee/common/vector_operations/VectorOperations.hpp"
#include "bumblebee/common/Assert.hpp"
#include "bumblebee/common/Hash.hpp"

using namespace bumblebee;

class VectorOperationsHashTest : public ::testing::Test {
protected:
    static constexpr idx_t TEST_COUNT = 4;

    void SetUp() override {
        input = std::make_unique<Vector>(ConstantType::INTEGER, TEST_COUNT);
        hashes = std::make_unique<Vector>(ConstantType::UBIGINT, TEST_COUNT);

        auto data = FlatVector::getData<int32_t>(*input);
        data[0] = 42;
        data[1] = 17;
        data[2] = -10;
        data[3] = -23;
    }

    std::unique_ptr<Vector> input;
    std::unique_ptr<Vector> hashes;

};

TEST_F(VectorOperationsHashTest, HashBasicFlatVector) {
    VectorOperations::hash(*input, *hashes, TEST_COUNT);
    auto hash_data = FlatVector::getData<hash_t>(*hashes);

    int32_t* input_data = FlatVector::getData<int32_t>(*input);
    for (idx_t i = 0; i < TEST_COUNT; ++i) {
        ASSERT_EQ(hash_data[i], Hash<int32_t>(input_data[i]));
    }
}

TEST_F(VectorOperationsHashTest, HashWithSelectionVector) {
    SelectionVector sel(TEST_COUNT);
    sel.setIndex(0, 3);
    sel.setIndex(1, 1);
    sel.setIndex(2, 2);

    VectorOperations::hash(*input, *hashes, sel, 3);
    auto hash_data = FlatVector::getData<hash_t>(*hashes);
    auto  input_data = FlatVector::getData<int32_t>(*input);
    // std::cout << hashes->toString(10) << std::endl;

    for (idx_t i = 0; i < 3; ++i) {
        idx_t idx = sel.getIndex(i);
        EXPECT_EQ( hash_data[idx], Hash<int32_t>(input_data[idx]) );
    }
}

TEST_F(VectorOperationsHashTest, CombineHashWithFlatVectors) {
    Vector extra(ConstantType::INTEGER, TEST_COUNT);
    auto extra_data = FlatVector::getData<int32_t>(extra);
    for (idx_t i = 0; i < TEST_COUNT; ++i) {
        extra_data[i] = i + 100;
    }

    VectorOperations::hash(*input, *hashes, TEST_COUNT);
    VectorOperations::combineHash(*hashes, extra, TEST_COUNT);

    auto hash_data = FlatVector::getData<hash_t>(*hashes);
    auto input_data = FlatVector::getData<int32_t>(*input);
    for (idx_t i = 0; i < TEST_COUNT; ++i) {
        auto h1 = Hash<int32_t>(input_data[i]);
        auto h2 = Hash<int32_t>(extra_data[i]);
        ASSERT_EQ(hash_data[i], (h1 * UINT64_C(0xbf58476d1ce4e5b9)) ^ h2);
    }
}

TEST(VectorOperationsHashConstantTest, HashConstantVector) {
    Vector constantInput(Value(123));
    Vector hashResult(ConstantType::UBIGINT);

    VectorOperations::hash(constantInput, hashResult, 1);

    auto input_val = *ConstantVector::getData<int32_t>(constantInput);
    auto hash_val = *ConstantVector::getData<hash_t>(hashResult);
    EXPECT_EQ(hashResult.getVectorType(), VectorType::CONSTANT_VECTOR);

    ASSERT_EQ(hash_val, Hash<int32_t>(input_val));
}


TEST(VectorOperationsHashConstantTest, HashNumericDifferentType) {
    // hash of different numeric type but same value should be the same

    Value val(123);
    Vector constantInput(val);
    Vector expectedHashResult(ConstantType::UBIGINT);

    VectorOperations::hash(constantInput, expectedHashResult, 1);

    auto expected_hash_val = *ConstantVector::getData<hash_t>(expectedHashResult);

    vector<ConstantType> types = {SMALLINT, INTEGER, USMALLINT, UINTEGER, UBIGINT};
    for (auto type : types) {
        Vector vec(val.cast(type));
        EXPECT_EQ(vec.getType(), type);
        Vector hashResult(ConstantType::UBIGINT);
        VectorOperations::hash(vec, hashResult, 1);
        auto hash_val = *ConstantVector::getData<hash_t>(hashResult);
        EXPECT_EQ(expected_hash_val, hash_val);
    }

}