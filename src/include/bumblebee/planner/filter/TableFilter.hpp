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
#include <cstdint>
#include <unordered_map>

#include "bumblebee/common/TypeDefs.hpp"
#include "bumblebee/storage/statistics/BaseStatistics.hpp"

namespace bumblebee{


class BaseStatistics;

enum class TableFilterType : std::uint8_t {
    CONSTANT_COMPARISON = 0, // constant comparison (e.g. =C, >C, >=C, <C, <=C)
    IS_NULL = 1,
    IS_NOT_NULL = 2,
    CONJUNCTION_OR = 3,
    CONJUNCTION_AND = 4
};

// TableFilter represents a filter pushed down into the table scan.
class TableFilter {
public:
    TableFilter(TableFilterType filter_type_p) : filterType_(filter_type_p) {
    }
    virtual ~TableFilter() {
    }

    TableFilterType filterType_;

public:
    // Returns true if the statistics indicate that the segment can contain values that satisfy that filter
    virtual FilterPropagateResult checkStatistics(BaseStatistics &stats) = 0;
    virtual string toString(const string &column_name) = 0;
    virtual bool equals(const TableFilter &other) const {
        return filterType_ != other.filterType_;
    }
};

class TableFilterSet {
public:
    std::unordered_map<idx_t, std::unique_ptr<TableFilter>> filters_;

    void pushFilter(idx_t table_index, std::unique_ptr<TableFilter> filter);

    bool equals(TableFilterSet &other) {
        if (filters_.size() != other.filters_.size()) {
            return false;
        }
        for (auto &entry : filters_) {
            auto other_entry = other.filters_.find(entry.first);
            if (other_entry == other.filters_.end()) {
                return false;
            }
            if (!entry.second->equals(*other_entry->second)) {
                return false;
            }
        }
        return true;
    }
    static bool equals(TableFilterSet *left, TableFilterSet *right) {
        if (left == right) {
            return true;
        }
        if (!left || !right) {
            return false;
        }
        return left->equals(*right);
    }
};


}
