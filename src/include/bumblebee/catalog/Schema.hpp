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
#include <map>
#include <string>

#include "PredicateTables.hpp"

namespace bumblebee{

using predicates_tables_map = std::unordered_map<PredicateMapEntry, predicate_table_ptr_t, PredicateMapEntry::PEHash>;

class Schema {
public:
    explicit Schema(const std::string &name);

    ~Schema() = default;

    Schema(const Schema &other) = delete;
    Schema & operator=(const Schema &other) = delete;
    Predicate* createPredicate(ClientContext* context, const char* predicateName, unsigned arity);
    predicate_table_ptr_t& getPredicateTable(Predicate*);
    vector<Predicate*> getPredicates();
    Predicate* getFASOPredicate();

private:
    void initDefaultPredicates();

    std::string name_;
    predicates_tables_map ptables_;
};


}
