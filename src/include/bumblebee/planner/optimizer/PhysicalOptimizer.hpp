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
#include "bumblebee/execution/PhysicalRule.hpp"
#include "bumblebee/parser/statement/Rule.hpp"

namespace bumblebee{


// Create the optimal physical operators for the execution of the Rules
class PhysicalOptimizer {
    using cols_vector_t = vector<vector<idx_t>> ;
    using type_vector_t = vector<vector<ConstantType>>;
public:
    explicit PhysicalOptimizer(ClientContext &context, bool recursiveRules = false);

    virtual ~PhysicalOptimizer() = default;

    virtual std::unordered_map<Predicate*, vector<ConstantType>> getHeadTypes(Rule& rule);
    virtual prule_ptr_vector_t optimize(Rule& rule);
    virtual void clear();


private:
    // create physical rules for the execution of the rule
    // multiple rules can be created from this rule with different priorities
    prule_ptr_vector_t createPhysicalRules(Rule & rule);
    // For each atom find the used columns and types for the data chunk
    void findColsAndTypes(Rule& rule);
    void findColsAndTypesClassicalAtom(Atom& atom);
    void findColsAndTypesAggregateAtom(Atom& atom);
    void findColsAndTypesBuiltin(Atom& atom);
    void findColsAndTypesExternalAtom(Atom &atom, idx_t index);

    void bindExternalAtom(idx_t index, Atom& atom, vector<ConstantType>& returnTypes, vector<string>& names);

    // return true if the rule can be not evaluated
    bool canBeSkipped(Rule& rule);

    // generate the patoms for the builtin atoms
    void generatePhysicalExpression(Atom& atom, vector<idx_t>& cols,vector<ConstantType> types,patom_ptr_vector_t& patoms);
    // generate the join patoms, vars is the variables seen so far in the rule used to calculate the join keys
    void generatePhysicalJoin(const set_term_variable_t& vars, idx_t i, Rule& rule,patom_ptr_vector_t& patoms, prule_ptr_vector_t& prules, idx_t& priority);

    void generateOutputPhysicalAtom(Rule &rule, patom_ptr_t &sink, Schema &schema, prule_ptr_vector_t& prules, idx_t priority);

    void generatePhysicalAgg(Atom& atom, vector<idx_t>& cols, patom_ptr_vector_t &patoms);

    void generateHTBuildRules(PredicateTables* pred, vector<idx_t>& keys, vector<idx_t>& payloads,
                              prule_ptr_vector_t& prules, idx_t& priority);
    void generateJoinRLHTBuildRules(PredicateTables* pred, vector<idx_t>& keys, vector<idx_t>& payloads,
                              prule_ptr_vector_t& prules, idx_t& priority);
    void generatePhysicalExternal(const set_term_variable_t& vars, idx_t index, Rule& rule, patom_ptr_vector_t &patoms);

    bool isRowLayoutHTRuleHTCreated(Predicate* predicate, const vector<idx_t>& keys, const vector<idx_t>& payloads );


    // global client context
    ClientContext& context_;
    // for each atom in the body list of column to use in the data chunk
    cols_vector_t cols_;
    cols_vector_t headCols_;
    // for each atom in the body the column to project (for atoms with predicates)
    cols_vector_t selectedCols_;
    // types for all the column, are the same for all the atoms. Atoms that use a subset of columns will
    // filter using cols_ field
    vector<ConstantType> types_;
    // Map of variable -> index in the data chunk
    std::unordered_map<std::string, idx_t> colsMap_;
    // Map of variable -> data type
    std::unordered_map<std::string, ConstantType> typesMap_;
    // skip atom map, if true does not create physical atoms
    std::unordered_map<idx_t, bool> skipAtom_;
    // bind data for the external atoms
    std::unordered_map<idx_t, function_data_ptr_t> externalBindData_;
    // Track all the row laoyt join table created
    std::unordered_map<Predicate*, vector<RowLayoutJoinHashTable*>> predicatesRLJoins_;;


    bool recursiveRules_{false};
};


}
