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

#include <unordered_set>
#include <gtest/gtest.h>
#include "hyperloglog/hyperloglog.h"

using namespace bumblebee_hll;


class HyperLogLogTest : public ::testing::Test {
protected:
    void SetUp() override {
        hll = hll_create();
        ASSERT_NE(hll, nullptr);
    }

    void TearDown() override {
        hll_destroy(hll);
    }

    robj* hll;
};

TEST_F(HyperLogLogTest, CreateDestroy) {
    robj* temp = hll_create();
    EXPECT_NE(temp, nullptr);
    hll_destroy(temp);
}

TEST_F(HyperLogLogTest, AddElementChangesCardinality) {
    const char* element1 = "hello";
    int result = hll_add(hll, (unsigned char*)element1, strlen(element1));
    EXPECT_EQ(result, 1);  // Should be 1 (cardinality change)
    const char* element2 = "hello";
    result = hll_add(hll, (unsigned char*)element2, strlen(element2));
    EXPECT_EQ(result, 0);  // Should be 0 (cardinality not change)
}

TEST_F(HyperLogLogTest, CountAfterAdd) {
    const char* element = "world";
    hll_add(hll, (unsigned char*)element, strlen(element));
    size_t count = 0;
    int result = hll_count(hll, &count);
    EXPECT_EQ(result, HLL_C_OK);
    EXPECT_EQ(count, 1u);
}

TEST_F(HyperLogLogTest, MergeTwoDistinctHLLs) {
    robj* hll1 = hll_create();
    robj* hll2 = hll_create();
    ASSERT_NE(hll1, nullptr);
    ASSERT_NE(hll2, nullptr);

    const char* e1 = "A";
    const char* e2 = "B";
    const char* e3 = "C";
    hll_add(hll1, (unsigned char*)e1, strlen(e1));
    hll_add(hll2, (unsigned char*)e2, strlen(e2));
    hll_add(hll2, (unsigned char*)e3, strlen(e2));

    robj* merged = hll_merge((robj*[]){hll1, hll2}, 2);
    ASSERT_NE(merged, nullptr);

    size_t count = 0;
    EXPECT_EQ(hll_count(merged, &count), HLL_C_OK);
    EXPECT_EQ(count, 3u);

    hll_destroy(hll1);
    hll_destroy(hll2);
    hll_destroy(merged);
}

TEST_F(HyperLogLogTest, EstimateDistinctCountWithDuplicates) {
    // Input with some duplicates
    const char* elements[] = {
        "apple", "banana", "apple", "orange", "grape",
        "banana", "peach", "melon", "kiwi", "peach"
    };

    // Add all elements to HLL
    for (const char* elem : elements) {
        int res = hll_add(hll, (unsigned char*)elem, strlen(elem));
        EXPECT_GE(res, HLL_C_OK);  // Add should not fail
    }

    // Get estimated cardinality
    size_t estimated = 0;
    int count_result = hll_count(hll, &estimated);
    EXPECT_EQ(count_result, HLL_C_OK);

    // Find true distinct elements
    std::unordered_set<std::string> result(elements, elements + sizeof(elements) / sizeof(elements[0]));
    size_t true_distinct = result.size();

    // Allow ~20% error margin
    double lower_bound = true_distinct * 0.8;
    double upper_bound = true_distinct * 1.2;

    EXPECT_GE(estimated, lower_bound);
    EXPECT_LE(estimated, upper_bound);
}
