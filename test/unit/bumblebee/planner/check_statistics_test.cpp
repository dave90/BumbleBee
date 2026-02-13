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

#include "bumblebee/planner/filter/ConstantFilter.hpp"
#include "bumblebee/storage/statistics/NumericStatistics.hpp"
#include "bumblebee/common/types/Date.hpp"

using namespace bumblebee;

// Helper: create NumericStatistics for DATE with given min/max days-since-epoch
static NumericStatistics makeDateStats(int32_t minDays, int32_t maxDays) {
    auto type = LogicalType::createDate();
    return NumericStatistics(type, Value(minDays), Value(maxDays));
}

// Helper: create NumericStatistics for DECIMAL with given min/max scaled values
static NumericStatistics makeDecimalStats(int width, int scale, int64_t minScaled, int64_t maxScaled) {
    auto type = LogicalType::createDecimal(width, scale);
    switch (type.getPhysicalType()) {
        case PhysicalType::SMALLINT:
            return NumericStatistics(type, Value((int16_t)minScaled), Value((int16_t)maxScaled));
        case PhysicalType::INTEGER:
            return NumericStatistics(type, Value((int32_t)minScaled), Value((int32_t)maxScaled));
        case PhysicalType::BIGINT:
        default:
            return NumericStatistics(type, Value(minScaled), Value(maxScaled));
    }
}

// --- DATE tests ---

// 2020-01-01 = day 18262, 2020-12-31 = day 18627
// Filter: col > '2020-06-15' (day 18428) => within range => NO_PRUNING_POSSIBLE
TEST(CheckStatisticsTest, DateGreaterWithinRange) {
    auto stats = makeDateStats(18262, 18627);
    ConstantFilter filter(GREATER, Value("2020-06-15"));
    EXPECT_EQ(filter.checkStatistics(stats), FilterPropagateResult::NO_PRUNING_POSSIBLE);
}

// Filter: col > '2021-06-15' => above max => FILTER_ALWAYS_FALSE
TEST(CheckStatisticsTest, DateGreaterAboveMax) {
    auto stats = makeDateStats(18262, 18627);
    ConstantFilter filter(GREATER, Value("2021-06-15"));
    EXPECT_EQ(filter.checkStatistics(stats), FilterPropagateResult::FILTER_ALWAYS_FALSE);
}

// Filter: col > '2019-01-01' => below min => FILTER_ALWAYS_TRUE
TEST(CheckStatisticsTest, DateGreaterBelowMin) {
    auto stats = makeDateStats(18262, 18627);
    ConstantFilter filter(GREATER, Value("2019-01-01"));
    EXPECT_EQ(filter.checkStatistics(stats), FilterPropagateResult::FILTER_ALWAYS_TRUE);
}

// Filter: col = '2020-01-01' (day 18262) with min==max==18262 => FILTER_ALWAYS_TRUE
TEST(CheckStatisticsTest, DateEqualExactMatch) {
    auto stats = makeDateStats(18262, 18262);
    ConstantFilter filter(EQUAL, Value("2020-01-01"));
    EXPECT_EQ(filter.checkStatistics(stats), FilterPropagateResult::FILTER_ALWAYS_TRUE);
}

// Filter: col < '2019-01-01' => below min => FILTER_ALWAYS_FALSE
TEST(CheckStatisticsTest, DateLessBelowMin) {
    auto stats = makeDateStats(18262, 18627);
    ConstantFilter filter(LESS, Value("2019-01-01"));
    EXPECT_EQ(filter.checkStatistics(stats), FilterPropagateResult::FILTER_ALWAYS_FALSE);
}

// Invalid date string => NO_PRUNING_POSSIBLE (cast fails)
TEST(CheckStatisticsTest, DateInvalidString) {
    auto stats = makeDateStats(18262, 18627);
    ConstantFilter filter(EQUAL, Value("not-a-date"));
    EXPECT_EQ(filter.checkStatistics(stats), FilterPropagateResult::NO_PRUNING_POSSIBLE);
}

// --- DECIMAL tests ---
// Use a constant with a different physical type than the stats to trigger the
// scaling path (simulates real SQL execution where parser constants are small types)

// DECIMAL(9, 2) physical=INTEGER: stats min=1000 (10.00), max=5000 (50.00)
// Filter constant as uint8_t (UTINYINT) to trigger scaling: 30 * 100 = 3000 within range
TEST(CheckStatisticsTest, DecimalGreaterWithinRange) {
    auto stats = makeDecimalStats(9, 2, 1000, 5000);
    ConstantFilter filter(GREATER, Value((uint8_t)30));
    EXPECT_EQ(filter.checkStatistics(stats), FilterPropagateResult::NO_PRUNING_POSSIBLE);
}

// Filter: col > 60 => scaledVal = 6000 > max 5000 => FILTER_ALWAYS_FALSE
TEST(CheckStatisticsTest, DecimalGreaterAboveMax) {
    auto stats = makeDecimalStats(9, 2, 1000, 5000);
    ConstantFilter filter(GREATER, Value((uint8_t)60));
    EXPECT_EQ(filter.checkStatistics(stats), FilterPropagateResult::FILTER_ALWAYS_FALSE);
}

// Filter: col > 5 => scaledVal = 500 < min 1000 => FILTER_ALWAYS_TRUE
TEST(CheckStatisticsTest, DecimalGreaterBelowMin) {
    auto stats = makeDecimalStats(9, 2, 1000, 5000);
    ConstantFilter filter(GREATER, Value((uint8_t)5));
    EXPECT_EQ(filter.checkStatistics(stats), FilterPropagateResult::FILTER_ALWAYS_TRUE);
}

// DECIMAL(4, 1) uses SMALLINT physical type
// stats min=100 (10.0), max=200 (20.0)
// Filter constant as uint8_t (UTINYINT) to trigger scaling: 15 * 10 = 150 within range
TEST(CheckStatisticsTest, DecimalSmallintWithinRange) {
    auto stats = makeDecimalStats(4, 1, 100, 200);
    ConstantFilter filter(LESS, Value((uint8_t)15));
    EXPECT_EQ(filter.checkStatistics(stats), FilterPropagateResult::NO_PRUNING_POSSIBLE);
}

// DECIMAL(18, 3) uses BIGINT physical type
// stats min=1000000 (1000.000), max=9000000 (9000.000)
// Filter constant as int32_t (INTEGER) to trigger scaling: 5000 * 1000 = 5000000 within range
TEST(CheckStatisticsTest, DecimalBigintWithinRange) {
    auto stats = makeDecimalStats(18, 3, 1000000, 9000000);
    ConstantFilter filter(EQUAL, Value((int32_t)5000));
    EXPECT_EQ(filter.checkStatistics(stats), FilterPropagateResult::NO_PRUNING_POSSIBLE);
}

// DECIMAL pre-scaled constant (same physical type as stats) should use standard path
TEST(CheckStatisticsTest, DecimalPreScaledConstant) {
    auto stats = makeDecimalStats(9, 2, 1000, 5000);
    // Pre-scaled: 3000 represents 30.00, same physical type INTEGER as stats
    ConstantFilter filter(GREATER, Value((int32_t)3000));
    EXPECT_EQ(filter.checkStatistics(stats), FilterPropagateResult::NO_PRUNING_POSSIBLE);
}

// --- Standard type tests (regression) ---

// Standard INTEGER stats: ensure existing path still works
TEST(CheckStatisticsTest, StandardIntegerPath) {
    LogicalType intType(LogicalTypeId::INTEGER);
    NumericStatistics stats(intType, Value((int32_t)10), Value((int32_t)100));
    ConstantFilter filter(GREATER, Value((int32_t)50));
    EXPECT_EQ(filter.checkStatistics(stats), FilterPropagateResult::NO_PRUNING_POSSIBLE);
}