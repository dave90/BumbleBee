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
#include <pybind11/pybind11.h>
#include <optional>
#include <string>

#include "bumblebee/common/TypeDefs.hpp"
#include "bumblebee/common/Vector.hpp"

namespace bumblebee {
class PredicateTables;
} // namespace bumblebee

namespace bumblebee::python {

class PyPredicateTable {
public:
    PyPredicateTable(std::string name, int arity, bumblebee::PredicateTables* pt);

    pybind11::list tuples() const;
    pybind11::object toDf(const vector<string>& names = {}) const;
    const std::string& getName() const;
    int getArity() const;

private:
    pybind11::dict fetchNumpyInternal(const vector<string>& names) const;


    std::string name_;
    int arity_;
    bumblebee::PredicateTables* pt_;
};

} // namespace bumblebee::python