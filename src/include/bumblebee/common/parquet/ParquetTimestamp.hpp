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

#include "bumblebee/common/TypeDefs.hpp"
#include "bumblebee/common/types/Date.hpp"
#include "bumblebee/common/types/Timestamp.hpp"

namespace bumblebee{


struct Int96 {
    uint32_t value[3];
};

static constexpr int64_t JULIAN_TO_UNIX_EPOCH_DAYS = 2440588LL;
static constexpr int64_t MILLISECONDS_PER_DAY = 86400000LL;
static constexpr int64_t NANOSECONDS_PER_DAY = MILLISECONDS_PER_DAY * 1000LL * 1000LL;


inline date_t parquetIntToDate(const int32_t &raw_date) {
    return raw_date;
}


inline timestamp_t impalaTimestampToNanoseconds(const Int96 &impala_timestamp) {
    int64_t days_since_epoch = impala_timestamp.value[2] - JULIAN_TO_UNIX_EPOCH_DAYS;
    auto nanoseconds = load<int64_t>((data_ptr_t)impala_timestamp.value);
    return days_since_epoch * NANOSECONDS_PER_DAY + nanoseconds;
}

inline timestamp_t impalaTimestampToTimestamp(const Int96 &raw_ts) {
    auto impala_ns = impalaTimestampToNanoseconds(raw_ts);
    return Timestamp::fromEpochMs(impala_ns / 1000000);
}

inline Int96 timestampToImpalaTimestamp(int64_t &ts) {
    int32_t hour, min, sec, msec;
    Timestamp::convert(Timestamp::getTime(ts), hour, min, sec, msec);
    uint64_t ms_since_midnight = hour * 60 * 60 * 1000 + min * 60 * 1000 + sec * 1000 + msec;
    auto days_since_epoch = Date::epoch(Timestamp::getDate(ts)) / (24 * 60 * 60);
    // first two uint32 in Int96 are nanoseconds since midnights
    // last uint32 is number of days since year 4713 BC ("Julian date")
    Int96 impala_ts;
    store<uint64_t>(ms_since_midnight * 1000000, (data_ptr_t)impala_ts.value);
    impala_ts.value[2] = days_since_epoch + JULIAN_TO_UNIX_EPOCH_DAYS;
    return impala_ts;
}

inline timestamp_t parquetTimestampMicrosToTimestamp(const int64_t &raw_ts) {
    return Timestamp::fromEpochMicroSeconds(raw_ts);
}
inline timestamp_t parquetTimestampMsToTimestamp(const int64_t &raw_ts) {
    return Timestamp::fromEpochMs(raw_ts);
}



}
