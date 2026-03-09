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
#include "bumblebee/common/types/Decimal.hpp"

namespace bumblebee{

template <class SIGNED, class UNSIGNED>
string templatedDecimalToString(SIGNED value, uint8_t scale) {
    auto len = DecimalToString::decimalLength<SIGNED, UNSIGNED>(value, scale);
    auto data = std::unique_ptr<char[]>(new char[len + 1]);
    DecimalToString::formatDecimal<SIGNED, UNSIGNED>(value, scale, data.get(), len);
    return string(data.get(), len);
}

string Decimal::toString(int16_t value, uint8_t scale) {
    return templatedDecimalToString<int16_t, uint16_t>(value, scale);
}

string Decimal::toString(int32_t value, uint8_t scale) {
    return templatedDecimalToString<int32_t, uint32_t>(value, scale);
}

string Decimal::toString(int64_t value, uint8_t scale) {
    return templatedDecimalToString<int64_t, uint64_t>(value, scale);
}


}