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
#include "../../include/bumblebee/planner/filter/ConjunctionFilter.hpp"

namespace bumblebee{

ConjunctionOrFilter::ConjunctionOrFilter() : TableFilter(TableFilterType::CONJUNCTION_OR) {
}

FilterPropagateResult ConjunctionOrFilter::checkStatistics(BaseStatistics &stats) {
	// the OR filter is true if ANY of the children is true
	BB_ASSERT(!childFilters_.empty());
	for (auto &filter : childFilters_) {
		auto prune_result = filter->checkStatistics(stats);
		if (prune_result == FilterPropagateResult::NO_PRUNING_POSSIBLE) {
			return FilterPropagateResult::NO_PRUNING_POSSIBLE;
		} else if (prune_result == FilterPropagateResult::FILTER_ALWAYS_TRUE) {
			return FilterPropagateResult::FILTER_ALWAYS_TRUE;
		}
	}
	return FilterPropagateResult::FILTER_ALWAYS_FALSE;
}

string ConjunctionOrFilter::toString(const string &column_name) {
	string result;
	for (idx_t i = 0; i < childFilters_.size(); i++) {
		if (i > 0) {
			result += " OR ";
		}
		result += childFilters_[i]->toString(column_name);
	}
	return result;
}

bool ConjunctionOrFilter::equals(const TableFilter &other_p) const {
	if (!TableFilter::equals(other_p)) {
		return false;
	}
	auto &other = (ConjunctionOrFilter &)other_p;
	if (other.childFilters_.size() != childFilters_.size()) {
		return false;
	}
	for (idx_t i = 0; i < other.childFilters_.size(); i++) {
		if (!childFilters_[i]->equals(*other.childFilters_[i])) {
			return false;
		}
	}
	return true;
}

ConjunctionAndFilter::ConjunctionAndFilter() : TableFilter(TableFilterType::CONJUNCTION_AND) {
}

FilterPropagateResult ConjunctionAndFilter::checkStatistics(BaseStatistics &stats) {
	// the OR filter is true if ALL of the children is true
	BB_ASSERT(!childFilters_.empty());
	auto result = FilterPropagateResult::FILTER_ALWAYS_TRUE;
	for (auto &filter : childFilters_) {
		auto prune_result = filter->checkStatistics(stats);
		if (prune_result == FilterPropagateResult::FILTER_ALWAYS_FALSE) {
			return FilterPropagateResult::FILTER_ALWAYS_FALSE;
		} else if (prune_result != result) {
			result = FilterPropagateResult::NO_PRUNING_POSSIBLE;
		}
	}
	return result;
}

string ConjunctionAndFilter::toString(const string &column_name) {
	string result;
	for (idx_t i = 0; i < childFilters_.size(); i++) {
		if (i > 0) {
			result += " AND ";
		}
		result += childFilters_[i]->toString(column_name);
	}
	return result;
}

bool ConjunctionAndFilter::equals(const TableFilter &other_p) const {
	if (!TableFilter::equals(other_p)) {
		return false;
	}
	auto &other = (ConjunctionAndFilter &)other_p;
	if (other.childFilters_.size() != childFilters_.size()) {
		return false;
	}
	for (idx_t i = 0; i < other.childFilters_.size(); i++) {
		if (!childFilters_[i]->equals(*other.childFilters_[i])) {
			return false;
		}
	}
	return true;
}

}