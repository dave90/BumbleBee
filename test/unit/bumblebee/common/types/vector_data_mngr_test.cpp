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
#include "bumblebee/common/types/VectorDataMngr.hpp"
#include "bumblebee/common/TypeDefs.hpp"
#include "bumblebee/common/types/SelectionVector.hpp"
#include "bumblebee/common/types/Vector.hpp"

using namespace bumblebee;


TEST(VectorDataMngrTest, ConstructorWithType) {
    VectorDataMngr mgr(VectorDataMngrType::STANDARD_DATA_MNGR);
    EXPECT_EQ(mgr.getType(), VectorDataMngrType::STANDARD_DATA_MNGR);
}

TEST(VectorDataMngrTest, ConstructorWithSizeAllocatesData) {
    VectorDataMngr mgr(10);
    EXPECT_NE(mgr.getData(), nullptr);
}

TEST(VectorDataMngrTest, SetDataReplacesPointer) {
    auto ptr = std::make_unique<data_t[]>(5);
    ptr[0] = 42;
    VectorDataMngr mgr(VectorDataMngrType::STANDARD_DATA_MNGR);
    mgr.setData(std::move(ptr));
    EXPECT_EQ(mgr.getData()[0], 42);
}

TEST(VectorDataMngrTest, FactoryCreatesStandardVector) {
    auto vec = VectorDataMngr::createStandardVector(PhysicalType::BIGINT, 5);
    EXPECT_EQ(vec->getType(), VectorDataMngrType::STANDARD_DATA_MNGR);
    EXPECT_NE(vec->getData(), nullptr);
}

TEST(VectorDataMngrTest, FactoryCreatesConstantVector) {
    auto vec = VectorDataMngr::createConstantVector(PhysicalType::BIGINT);
    EXPECT_EQ(vec->getType(), VectorDataMngrType::STANDARD_DATA_MNGR);
    EXPECT_NE(vec->getData(), nullptr);
}

TEST(DictionaryDataMngrTest, ConstructorWithSizeInitializesSelection) {
    DictionaryDataMngr mgr(5);
    EXPECT_EQ(mgr.getType(), VectorDataMngrType::DICTIONARY_DATA_MNGR);
    EXPECT_NE(mgr.getSelection().getSelData().get(), nullptr);
    EXPECT_EQ(mgr.getData(), nullptr);
}


TEST(StringDataMngrTest, AddStringReturnsCorrectString) {
    StringDataMngr mgr;
    std::string s = "hello";
    string_t result = mgr.addString(s.c_str(), s.length());
    EXPECT_STREQ(result.c_str(), "hello");
}

TEST(StringDataMngrTest, AddStringWithString_t) {
    StringDataMngr mgr;
    string_t str ("world");
    string_t result = mgr.addString(str);
    EXPECT_STREQ(result.c_str(), "world");
}

TEST(StringDataMngrTest, AddEmptyStringHasCorrectLength) {
    StringDataMngr mgr;
    string_t emptyStr = mgr.addEmptyString(10);
    EXPECT_EQ(emptyStr.size(), 10);
}

TEST(StringDataMngrTest, AddHeapReferenceStoresReference) {
    auto ref =  vector_data_mngr_ptr_t( new StringDataMngr);
    StringDataMngr mgr;
    mgr.addHeapReference(ref);
    // No assertion possible here unless we expose the references_ vector
    SUCCEED();  // Just validate that no exception or crash occurs
}
