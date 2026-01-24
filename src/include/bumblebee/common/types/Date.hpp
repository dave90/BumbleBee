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
#pragma once
#include "BumbleString.hpp"
#include "Timestamp.hpp"
#include "bumblebee/common/StringUtils.hpp"

namespace bumblebee{



//! The Date class is a static class that holds helper functions for the Date type.
class Date {
public:
	static const string_t MONTH_NAMES[12];
	static const string_t MONTH_NAMES_ABBREVIATED[12];
	static const string_t DAY_NAMES[7];
	static const string_t DAY_NAMES_ABBREVIATED[7];
	static const int32_t NORMAL_DAYS[13];
	static const int32_t CUMULATIVE_DAYS[13];
	static const int32_t LEAP_DAYS[13];
	static const int32_t CUMULATIVE_LEAP_DAYS[13];
	static const int32_t CUMULATIVE_YEAR_DAYS[401];
	static const int8_t MONTH_PER_DAY_OF_YEAR[365];
	static const int8_t LEAP_MONTH_PER_DAY_OF_YEAR[366];

	// min date is 5877642-06-23 (BC) (-2^31)
	constexpr static const int32_t DATE_MIN_YEAR = -5877641;
	constexpr static const int32_t DATE_MIN_MONTH = 6;
	constexpr static const int32_t DATE_MIN_DAY = 23;
	// max date is 5881580-07-11 (2^31)
	constexpr static const int32_t DATE_MAX_YEAR = 5881580;
	constexpr static const int32_t DATE_MAX_MONTH = 7;
	constexpr static const int32_t DATE_MAX_DAY = 11;
	constexpr static const int32_t EPOCH_YEAR = 1970;

	constexpr static const int32_t YEAR_INTERVAL = 400;
	constexpr static const int32_t DAYS_PER_YEAR_INTERVAL = 146097;

public:
	static bool parseDoubleDigit(const char *buf, idx_t len, idx_t &pos, int32_t &result);
	static bool isLeapYear(int32_t year);
	static bool isValid(int32_t year, int32_t month, int32_t day);
	static bool tryFromDate(int32_t year, int32_t month, int32_t day, date_t &result);
	static int64_t epoch(int32_t date);

	static bool tryConvertDate(const char *buf, idx_t len, idx_t &pos, date_t &result, bool strict);

	static void convert(int32_t d, int32_t &year, int32_t &month, int32_t &day);
	static idx_t length(int32_t date[], idx_t &year_length, bool &add_bc);
	static void format(char *data, int32_t date[], idx_t year_length, bool add_bc);
private:
	static void extractYearOffset(int32_t &n, int32_t &year, int32_t &year_offset);
};


}
