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


class Serializer;
class Deserializer;
class Vector;

class ValidityStatistics : public BaseStatistics {
public:
    ValidityStatistics(bool has_null = false, bool has_no_null = true);

    // Whether or not the segment can contain NULL values
    bool hasNull_;
    // Whether or not the segment can contain values that are not null
    bool hasNoNull_;

public:
    void merge(const BaseStatistics &other) override;

    bool isConstant() override;

    std::unique_ptr<BaseStatistics> copy() override;

    static std::unique_ptr<BaseStatistics> combine(const std::unique_ptr<BaseStatistics> &lstats,
                                              const std::unique_ptr<BaseStatistics> &rstats);

    string toString() override;
};

}
