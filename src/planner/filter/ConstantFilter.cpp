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
#include "../../include/bumblebee/planner/filter/ConstantFilter.hpp"
#include "bumblebee/storage/statistics/NumericStatistics.hpp"
#include "bumblebee/storage/statistics/StringStatistics.hpp"

namespace bumblebee{

ConstantFilter::ConstantFilter(Binop comparison_type_p, Value constant_p)
    : TableFilter(TableFilterType::CONSTANT_COMPARISON), comparisonType_(comparison_type_p),
      constant_(std::move(constant_p)) {
}

FilterPropagateResult ConstantFilter::checkStatistics(BaseStatistics &stats) {
    BB_ASSERT(constant_.getPhysicalType() == stats.type_);
    switch (constant_.getPhysicalType()) {
        case PhysicalType::UTINYINT:
        case PhysicalType::USMALLINT:
        case PhysicalType::UINTEGER:
        case PhysicalType::UBIGINT:
        case PhysicalType::TINYINT:
        case PhysicalType::SMALLINT:
        case PhysicalType::INTEGER:
        case PhysicalType::BIGINT:
        case PhysicalType::FLOAT:
        case PhysicalType::DOUBLE:
            return ((NumericStatistics &)stats).checkZonemap(comparisonType_, constant_);
        case PhysicalType::STRING:
            return ((StringStatistics &)stats).checkZonemap(comparisonType_, constant_.toString());
        default:
            return FilterPropagateResult::NO_PRUNING_POSSIBLE;
    }
}

string ConstantFilter::toString(const string &column_name) {
    return column_name + " "+getBinopStr(comparisonType_) +" "+ constant_.toString();
}

bool ConstantFilter::equals(const TableFilter &other_p) const {
    if (!TableFilter::equals(other_p)) {
        return false;
    }
    auto &other = (ConstantFilter &)other_p;
    return other.comparisonType_ == comparisonType_ && other.constant_ == constant_;
}

}