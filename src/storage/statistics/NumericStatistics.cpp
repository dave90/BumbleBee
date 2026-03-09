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
#include "bumblebee/storage/statistics/NumericStatistics.hpp"

#include "bumblebee/common/StringUtils.hpp"

namespace bumblebee{

template <>
void NumericStatistics::update<int8_t>(SegmentStatistics &stats, int8_t new_value) {
	auto &nstats = (NumericStatistics &)*stats.statistics_;
	updateValue<int8_t>(new_value, nstats.min_.value_.tinyint, nstats.max_.value_.tinyint);
}

template <>
void NumericStatistics::update<int16_t>(SegmentStatistics &stats, int16_t new_value) {
	auto &nstats = (NumericStatistics &)*stats.statistics_;
	updateValue<int16_t>(new_value, nstats.min_.value_.smallint, nstats.max_.value_.smallint);
}

template <>
void NumericStatistics::update<int32_t>(SegmentStatistics &stats, int32_t new_value) {
	auto &nstats = (NumericStatistics &)*stats.statistics_;
	updateValue<int32_t>(new_value, nstats.min_.value_.integer, nstats.max_.value_.integer);
}

template <>
void NumericStatistics::update<int64_t>(SegmentStatistics &stats, int64_t new_value) {
	auto &nstats = (NumericStatistics &)*stats.statistics_;
	updateValue<int64_t>(new_value, nstats.min_.value_.bigint, nstats.max_.value_.bigint);
}

template <>
void NumericStatistics::update<uint8_t>(SegmentStatistics &stats, uint8_t new_value) {
	auto &nstats = (NumericStatistics &)*stats.statistics_;
	updateValue<uint8_t>(new_value, nstats.min_.value_.utinyint, nstats.max_.value_.utinyint);
}

template <>
void NumericStatistics::update<uint16_t>(SegmentStatistics &stats, uint16_t new_value) {
	auto &nstats = (NumericStatistics &)*stats.statistics_;
	updateValue<uint16_t>(new_value, nstats.min_.value_.usmallint, nstats.max_.value_.usmallint);
}

template <>
void NumericStatistics::update<uint32_t>(SegmentStatistics &stats, uint32_t new_value) {
	auto &nstats = (NumericStatistics &)*stats.statistics_;
	updateValue<uint32_t>(new_value, nstats.min_.value_.uinteger, nstats.max_.value_.uinteger);
}

template <>
void NumericStatistics::update<uint64_t>(SegmentStatistics &stats, uint64_t new_value) {
	auto &nstats = (NumericStatistics &)*stats.statistics_;
	updateValue<uint64_t>(new_value, nstats.min_.value_.ubigint, nstats.max_.value_.ubigint);
}

template <>
void NumericStatistics::update<float>(SegmentStatistics &stats, float new_value) {
	auto &nstats = (NumericStatistics &)*stats.statistics_;
	updateValue<float>(new_value, nstats.min_.value_.float_, nstats.max_.value_.float_);
}

template <>
void NumericStatistics::update<double>(SegmentStatistics &stats, double new_value) {
	auto &nstats = (NumericStatistics &)*stats.statistics_;
	updateValue<double>(new_value, nstats.min_.value_.double_, nstats.max_.value_.double_);
}


NumericStatistics::NumericStatistics(const LogicalType &type_p) : BaseStatistics(type_p) {
	min_ = Value::maximumValue(type_p.getPhysicalType());
	max_ = Value::minimumValue(type_p.getPhysicalType());
	validityStats_ = std::make_unique<ValidityStatistics>(false);
}

NumericStatistics::NumericStatistics(const LogicalType &type_p, const Value& min_p, const Value& max_p)
    : BaseStatistics(type_p), min_(min_p.clone()), max_(max_p.clone()) {
}

void NumericStatistics::merge(const BaseStatistics &other_p) {
	BaseStatistics::merge(other_p);
	auto &other = (const NumericStatistics &)other_p;
	if (other.min_ < min_) {
		min_ = other.min_.clone();
	}
	if (other.max_ > max_) {
		max_ = other.max_.clone();
	}
}

FilterPropagateResult NumericStatistics::checkZonemap(Binop comparison_type, const Value &constant) {
	switch (comparison_type) {
	case EQUAL:
		if (constant == min_ && constant == max_) {
			return FilterPropagateResult::FILTER_ALWAYS_TRUE;
		} else if (constant >= min_ && constant <= max_) {
			return FilterPropagateResult::NO_PRUNING_POSSIBLE;
		} else {
			return FilterPropagateResult::FILTER_ALWAYS_FALSE;
		}
	case UNEQUAL:
		if (constant == min_ && constant == max_) {
			return FilterPropagateResult::FILTER_ALWAYS_FALSE;
		} else if (constant >= min_ && constant <= max_) {
			return FilterPropagateResult::NO_PRUNING_POSSIBLE;
		} else {
			return FilterPropagateResult::FILTER_ALWAYS_TRUE;
		}
	case GREATER_OR_EQ:
		// X >= C
		// this can be true only if max(X) >= C
		// if min(X) >= C, then this is always true
		if (min_ >= constant) {
			return FilterPropagateResult::FILTER_ALWAYS_TRUE;
		} else if (max_ >= constant) {
			return FilterPropagateResult::NO_PRUNING_POSSIBLE;
		} else {
			return FilterPropagateResult::FILTER_ALWAYS_FALSE;
		}
	case GREATER:
		// X > C
		// this can be true only if max(X) > C
		// if min(X) > C, then this is always true
		if (min_ > constant) {
			return FilterPropagateResult::FILTER_ALWAYS_TRUE;
		} else if (max_ > constant) {
			return FilterPropagateResult::NO_PRUNING_POSSIBLE;
		} else {
			return FilterPropagateResult::FILTER_ALWAYS_FALSE;
		}
	case LESS_OR_EQ:
		// X <= C
		// this can be true only if min(X) <= C
		// if max(X) <= C, then this is always true
		if (max_ <= constant) {
			return FilterPropagateResult::FILTER_ALWAYS_TRUE;
		} else if (min_ <= constant) {
			return FilterPropagateResult::NO_PRUNING_POSSIBLE;
		} else {
			return FilterPropagateResult::FILTER_ALWAYS_FALSE;
		}
	case LESS:
		// X < C
		// this can be true only if min(X) < C
		// if max(X) < C, then this is always true
		if (max_ < constant) {
			return FilterPropagateResult::FILTER_ALWAYS_TRUE;
		} else if (min_ < constant) {
			return FilterPropagateResult::NO_PRUNING_POSSIBLE;
		} else {
			return FilterPropagateResult::FILTER_ALWAYS_FALSE;
		}
		default:
			ErrorHandler::errorNotImplemented("Expression type in zonemap check not implemented");
			return FilterPropagateResult::NO_PRUNING_POSSIBLE;
	}
}

std::unique_ptr<BaseStatistics> NumericStatistics::copy() {
	auto stats = std::make_unique<NumericStatistics>(type_, min_, max_);
	if (validityStats_) {
		stats->validityStats_ = validityStats_->copy();
	}
	return std::move(stats);
}

bool NumericStatistics::isConstant() {
	return max_ <= min_;
}


string NumericStatistics::toString() {
	return StringUtils::format("[Min: %s, Max: %s]%s", min_.toString(), max_.toString(),
	                          validityStats_ ? validityStats_->toString() : string(""));
}


}
