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
#include "bumblebee/planner/filter/TableFilter.hpp"

#include "bumblebee/planner/filter/ConjunctionFilter.hpp"

namespace bumblebee{

void TableFilterSet::pushFilter(idx_t column_index, std::unique_ptr<TableFilter> filter) {
    auto entry = filters_.find(column_index);
    if (entry == filters_.end()) {
        // no filter yet: push the filter directly
        filters_[column_index] = std::move(filter);
    } else {
        // there is already a filter: AND it together
        if (entry->second->filterType_ == TableFilterType::CONJUNCTION_AND) {
            auto &and_filter = (ConjunctionAndFilter &)*entry->second;
            and_filter.childFilters_.push_back(std::move(filter));
        } else {
            auto and_filter = std::make_unique<ConjunctionAndFilter>();
            and_filter->childFilters_.push_back(std::move(entry->second));
            and_filter->childFilters_.push_back(std::move(filter));
            filters_[column_index] = std::move(and_filter);
        }
    }
}

}
