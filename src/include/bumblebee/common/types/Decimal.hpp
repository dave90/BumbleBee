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

class Decimal {
public:
    static constexpr uint8_t MAX_WIDTH_INT16 = 4;
    static constexpr uint8_t MAX_WIDTH_INT32 = 9;
    static constexpr uint8_t MAX_WIDTH_INT64 = 18;
    static constexpr uint8_t MAX_WIDTH_INT128 = 38;
    static constexpr uint8_t MAX_WIDTH_DECIMAL = MAX_WIDTH_INT128;

public:
    static string toString(int16_t value, uint8_t scale);
    static string toString(int32_t value, uint8_t scale);
    static string toString(int64_t value, uint8_t scale);
};


}
