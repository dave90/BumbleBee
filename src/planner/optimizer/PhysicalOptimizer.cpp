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

#include "bumblebee/planner/optimizer/PhysicalOptimizer.h"

namespace bumblebee {

prule_ptr_vector_t PhysicalOptimizer::optimize(Rule &rule) {
    auto cols = findCols(rule);
    auto types = findTypes(cols, rule);
    // TODO complete it
}

PhysicalOptimizer::cols_vector_t PhysicalOptimizer::findCols(Rule &rule) {
    cols_vector_t cols;
    // maps of variable and cols in the data chunk
    std::unordered_map<std::string, idx_t> colsMap;
    for (auto& atom:rule.getBody()) {
        std::vector<idx_t> atomCols;
        for (auto& term : atom.getTerms()) {
            // expected variable
            BB_ASSERT(term.getType() == VARIABLE);
            if (term.isAnonymous())continue;
            auto& variable = term.getVariable();
            if (!colsMap.contains(term.getVariable())) {
                // first time we see this variable, will be a new column in data chunk
                colsMap[term.getVariable()] = colsMap.size();
            }
            atomCols.push_back(colsMap[term.getVariable()]);
        }
        cols.push_back(std::move(atomCols));
    }

    return cols;
}

PhysicalOptimizer::type_vector_t PhysicalOptimizer::findTypes(cols_vector_t &cols, Rule &rule) {
    // TODO complete it
}
}
