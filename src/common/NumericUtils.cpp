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
#include "../include/bumblebee/common/NumericUtils.hpp"

namespace bumblebee{

const char NumericHelper::digits[] =
    "0001020304050607080910111213141516171819"
    "2021222324252627282930313233343536373839"
    "4041424344454647484950515253545556575859"
    "6061626364656667686970717273747576777879"
    "8081828384858687888990919293949596979899";

const int64_t NumericHelper::POWERS_OF_TEN[] {1,
                                              10,
                                              100,
                                              1000,
                                              10000,
                                              100000,
                                              1000000,
                                              10000000,
                                              100000000,
                                              1000000000,
                                              10000000000,
                                              100000000000,
                                              1000000000000,
                                              10000000000000,
                                              100000000000000,
                                              1000000000000000,
                                              10000000000000000,
                                              100000000000000000,
                                              1000000000000000000};

const double NumericHelper::DOUBLE_POWERS_OF_TEN[] {1e0,  1e1,  1e2,  1e3,  1e4,  1e5,  1e6,  1e7,  1e8,  1e9,
                                                    1e10, 1e11, 1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19,
                                                    1e20, 1e21, 1e22, 1e23, 1e24, 1e25, 1e26, 1e27, 1e28, 1e29,
                                                    1e30, 1e31, 1e32, 1e33, 1e34, 1e35, 1e36, 1e37, 1e38, 1e39};


template <>
int NumericHelper::unsignedLength(uint8_t value) {
    int length = 1;
    length += value >= 10;
    length += value >= 100;
    return length;
}

template <>
int NumericHelper::unsignedLength(uint16_t value) {
    int length = 1;
    length += value >= 10;
    length += value >= 100;
    length += value >= 1000;
    length += value >= 10000;
    return length;
}

template <>
int NumericHelper::unsignedLength(uint32_t value) {
    if (value >= 10000) {
        int length = 5;
        length += value >= 100000;
        length += value >= 1000000;
        length += value >= 10000000;
        length += value >= 100000000;
        length += value >= 1000000000;
        return length;
    } else {
        int length = 1;
        length += value >= 10;
        length += value >= 100;
        length += value >= 1000;
        return length;
    }
}

template <>
int NumericHelper::unsignedLength(uint64_t value) {
    if (value >= 10000000000ULL) {
        if (value >= 1000000000000000ULL) {
            int length = 16;
            length += value >= 10000000000000000ULL;
            length += value >= 100000000000000000ULL;
            length += value >= 1000000000000000000ULL;
            length += value >= 10000000000000000000ULL;
            return length;
        } else {
            int length = 11;
            length += value >= 100000000000ULL;
            length += value >= 1000000000000ULL;
            length += value >= 10000000000000ULL;
            length += value >= 100000000000000ULL;
            return length;
        }
    } else {
        if (value >= 100000ULL) {
            int length = 6;
            length += value >= 1000000ULL;
            length += value >= 10000000ULL;
            length += value >= 100000000ULL;
            length += value >= 1000000000ULL;
            return length;
        } else {
            int length = 1;
            length += value >= 10ULL;
            length += value >= 100ULL;
            length += value >= 1000ULL;
            length += value >= 10000ULL;
            return length;
        }
    }
}


}