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
#include "bumblebee/common/operator/ComparisonOperators.hpp"
#include "SegmentStatistics.hpp"

namespace bumblebee{


class NumericStatistics : public BaseStatistics {
public:
	explicit NumericStatistics(PhysicalType type);
	NumericStatistics(PhysicalType type,const Value& min,const Value& max);

	// The minimum value of the segment
	Value min_;
	// The maximum value of the segment
	Value max_;

public:
	void merge(const BaseStatistics &other) override;

	bool isConstant() override;

	FilterPropagateResult checkZonemap(Binop comparison_type, const Value &constant);

	std::unique_ptr<BaseStatistics> copy() override;

	string toString() override;

private:

public:
	template <class T>
	static inline void updateValue(T new_value, T &min, T &max) {
		if (LessThan::operation(new_value, min)) {
			min = new_value;
		}
		if (GreaterThan::operation(new_value, max)) {
			max = new_value;
		}
	}

	template <class T>
	static inline void update(SegmentStatistics &stats, T new_value);
};

template <>
void NumericStatistics::update<int8_t>(SegmentStatistics &stats, int8_t new_value);
template <>
void NumericStatistics::update<int16_t>(SegmentStatistics &stats, int16_t new_value);
template <>
void NumericStatistics::update<int32_t>(SegmentStatistics &stats, int32_t new_value);
template <>
void NumericStatistics::update<int64_t>(SegmentStatistics &stats, int64_t new_value);
template <>
void NumericStatistics::update<uint8_t>(SegmentStatistics &stats, uint8_t new_value);
template <>
void NumericStatistics::update<uint16_t>(SegmentStatistics &stats, uint16_t new_value);
template <>
void NumericStatistics::update<uint32_t>(SegmentStatistics &stats, uint32_t new_value);
template <>
void NumericStatistics::update<uint64_t>(SegmentStatistics &stats, uint64_t new_value);
template <>
void NumericStatistics::update<float>(SegmentStatistics &stats, float new_value);
template <>
void NumericStatistics::update<double>(SegmentStatistics &stats, double new_value);


}
