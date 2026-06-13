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
#include "bumblebee/common/types/Vector.hpp"
#include "bumblebee/common/operator/ComparisonOperators.hpp"
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
    // An all-NULL segment never satisfies a value comparison (NULL is never =,<,> a constant).
    if (!stats.canHaveNoNull()) {
        return FilterPropagateResult::FILTER_ALWAYS_FALSE;
    }
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

namespace {

template <class T, class OP>
void filterRowsLoop(Vector &v, const T constant, idx_t count, std::bitset<STANDARD_VECTOR_SIZE> &mask) {
    auto data = FlatVector::getData<T>(v);
    for (idx_t i = 0; i < count; i++) {
        if (!mask[i]) continue;          // already filtered out (value may be undecoded)
        if (!v.rowIsValid(i)) continue;  // NULL: must be left to the pipeline filter
        if (!OP::operation(data[i], constant)) mask[i] = false;
    }
}

template <class T>
void filterRowsTypedOp(Binop op, Vector &v, const T constant, idx_t count, std::bitset<STANDARD_VECTOR_SIZE> &mask) {
    switch (op) {
        case EQUAL:         filterRowsLoop<T, Equals>(v, constant, count, mask); break;
        case UNEQUAL:       filterRowsLoop<T, NotEquals>(v, constant, count, mask); break;
        case LESS:          filterRowsLoop<T, LessThan>(v, constant, count, mask); break;
        case LESS_OR_EQ:    filterRowsLoop<T, LessThanEquals>(v, constant, count, mask); break;
        case GREATER:       filterRowsLoop<T, GreaterThan>(v, constant, count, mask); break;
        case GREATER_OR_EQ: filterRowsLoop<T, GreaterThanEquals>(v, constant, count, mask); break;
        default: break; // unknown comparison: keep all rows
    }
}

} // namespace

void ConstantFilter::filterRows(Vector &v, idx_t count, std::bitset<STANDARD_VECTOR_SIZE> &mask) {
    // Evaluate `column <op> constant` on the decoded values and clear the mask
    // bits of rows that provably fail. This must be a subset of what the
    // downstream pipeline filter rejects, so every uncertain case (NULLs,
    // non-representable constants, unhandled types) keeps its bit set.
    const auto physType = v.getType();
    const auto logicalId = v.getLogicalType().type();

    if (logicalId == LogicalTypeId::TIMESTAMP) {
        return; // mirror checkStatistics: timestamp constants are not handled
    }
    if (logicalId == LogicalTypeId::DATE ||
        (logicalId == LogicalTypeId::DECIMAL && constant_.getPhysicalType() != physType)) {
        // mirror checkStatistics: convert the constant (string date -> days,
        // integer -> scaled decimal) only in the cases the statistics check
        // converts; bail out (keep all rows) when it isn't convertible
        Value converted;
        if (!castConstantForStats(constant_, v.getLogicalType(), converted)) return;
        switch (physType) {
            case PhysicalType::SMALLINT: filterRowsTypedOp<int16_t>(comparisonType_, v, converted.getValueUnsafe<int16_t>(), count, mask); break;
            case PhysicalType::INTEGER:  filterRowsTypedOp<int32_t>(comparisonType_, v, converted.getValueUnsafe<int32_t>(), count, mask); break;
            case PhysicalType::BIGINT:   filterRowsTypedOp<int64_t>(comparisonType_, v, converted.getValueUnsafe<int64_t>(), count, mask); break;
            default: break;
        }
        return;
    }
    if (logicalId == LogicalTypeId::DECIMAL) {
        // same physical type: the constant already carries the scaled
        // representation (as in checkStatistics), compare it directly
        switch (physType) {
            case PhysicalType::SMALLINT: filterRowsTypedOp<int16_t>(comparisonType_, v, constant_.getValueUnsafe<int16_t>(), count, mask); break;
            case PhysicalType::INTEGER:  filterRowsTypedOp<int32_t>(comparisonType_, v, constant_.getValueUnsafe<int32_t>(), count, mask); break;
            case PhysicalType::BIGINT:   filterRowsTypedOp<int64_t>(comparisonType_, v, constant_.getValueUnsafe<int64_t>(), count, mask); break;
            default: break;
        }
        return;
    }
    if (physType == PhysicalType::STRING) {
        const string constStr = constant_.toString();
        const string_t cst(constStr.c_str(), (uint32_t)constStr.size());
        filterRowsTypedOp<string_t>(comparisonType_, v, cst, count, mask);
        return;
    }

    // Numeric columns: cast the constant to the column type, but only filter
    // when the cast is exact (round-trips back to the original value).
    // Otherwise a truncated constant (e.g. X < 10.5 against an int column)
    // would reject rows the pipeline filter keeps.
    Value casted, roundTrip;
    string error;
    if (!constant_.tryCastAs(physType, casted, &error)) return;
    if (!casted.tryCastAs(constant_.getPhysicalType(), roundTrip, &error)) return;
    if (!(roundTrip == constant_)) return;

    switch (physType) {
        case PhysicalType::TINYINT:   filterRowsTypedOp<int8_t>(comparisonType_, v, casted.getValueUnsafe<int8_t>(), count, mask); break;
        case PhysicalType::SMALLINT:  filterRowsTypedOp<int16_t>(comparisonType_, v, casted.getValueUnsafe<int16_t>(), count, mask); break;
        case PhysicalType::INTEGER:   filterRowsTypedOp<int32_t>(comparisonType_, v, casted.getValueUnsafe<int32_t>(), count, mask); break;
        case PhysicalType::BIGINT:    filterRowsTypedOp<int64_t>(comparisonType_, v, casted.getValueUnsafe<int64_t>(), count, mask); break;
        case PhysicalType::UTINYINT:  filterRowsTypedOp<uint8_t>(comparisonType_, v, casted.getValueUnsafe<uint8_t>(), count, mask); break;
        case PhysicalType::USMALLINT: filterRowsTypedOp<uint16_t>(comparisonType_, v, casted.getValueUnsafe<uint16_t>(), count, mask); break;
        case PhysicalType::UINTEGER:  filterRowsTypedOp<uint32_t>(comparisonType_, v, casted.getValueUnsafe<uint32_t>(), count, mask); break;
        case PhysicalType::UBIGINT:   filterRowsTypedOp<uint64_t>(comparisonType_, v, casted.getValueUnsafe<uint64_t>(), count, mask); break;
        case PhysicalType::FLOAT:     filterRowsTypedOp<float>(comparisonType_, v, casted.getValueUnsafe<float>(), count, mask); break;
        case PhysicalType::DOUBLE:    filterRowsTypedOp<double>(comparisonType_, v, casted.getValueUnsafe<double>(), count, mask); break;
        default: break; // unhandled type: keep all rows
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