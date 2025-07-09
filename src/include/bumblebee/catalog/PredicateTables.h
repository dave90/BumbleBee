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
#include "bumblebee/common/Hash.h"
#include "bumblebee/parser/statement/Predicate.h"

namespace bumblebee{


class PredicateTables {
public:
    PredicateTables(const char* name, unsigned arity);
    PredicateTables(const PredicateTables &other) = delete;
    PredicateTables(PredicateTables &&other) noexcept = delete;
    ~PredicateTables() = default;

    PredicateTables & operator=(const PredicateTables &other) = delete;
    PredicateTables & operator=(PredicateTables &&other) noexcept = delete;

    predicate_ptr_t predicate_;
    // TODO EDB and IDB
    // cache output
    // hash tables etc

    friend bool operator==(const PredicateTables &lhs, const PredicateTables &rhs);

    friend bool operator!=(const PredicateTables &lhs, const PredicateTables &rhs);
};

using predicate_table_ptr_t = std::unique_ptr<PredicateTables>;

}
