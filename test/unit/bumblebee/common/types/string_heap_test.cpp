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

#include "bumblebee/common/Constants.hpp"
#include "bumblebee/common/types/StringHeap.hpp"
using namespace bumblebee;

class StringHeapTest : public ::testing::Test {
protected:
    static constexpr idx_t chunkSize = MINIMUM_HEAP_SIZE;
    StringHeap heap;
};

int compareStringT(string_t& s1, const char* s2 ) {
    return strncmp(s1.c_str(), s2, s1.length());
}

TEST_F(StringHeapTest, AddCStringWithLength) {
    const char* data = "HelloHelloHello";
    string_t result = heap.addString(data, strlen(data));
    EXPECT_EQ(compareStringT(result, "HelloHelloHello"), 0 );
}

TEST_F(StringHeapTest, AddCStringWithoutLength) {
    const char* data = "WorldWorldWorldWorldWorld";
    string_t result = heap.addString(data);
    EXPECT_EQ(compareStringT(result, "WorldWorldWorldWorldWorld"), 0 );
}

TEST_F(StringHeapTest, AddStdString) {
    std::string data = "MiaoMiaoMiaoMiaoMiaoMiao";
    string_t result = heap.addString(data);
    EXPECT_EQ(compareStringT(result, "MiaoMiaoMiaoMiaoMiaoMiao"), 0 );
}

TEST_F(StringHeapTest, AddStringT) {
    string_t data = "BumbleBee";
    string_t result = heap.addString(data);
    EXPECT_EQ(compareStringT(result, "BumbleBee"), 0 );
}


TEST_F(StringHeapTest, AddEmptyString) {
    string_t result = heap.addEmptyString(200);
    EXPECT_EQ(result.length(), 200 );
    EXPECT_EQ(compareStringT(result, ""), 0 );
}

TEST_F(StringHeapTest, DestroyDoesNotCrash) {
    heap.addString("Before Destroyyy");
    heap.destroy();
    string_t result = heap.addString("After Destroyy" );
    std::cout <<result.c_str()<<std::endl;
    EXPECT_EQ(compareStringT(result, "After Destroyy"), 0 );
}

TEST_F(StringHeapTest, SingleChunkAllocation) {
    // Insert a  string that fits in a single chunk (note insert not inlined strings)
    auto s1 = heap.addEmptyString(100);
    s1.getDataWriteable()[0] = 'A';
    auto s2 = heap.addEmptyString(200);
    auto s3 = heap.addEmptyString(300);
    auto s4 = heap.addEmptyString(chunkSize - 700);
    // All of them should be in the same chunk (no overflow)
    auto base = s1.c_str();
    EXPECT_EQ(s2.c_str(), base + 101); // +1 for null termination allocation
    EXPECT_EQ(s3.c_str(), base + 101 + 201);
    EXPECT_EQ(s4.c_str(), base + 101 + 201 + 301);

    // This one causes overflow and allocates a new chunk
    auto s5 = heap.addEmptyString(101);
    EXPECT_NE( s5.c_str(), base + 101 + 201 + 301+(chunkSize - 700 +1) );
    // check s1 first chunk still exist
    EXPECT_EQ(s1.c_str()[0], 'A');
    heap.destroy();
}