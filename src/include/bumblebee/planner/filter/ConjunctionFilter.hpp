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
#include "TableFilter.hpp"

namespace bumblebee{


class ConjunctionOrFilter : public TableFilter {
public:
    ConjunctionOrFilter();

    //! The filters to OR together
    vector<std::unique_ptr<TableFilter>> childFilters_;

public:
    FilterPropagateResult checkStatistics(BaseStatistics &stats) override;
    string toString(const string &column_name) override;
    bool equals(const TableFilter &other) const override;
};

class ConjunctionAndFilter : public TableFilter {
public:
    ConjunctionAndFilter();

    //! The filters to OR together
    vector<std::unique_ptr<TableFilter>> childFilters_;

public:
    FilterPropagateResult checkStatistics(BaseStatistics &stats) override;
    string toString(const string &column_name) override;
    bool equals(const TableFilter &other) const override;
};


}
