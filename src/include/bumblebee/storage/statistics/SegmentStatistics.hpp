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
#include "ValidityStatistics.hpp"

namespace bumblebee{


class SegmentStatistics {
public:
    SegmentStatistics(const LogicalType &type):type_(type) {
        reset();
    }
    SegmentStatistics(const LogicalType &type, std::unique_ptr<BaseStatistics> statistics): type_(type), statistics_(std::move(statistics)) {
        if (!statistics) {
            reset();
        }
    }

    LogicalType type_;

    // Type-specific statistics of the segment
    std::unique_ptr<BaseStatistics> statistics_;

    void reset() {
        statistics_ = BaseStatistics::createEmpty(type_);
        statistics_->validityStats_ = std::make_unique<ValidityStatistics>(false);
    }
};


}
