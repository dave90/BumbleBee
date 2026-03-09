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

#include "bumblebee/storage/statistics/BaseStatistics.hpp"

#include "bumblebee/common/StringUtils.hpp"
#include "bumblebee/storage/statistics/NumericStatistics.hpp"
#include "bumblebee/storage/statistics/StringStatistics.hpp"
#include "bumblebee/storage/statistics/ValidityStatistics.hpp"

namespace bumblebee {

BaseStatistics::BaseStatistics(LogicalType type) : type_(type) {
}

BaseStatistics::~BaseStatistics() {
}

bool BaseStatistics::canHaveNull() {
	if (!validityStats_) {
		// we don't know
		// solid maybe
		return true;
	}
	return ((ValidityStatistics &)*validityStats_).hasNull_;}

bool BaseStatistics::canHaveNoNull() {
	if (!validityStats_) {
		// we don't know
		// solid maybe
		return true;
	}
	return ((ValidityStatistics &)*validityStats_).hasNull_;
}

std::unique_ptr<BaseStatistics> BaseStatistics::copy() {
	auto statistics = std::make_unique<BaseStatistics>(type_);
	if (validityStats_) {
		statistics->validityStats_ = validityStats_->copy();
	}
	return statistics;
}

void BaseStatistics::merge(const BaseStatistics &other) {
	BB_ASSERT(type_ == other.type_);
	if (other.validityStats_) {
		if (validityStats_) {
			validityStats_->merge(*other.validityStats_);
		} else {
			validityStats_ = other.validityStats_->copy();
		}
	}
}

std::unique_ptr<BaseStatistics> BaseStatistics::createEmpty(const LogicalType &type) {
	switch (type.getPhysicalType()) {
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
		return std::make_unique<NumericStatistics>(type);
	case PhysicalType::STRING:
		return std::make_unique<StringStatistics>(type);
	default:
		auto base_stats = std::make_unique<BaseStatistics>(type);
		return base_stats;
	}
}

void BaseStatistics::serialize(Serializer &serializer) {
	// TODO
}

std::unique_ptr<BaseStatistics> BaseStatistics::deserialize(Deserializer &source, const LogicalType &type) {
	// TODO
	return nullptr;
}

string BaseStatistics::toString() {
	return StringUtils::format("Base Statistics %s", validityStats_ ? validityStats_->toString() : "[]");
}

void BaseStatistics::verify(Vector &vector, const SelectionVector &sel, idx_t count) {
	if (validityStats_) {
		validityStats_->verify(vector, sel, count);
	}
}

void BaseStatistics::verify(Vector &vector, idx_t count) {
	verify(vector, FlatVector::INCREMENTAL_SELECTION_VECTOR, count);
}

}
