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
#include "bumblebee/common/types/Date.hpp"
#include "bumblebee/common/NumericUtils.hpp"

namespace bumblebee{

static bool castConstantForStats(const Value &constant, const LogicalType &statsType, Value &outValue) {
    switch (statsType.type()) {
        case LogicalTypeId::DATE: {
            auto str = constant.toString();
            idx_t pos;
            date_t dateVal;
            if (!Date::tryConvertDate(str.c_str(), str.length(), pos, dateVal, true)) {
                return false;
            }
            outValue = Value((int32_t)dateVal);
            return true;
        }
        case LogicalTypeId::TIMESTAMP: {
            ErrorHandler::errorNotImplemented("Timestamp filter pushdown not yet supported");
            return false;
        }
        case LogicalTypeId::DECIMAL: {
            auto scale = statsType.getDecimalData().scale_;
            int64_t scaledVal = constant.getNumericValue<int64_t>() * NumericHelper::POWERS_OF_TEN[scale];
            switch (statsType.getPhysicalType()) {
                case PhysicalType::SMALLINT:
                    outValue = Value((int16_t)scaledVal);
                    return true;
                case PhysicalType::INTEGER:
                    outValue = Value((int32_t)scaledVal);
                    return true;
                case PhysicalType::BIGINT:
                    outValue = Value(scaledVal);
                    return true;
                default:
                    return false;
            }
        }
        default:
            return false;
    }
}

ConstantFilter::ConstantFilter(Binop comparison_type_p, Value constant_p)
    : TableFilter(TableFilterType::CONSTANT_COMPARISON), comparisonType_(comparison_type_p),
      constant_(std::move(constant_p)) {
}

FilterPropagateResult ConstantFilter::checkStatistics(BaseStatistics &stats) {
    auto logicalTypeId = stats.type_.type();
    if (logicalTypeId == LogicalTypeId::DATE ||
        logicalTypeId == LogicalTypeId::TIMESTAMP ||
        (logicalTypeId == LogicalTypeId::DECIMAL &&
         constant_.getPhysicalType() != stats.type_.getPhysicalType())) {
        Value convertedConstant;
        if (!castConstantForStats(constant_, stats.type_, convertedConstant)) {
            return FilterPropagateResult::NO_PRUNING_POSSIBLE;
        }
        return ((NumericStatistics &)stats).checkZonemap(comparisonType_, convertedConstant);
    }

    switch (stats.type_.getPhysicalType()) {
        case PhysicalType::UTINYINT:
        case PhysicalType::USMALLINT:
        case PhysicalType::UINTEGER:
        case PhysicalType::UBIGINT:
        case PhysicalType::TINYINT:
        case PhysicalType::SMALLINT:
        case PhysicalType::INTEGER:
        case PhysicalType::BIGINT:
        case PhysicalType::FLOAT:
        case PhysicalType::DOUBLE: {
            return ((NumericStatistics &)stats).checkZonemap(comparisonType_, constant_.cast(stats.type_.getPhysicalType()));
        }case PhysicalType::STRING: {
            BB_ASSERT(stats.type_.getPhysicalType() == PhysicalType::STRING);
            return ((StringStatistics &)stats).checkZonemap(comparisonType_, constant_.toString());
        }default:
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