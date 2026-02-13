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
#include "BaseStatistics.hpp"

namespace bumblebee{


class StringStatistics : public BaseStatistics {
public:
    constexpr static uint32_t MAX_STRING_MINMAX_SIZE = 8;

public:
    explicit StringStatistics(const LogicalType &type);

    // The minimum value of the segment, potentially truncated
    data_t min_[MAX_STRING_MINMAX_SIZE];
    // The maximum value of the segment, potentially truncated
    data_t max_[MAX_STRING_MINMAX_SIZE];
    // Whether or not the column can contain unicode characters
    bool hasUnicode_;
    // The maximum string length in bytes
    uint32_t maxStringLength_;
    // Whether or not the segment contains any big strings in overflow blocks
    bool hasOverflowStrings_;

public:
    void update(const string_t &value);
    void merge(const BaseStatistics &other) override;

    std::unique_ptr<BaseStatistics> copy() override;

    FilterPropagateResult checkZonemap(Binop comparison_type, const string &value);

    string toString() override;
};


}
