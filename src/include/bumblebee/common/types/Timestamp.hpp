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
#include <cstdint>

#include "bumblebee/common/NumericUtils.hpp"
#include "bumblebee/common/TypeDefs.hpp"

namespace bumblebee{

struct timestamp_struct {
	int32_t year;
	int8_t month;
	int8_t day;
	int8_t hour;
	int8_t min;
	int8_t sec;
	int16_t msec;
};
// The Timestamp class is a static class that holds helper functions for the Timestamp
// type.
class Timestamp {
public:
	static constexpr const int32_t DAYS_PER_WEEK = 7;
	static constexpr const int64_t DAYS_PER_MONTH = 30;

	static constexpr const int64_t MSECS_PER_SEC = 1000;
	static constexpr const int32_t SECS_PER_MINUTE = 60;
	static constexpr const int32_t MINS_PER_HOUR = 60;
	static constexpr const int32_t HOURS_PER_DAY = 24;
	static constexpr const int32_t SECS_PER_HOUR = SECS_PER_MINUTE * MINS_PER_HOUR;
	static constexpr const int32_t SECS_PER_DAY = SECS_PER_HOUR * HOURS_PER_DAY;
	static constexpr const int32_t SECS_PER_WEEK = SECS_PER_DAY * DAYS_PER_WEEK;

	static constexpr const int64_t MICROS_PER_MSEC = 1000;
	static constexpr const int64_t MICROS_PER_SEC = MICROS_PER_MSEC * MSECS_PER_SEC;
	static constexpr const int64_t MICROS_PER_MINUTE = MICROS_PER_SEC * SECS_PER_MINUTE;
	static constexpr const int64_t MICROS_PER_HOUR = MICROS_PER_MINUTE * MINS_PER_HOUR;
	static constexpr const int64_t MICROS_PER_DAY = MICROS_PER_HOUR * HOURS_PER_DAY;
	static constexpr const int64_t MICROS_PER_WEEK = MICROS_PER_DAY * DAYS_PER_WEEK;
	static constexpr const int64_t MICROS_PER_MONTH = MICROS_PER_DAY * DAYS_PER_MONTH;

public:
	static int64_t fromEpochMs(int64_t ms);
	static int64_t fromEpochNano(int64_t ms);
	static int64_t getTime(int64_t timestamp);
	static int32_t getDate(int64_t timestamp);
	static int64_t fromEpochMicroSeconds(int64_t micros);
	static int64_t getEpochNanoSeconds(timestamp_t timestamp);

	static void convert(int64_t dtime, int32_t &hour, int32_t &min, int32_t &sec, int32_t &micros);
	static void convert(int64_t timestamp, int32_t &out_date, int64_t &out_time);
	static int64_t fromDatetime(date_t date);

	static idx_t length(int32_t time[], char micro_buffer[]);
	static int32_t formatMicros(uint32_t microseconds, char micro_buffer[]);
	static void format(char *data, idx_t length, int32_t time[], char micro_buffer[]);
	static void formatTwoDigits(char *ptr, int32_t value);
};

}
