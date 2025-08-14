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

#include "bumblebee/catalog/PredicateTables.h"
#include "bumblebee/execution/Expression.h"
#include "bumblebee/execution/NestedLoopJoin.h"
#include "bumblebee/execution/atom/expression/PhysicalExpression.h"
#include "bumblebee/execution/atom/join/PhysicalCrossProduct.h"
#include "bumblebee/execution/atom/join/PhysicalHashJoin.h"
#include "bumblebee/execution/atom/join/PhysicalHJBuild.h"
#include "bumblebee/execution/atom/join/PhysicalNestedLoop.h"
#include "bumblebee/execution/atom/output/PhysicalChunkOutput.h"
#include "bumblebee/execution/atom/output/PhysicalNopeOutput.h"
#include "bumblebee/execution/atom/scan/PhysicalChunkScan.h"

namespace bumblebee {
PhysicalOptimizer::PhysicalOptimizer(ClientContext& context)
    : context_(context) {
}

prule_ptr_vector_t PhysicalOptimizer::optimize(Rule &rule) {
    findColsAndTypes(rule);
    return createPhysicalRules(rule);
}

void PhysicalOptimizer::findColsAndTypesBuiltin(Atom &atom) {
    BB_ASSERT(atom.getType() == BUILTIN);
    BB_ASSERT(atom.getTerms().size() == 2);
    std::vector<string> vars;

    if (atom.isConstantAssignment()) {
        // is an ssignment with constant value
        auto& left = atom.getTerms()[0];
        auto& right = atom.getTerms()[1];
        BB_ASSERT(left.getType() == VARIABLE);
        BB_ASSERT(right.getType() == CONSTANT);
        BB_ASSERT(!colsMap_.contains(left.getVariable()));
        // we need to calculate the type of the left side
        typesMap_[left.getVariable()] = right.getConstantType();
        colsMap_[left.getVariable()] = colsMap_.size();
        vars.insert(vars.begin(),left.getVariable());
    }else if (atom.getBinop() != ASSIGNMENT) {
        // is not assignment so all the variables should be present in the col and types map
        for (auto &term: atom.getTerms()) {
            if (term.getType() == VARIABLE) {
                BB_ASSERT(colsMap_.contains(term.getVariable()));
                vars.push_back(term.getVariable());
            }else {
                // expected ARITH (no constant or range)
                BB_ASSERT(term.getType() == ARITH);
                // push the variables in the same order in the arith
                for (auto& t: term.getTerms()) {
                    BB_ASSERT(t.getType() == VARIABLE);
                    vars.push_back(t.getVariable());
                }
            }
        }
    }else {
        // if is assignment the left part is a new variable and we need to insert in the
        // map and type cols
        auto& left = atom.getTerms()[0];
        auto& right = atom.getTerms()[1];
        ConstantType resultType = UNKNOWN; // result type of the right side operations
        if (right.getType() == VARIABLE) {
            BB_ASSERT(colsMap_.contains(right.getVariable()));
            vars.push_back(right.getVariable());
            resultType = typesMap_[right.getVariable()];
        }else {
            // expected ARITH (no constant or range)
            BB_ASSERT(right.getType() == ARITH);
            // push the variables in the same order in the arith
            for (auto& t: right.getTerms()) {
                BB_ASSERT(t.getType() == VARIABLE);
                vars.push_back(t.getVariable());
                if (resultType == UNKNOWN) {
                    resultType = typesMap_[t.getVariable()];
                    continue;
                }
                resultType = getCommonType(resultType, typesMap_[t.getVariable()]);
                resultType = getBumpedType(resultType);
            }
        }
        // if we have a diff and is unsigned set to signed
        bool diff = std::find(right.getOperators().begin(), right.getOperators().end(), MINUS) != right.getOperators().end();
        if (diff && isUnsigned(resultType))
            resultType = BIGINT;

        BB_ASSERT(left.getType() == VARIABLE);
        BB_ASSERT(!colsMap_.contains(left.getVariable()));
        // we need to calculate the type of the left side
        typesMap_[left.getVariable()] = resultType;
        colsMap_[left.getVariable()] = colsMap_.size();
        vars.insert(vars.begin(),left.getVariable());
    }

    // populate the cols and types
    std::vector<idx_t> atomCols;
    for (auto& var : vars) {
        atomCols.push_back(colsMap_[var]);
    }
    cols_.push_back(std::move(atomCols));
    selectedCols_.emplace_back(); // push empty array as builtin does not have predicates
}

void PhysicalOptimizer::findColsAndTypesClassicalAtom(Atom &atom) {
    BB_ASSERT(atom.getType() == CLASSICAL);
    std::vector<idx_t> atomCols;
    std::vector<idx_t> prjCols;
    for (idx_t i=0;i<atom.getTerms().size();++i) {
        auto& term = atom.getTerms()[i];
        // expected variable
        BB_ASSERT(term.getType() == VARIABLE);
        if (term.isAnonymous())continue;
        if (!colsMap_.contains(term.getVariable())) {
            // first time we see this variable, will be a new column in data chunk
            // find the data types in the predicate tables
            auto& pt = context_.defaultSchema_.getPredicateTable(atom.getPredicate());
            BB_ASSERT(pt->getTypes()[i] != UNKNOWN);
            typesMap_[term.getVariable()] = pt->getTypes()[i];
            colsMap_[term.getVariable()] = colsMap_.size();
        }
        atomCols.push_back(colsMap_[term.getVariable()]);
        prjCols.push_back(i);
    }
    selectedCols_.push_back(std::move(prjCols));
    cols_.push_back(std::move(atomCols));
}

void PhysicalOptimizer::findColsAndTypes(Rule &rule ) {
    // maps of variable and cols in the data chunk
    for (auto& atom:rule.getBody()) {
        if (atom.getType() == CLASSICAL)
            findColsAndTypesClassicalAtom(atom);
        else if (atom.getType() == BUILTIN)
            findColsAndTypesBuiltin(atom);
        else
            ErrorHandler::errorNotImplemented("Optimizer: find columns implemented only for CLASSICAL and BUILTIN atoms");
    }
    // create the types for all the columns
    types_.clear();
    types_.resize(typesMap_.size());
    for (auto& [var, type] :typesMap_ ) {
        types_[colsMap_[var]] = type;
    }

    // find cols for the head
    for (auto& atom : rule.getHead()) {
        std::vector<idx_t> atomCols;
        BB_ASSERT(atom.getType() == CLASSICAL);
        for (auto& t: atom.getTerms()) {
            BB_ASSERT(t.getType() == VARIABLE);
            atomCols.push_back(colsMap_[t.getVariable()]);
        }
        headCols_.push_back(std::move(atomCols));
    }
}

void PhysicalOptimizer::generatePhysicalExpression(Atom& atom, std::vector<idx_t>& cols,std::vector<ConstantType> types,patom_ptr_vector_t& patoms ) {
    BB_ASSERT(atom.getType() == BUILTIN);
    if (atom.isConstantAssignment()) {
        auto patom = patom_ptr_t(new PhysicalExpression(cols[0], atom.getTerms()[1].getValue(), types));
        patoms.push_back(std::move(patom));
        return;
    }
    Operands left, right;
    Term& leftTerm = atom.getTerms()[0];
    Term& rightTerm = atom.getTerms()[1];
    left.cols_.push_back(cols[0]);
    idx_t idx = 1;
    for (auto& o: leftTerm.getOperators()) {
        BB_ASSERT(cols.size() > idx);
        left.cols_.push_back(cols[idx++]);
        left.operators_.push_back(o);
    }
    right.cols_.push_back(cols[idx++]);
    for (auto& o: rightTerm.getOperators()) {
        BB_ASSERT(cols.size() > idx - leftTerm.getOperators().size() - 1); // subtract the left iterms counter
        right.cols_.push_back(cols[idx++]);
        right.operators_.push_back(o);
    }

    // create the expression based on atom
    Expression expr(atom.getBinop(), left, right);
    auto patom = patom_ptr_t(new PhysicalExpression(expr, types, 0));
    patoms.push_back(std::move(patom));
}

void PhysicalOptimizer::generateHTBuildRules(PredicateTables* pred,
    std::vector<idx_t>& keys, prule_ptr_vector_t& prules, idx_t& priority) {

    std::vector<idx_t> cols;
    for (idx_t i = 0;i<pred->predicate_->getArity();++i)
        cols.push_back(i);

    auto types = pred->getTypes();

    patom_ptr_vector_t empty;
    {
        auto dbCols = cols, selCols = cols; // need to create a copy as constructor will move the data
        patom_ptr_t source = patom_ptr_t(new PhysicalChunkScan(types, dbCols, selCols, pred->getCount(), pred));
        dbCols = cols; selCols = cols;
        patom_ptr_t sink = patom_ptr_t(new PhysicalHJBuild(types, dbCols, selCols, 0 , pred, keys, true));
        prule_ptr_t pruleStats(new PhysicalRule(source, sink, empty, 0));
        prules.push_back(std::move(pruleStats));
    }
    {
        auto dbCols = cols, selCols = cols;
        auto estimatedBuckets = nextPowerOfTwo(pred->getCount());
        patom_ptr_t source = patom_ptr_t(new PhysicalHJBuild(types, dbCols, selCols, estimatedBuckets , pred, keys, false));
        dbCols = cols; selCols = cols;
        patom_ptr_t sink = patom_ptr_t(new PhysicalNopeOutput(types, dbCols, selCols, 0 ));
        prule_ptr_t pruleBuild(new PhysicalRule(source, sink, empty, 1));
        prules.push_back(std::move(pruleBuild));
    }
    if (priority <= 1)priority = 2;
}

void PhysicalOptimizer::generatePhysicalJoin(const set_term_variable_t& vars,
                                             idx_t i, Rule& rule, patom_ptr_vector_t &patoms,
                                             prule_ptr_vector_t& prules, idx_t& priority) {

    auto& dcCols = cols_[i];
    auto& selCols = selectedCols_[i];
    auto& atom = rule.getBody()[i];
    auto& types = types_;
    auto& schema = context_.defaultSchema_;

    // try to build a join
    // calculate the join keys and conditions
    auto& terms = atom.getTerms();
    std::vector<Expression> joinConditions;
    std::unordered_map<string,idx_t> varMap; // for each variable the index term in the atom

    for (idx_t i = 0; i < terms.size(); ++i) {
        if (terms[i].isAnonymous())continue;
        BB_ASSERT(terms[i].getType() == VARIABLE);
        auto variable = terms[i].getVariable();
        varMap[variable] = i;
        if (!vars.contains(variable)) continue;
        // variable is a join variable
        BB_ASSERT(colsMap_.contains(variable));
        joinConditions.emplace_back(Expression::generateExpression(EQUAL, colsMap_[variable], i ));
        // remove from selCols and dcCols the keys column as are duplicate columns
        auto index = std::distance(selCols.begin(), std::find(selCols.begin(), selCols.end(), i));
        std::erase(selCols, i);
        dcCols.erase(dcCols.begin() + index);
    }

    for (idx_t j = i+1;j < rule.getBody().size();++j) {
        auto& nextAtom = rule.getBody()[j];
        if (nextAtom.getType() != BUILTIN)break; // If a classical atom is found, stop the process as all possible built-ins have been evaluated
        if (nextAtom.getBinop() == ASSIGNMENT) continue;
        // check the size of conditions, for now is allowed only simplified condition
        // TODO allow expression with arith
        auto &left = nextAtom.getTerms()[0];
        auto &right = nextAtom.getTerms()[1];
        if (left.getType() != VARIABLE || right.getType() != VARIABLE) continue;
        auto& lvar = left.getVariable();
        auto& rvar = right.getVariable();

        if (varMap.contains( rvar) && varMap.contains( lvar)) {
            // skip this binop as the condition does not involve left side
            continue;
        }
        if (varMap.contains( rvar) ) {
            BB_ASSERT(colsMap_.contains(lvar));
            joinConditions.emplace_back(Expression::generateExpression(nextAtom.getBinop(), colsMap_[lvar], varMap[rvar] ));
        } else {
            // right var point to left join
            // then swap the condition to set the left index in left side and right index in right side
            BB_ASSERT(colsMap_.contains(rvar));
            BB_ASSERT(varMap.contains(lvar));
            joinConditions.emplace_back(Expression::generateExpression(getFlippedBinop(nextAtom.getBinop()), colsMap_[rvar], varMap[lvar] ));
        }
        BB_ASSERT(colsMap_.contains(lvar));
        BB_ASSERT(colsMap_.contains(rvar));
        skipAtom_[j] = true; // skip the creation of physical atom j
    }


    auto pred = schema.getPredicateTable(atom.getPredicate()).get();
    if (joinConditions.size() == 0 ) {
        // cross product join
        auto cp = patom_ptr_t(new PhysicalCrossProduct(types, dcCols, selCols, 0, pred ));
        patoms.push_back(std::move(cp));
        return;
    }
    // check if hash join is possible finding if there are common variables
    std::vector<idx_t> keys; // keys on current predicate
    std::vector<idx_t> dcKeys; // keys in the input data chunk of the hash patom
    for (auto& condition: joinConditions)
        if (condition.op_ == EQUAL && condition.right_.cols_.size() == 1) {
            BB_ASSERT(condition.left_.cols_.size() == 1);
            keys.push_back(condition.right_.cols_[0]);
            dcKeys.push_back(condition.left_.cols_[0]);
        }

    if (keys.size() == 0) {
        // no hash join possible
        // TODO create sort merge join instead of nested loop
        auto nj = patom_ptr_t(new PhysicalNestedLoop(types, dcCols, selCols, 0, pred, joinConditions ));
        patoms.push_back(std::move(nj));
        return;
    }
    // hash join possible, check if the hash table is present

    if (!pred->existJoinHashTable(keys) ) {
        // we need to build the hash table
        generateHTBuildRules(pred, keys, prules, priority);
    }
    // check if the hash table is ready, otherwise we need to set the priority >= 2
    if (!pred->getJoinHashTable(keys).isReady())
        priority = (priority < 2)? 2 : priority;

    auto nj = patom_ptr_t(new PhysicalHashJoin(types, dcCols, selCols, 0, pred, keys, dcKeys, joinConditions ));
    patoms.push_back(std::move(nj));

}

prule_ptr_vector_t PhysicalOptimizer::createPhysicalRules(Rule &rule) {
    // simple optimizer that takes the first atom as source, head as sink
    // and accept only filters in body

    prule_ptr_vector_t prules;
    patom_ptr_t source;
    patom_ptr_t sink;
    patom_ptr_vector_t patoms;
    set_term_variable_t vars;
    idx_t priority = 0;
    auto& schema = context_.defaultSchema_;
    {
        BB_ASSERT(rule.getBody().size() > 0);
        auto& firstAtom = rule.getBody()[0];
        firstAtom.getVariables(vars);
        auto& ptSource = schema.getPredicateTable(firstAtom.getPredicate());
        auto types = types_;
        source = patom_ptr_t(new PhysicalChunkScan(types, cols_[0], selectedCols_[0], ptSource->getCount(), ptSource.get()));
    }
    {
        BB_ASSERT(rule.getHead().size() == 1);
        auto& headAtom = rule.getHead()[0];
        auto& ptSink = schema.getPredicateTable(headAtom.getPredicate());
        auto types = types_;
        sink = patom_ptr_t(new PhysicalChunkOutput(types, headCols_[0], 0, ptSink.get()));
    }
    for (idx_t i=1;i<rule.getBody().size();++i) {
        if (skipAtom_.contains(i) && skipAtom_[i])continue;
        auto& atom = rule.getBody()[i];
        switch (atom.getType()) {
            case BUILTIN:
                generatePhysicalExpression(atom, cols_[i], types_, patoms);
                break;
            case CLASSICAL:
                // find the physical join
                generatePhysicalJoin(vars, i, rule, patoms, prules, priority);
                break;
        }
        atom.getVariables(vars);

    }

    prule_ptr_t prule(new PhysicalRule(source, sink, patoms, priority));
    prules.push_back(prule);
    return prules;
}

void PhysicalOptimizer::clear() {
    cols_.clear();
    headCols_.clear();
    types_.clear();
    colsMap_.clear();
    typesMap_.clear();
    selectedCols_.clear();
}
}
