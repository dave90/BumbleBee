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
#include "bumblebee/common/types/Timestamp.hpp"

#include "bumblebee/common/ErrorHandler.hpp"

namespace bumblebee{
int64_t Timestamp::fromEpochMs(int64_t ms) {
    int64_t result = ms * MICROS_PER_MSEC;
    return result;
}

int64_t Timestamp::getTime(int64_t timestamp) {
    auto days = Timestamp::getDate(timestamp);
    return timestamp - (int64_t(days) * int64_t(MICROS_PER_DAY));

}

int32_t Timestamp::getDate(int64_t timestamp) {
    return (timestamp + (timestamp < 0)) / MICROS_PER_DAY - (timestamp < 0);
}

int64_t Timestamp::fromEpochMicroSeconds(int64_t micros) {
    return micros;
}

void Timestamp::convert(int64_t dtime, int32_t &hour, int32_t &min, int32_t &sec, int32_t &micros) {
    int64_t time = dtime;
    hour = int32_t(time / MICROS_PER_HOUR);
    time -= int64_t(hour) * MICROS_PER_HOUR;
    min = int32_t(time / MICROS_PER_MINUTE);
    time -= int64_t(min) * MICROS_PER_MINUTE;
    sec = int32_t(time / MICROS_PER_SEC);
    time -= int64_t(sec) * MICROS_PER_SEC;
    micros = int32_t(time);
}

void Timestamp::convert(int64_t timestamp, int32_t &out_date, int64_t &out_time) {
    out_date = getDate(timestamp);
    out_time = timestamp - (int64_t(out_date) * int64_t(MICROS_PER_DAY));
}

int64_t Timestamp::fromDatetime(date_t date) {
    return date * MICROS_PER_DAY;
}

idx_t Timestamp::length(int32_t time[], char micro_buffer[]) {
    // format is HH:MM:DD.MS
    // microseconds come after the time with a period separator
    idx_t length;
    if (time[3] == 0) {
        // no microseconds
        // format is HH:MM:DD
        length = 8;
    } else {
        length = 15;
        // for microseconds, we truncate any trailing zeros (i.e. "90000" becomes ".9")
        // first write the microseconds to the microsecond buffer
        // we write backwards and pad with zeros to the left
        // now we figure out how many digits we need to include by looking backwards
        // and checking how many zeros we encounter
        length -= formatMicros(time[3], micro_buffer);
    }
    return length;
}

int32_t Timestamp::formatMicros(uint32_t microseconds, char micro_buffer[]) {
    char *endptr = micro_buffer + 6;
    endptr = NumericHelper::formatUnsigned<uint32_t>(microseconds, endptr);
    while (endptr > micro_buffer) {
        *--endptr = '0';
    }
    idx_t trailing_zeros = 0;
    for (idx_t i = 5; i > 0; i--) {
        if (micro_buffer[i] != '0') {
            break;
        }
        trailing_zeros++;
    }
    return trailing_zeros;
}

void Timestamp::format(char *data, idx_t length, int32_t time[], char micro_buffer[]) {
    // first write hour, month and day
    auto ptr = data;
    ptr[2] = ':';
    ptr[5] = ':';
    for (int i = 0; i <= 2; i++) {
        formatTwoDigits(ptr, time[i]);
        ptr += 3;
    }
    if (length > 8) {
        // write the micro seconds at the end
        data[8] = '.';
        memcpy(data + 9, micro_buffer, length - 9);
    }
}

void Timestamp::formatTwoDigits(char *ptr, int32_t value) {
    BB_ASSERT(value >= 0 && value <= 99);
    if (value < 10) {
        ptr[0] = '0';
        ptr[1] = '0' + value;
    } else {
        auto index = static_cast<unsigned>(value * 2);
        ptr[0] = NumericHelper::digits[index];
        ptr[1] = NumericHelper::digits[index + 1];
    }
}
}
