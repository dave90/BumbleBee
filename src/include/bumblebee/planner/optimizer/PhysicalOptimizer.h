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
    explicit PhysicalOptimizer(ClientContext &context);

    virtual ~PhysicalOptimizer() = default;


    virtual prule_ptr_vector_t optimize(Rule& rule);
    virtual void clear();


private:
    // create physical rules for the execution of the rule
    // multiple rules can be created from this rule with different priorities
    prule_ptr_vector_t createPhysicalRules(Rule & rule);
    // For each atom find the used columns and types for the data chunk
    void findColsAndTypes(Rule& rule);
    void findColsAndTypesClassicalAtom(Atom& atom);
    void findColsAndTypesBuiltin(Atom& atom);

    void generatePhysicalExpression(Atom& atom, std::vector<idx_t>& cols,std::vector<ConstantType> types,patom_ptr_vector_t& patoms);

    // global client context
    ClientContext& context_;
    // for each atom in the body list of column to use in the data chunk
    cols_vector_t cols_;
    cols_vector_t headCols_;
    // for each atom in the body the column to project (for atoms with predicates)
    cols_vector_t projectCols_;
    // types for all the column, are the same for all the atoms. Atoms that use a subset of columns will
    // filter using cols_ field
    std::vector<ConstantType> types_;
    // Map of variable -> index in the data chunk
    std::unordered_map<std::string, idx_t> colsMap_;
    // Map of variable -> data type
    std::unordered_map<std::string, ConstantType> typesMap_;
};


}
