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
#include "bumblebee/storage/statistics/ValidityStatistics.hpp"

namespace bumblebee{

ValidityStatistics::ValidityStatistics(bool has_null, bool has_no_null)
    : BaseStatistics(LogicalType(PhysicalType::UTINYINT)), hasNull_(has_null), hasNoNull_(has_no_null) {
}

std::unique_ptr<BaseStatistics> ValidityStatistics::combine(const std::unique_ptr<BaseStatistics> &lstats,
                                                       const std::unique_ptr<BaseStatistics> &rstats) {
	if (!lstats && !rstats) {
		return nullptr;
	} else if (!lstats) {
		return rstats->copy();
	} else if (!rstats) {
		return lstats->copy();
	} else {
		auto &l = (ValidityStatistics &)*lstats;
		auto &r = (ValidityStatistics &)*rstats;
		return std::make_unique<ValidityStatistics>(l.hasNull_ || r.hasNull_, l.hasNoNull_ || r.hasNoNull_);
	}
}

bool ValidityStatistics::isConstant() {
	if (!hasNull_) {
		return true;
	}
	if (!hasNoNull_) {
		return true;
	}
	return false;
}

void ValidityStatistics::merge(const BaseStatistics &other_p) {
	auto &other = (ValidityStatistics &)other_p;
	hasNull_ = hasNull_ || other.hasNull_;
	hasNoNull_ = hasNoNull_ || other.hasNoNull_;
}

std::unique_ptr<BaseStatistics> ValidityStatistics::copy() {
	return std::make_unique<ValidityStatistics>(hasNull_, hasNoNull_);
}


string ValidityStatistics::toString() {
	return hasNull_ ? "[Has Null: true]" : "[Has Null: false]";
}

}