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
#include "bumblebee/execution/PhysicalRule.h"
#include "bumblebee/parser/statement/Rule.h"

namespace bumblebee{

// Create the optimal physical operators for the execution of the Rules
class PhysicalOptimizer {
    using cols_vector_t = std::vector<std::vector<idx_t>> ;
    using type_vector_t = std::vector<std::vector<ConstantType>>;
public:
    virtual ~PhysicalOptimizer() = default;
    virtual prule_ptr_vector_t optimize(Rule& rule);
private:
    // For each atom find the used columns for the data chunk
    cols_vector_t findCols(Rule& rule);
    // Find for each atom the types of the column
    type_vector_t findTypes(cols_vector_t &cols, Rule& rule);
};


}
