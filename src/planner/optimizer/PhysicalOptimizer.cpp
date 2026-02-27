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

#include "bumblebee/planner/optimizer/PhysicalOptimizer.hpp"

#include "bumblebee/catalog/PredicateTables.hpp"
#include "bumblebee/execution/Expression.hpp"
#include "bumblebee/execution/NestedLoopJoin.hpp"
#include "bumblebee/execution/atom/aggregate/PhysicalPartitionedAggHT.hpp"
#include "bumblebee/execution/atom/expression/PhysicalExpression.hpp"
#include "bumblebee/execution/atom/join/PhysicalCrossProduct.hpp"
#include "bumblebee/execution/atom/join/PhysicalHashJoin.hpp"
#include "bumblebee/execution/atom/join/PhysicalNestedLoop.hpp"
#include "bumblebee/execution/atom/output/PhysicalChunkOutput.hpp"
#include "bumblebee/execution/atom/output/PhysicalNopeOutput.hpp"
#include "bumblebee/execution/atom/scan/PhysicalChunkScan.hpp"
#include "../../include/bumblebee/execution/atom/external/PhysicalPredFunction.hpp"
#include "bumblebee/common/Log.hpp"
#include "bumblebee/common/types/Decimal.hpp"
#include "bumblebee/planner/filter/ConstantFilter.hpp"
#include "bumblebee/execution/atom/join/PhysicalRowLayoutHashJoin.hpp"
#include "bumblebee/execution/atom/output/PhysicalTopNHOutput.hpp"

namespace bumblebee {

TableFilterSet PhysicalOptimizer::extractConstantFilters(atoms_vector_t& body, const vector<string>& names) {
    TableFilterSet filters;

    // Build varName -> index map for the external atom's output columns
    std::unordered_map<string, idx_t> varToNameIdx;
    for (idx_t i = 0; i < names.size(); ++i) {
        if (names[i] == "*") return filters; // wildcard: skip all
        varToNameIdx[names[i]] = i;
    }

    // Pass 1: Build constant resolution map from constant assignments
    // e.g., C = 10 -> constantMap["C"] = Value(10)
    std::unordered_map<string, Value> constantMap;
    for (auto& atom : body) {
        if (atom.getType() != BUILTIN) continue;
        if (!atom.isConstantAssignment()) continue;
        auto& left = atom.getTerms()[0];
        auto& right = atom.getTerms()[1];
        if (left.getType() == VARIABLE && right.getType() == CONSTANT) {
            if (!varToNameIdx.contains(left.getVariable()))
                constantMap[left.getVariable()] = right.getValue().clone();
        } else if (right.getType() == VARIABLE && left.getType() == CONSTANT) {
            if (!varToNameIdx.contains(right.getVariable()))
                constantMap[right.getVariable()] = left.getValue().clone();
        }
    }

    // Pass 1b: Propagate constants through variable-to-variable equalities
    // Handles: X = 3, Y = X, K < Y  ->  constantMap["X"]=3, constantMap["Y"]=3
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto& atom : body) {
            if (atom.getType() != BUILTIN) continue;
            if (atom.isOrBuiltin()) continue;
            Binop op = atom.getBinop();
            if (op != EQUAL && op != ASSIGNMENT) continue;
            auto& terms = atom.getTerms();
            if (terms.size() != 2) continue;
            auto& left = terms[0];
            auto& right = terms[1];
            if (left.getType() != VARIABLE || right.getType() != VARIABLE) continue;
            if (constantMap.contains(left.getVariable()) && !constantMap.contains(right.getVariable())
                && !varToNameIdx.contains(right.getVariable())) {
                constantMap[right.getVariable()] = constantMap[left.getVariable()].clone();
                changed = true;
            } else if (constantMap.contains(right.getVariable()) && !constantMap.contains(left.getVariable())
                       && !varToNameIdx.contains(left.getVariable())) {
                constantMap[left.getVariable()] = constantMap[right.getVariable()].clone();
                changed = true;
            }
        }
    }

    // Pass 2: Extract filters from comparison builtins
    for (auto& atom : body) {
        if (atom.getType() != BUILTIN) continue;
        if (atom.isOrBuiltin()) continue;

        Binop op = atom.getBinop();
        if (op == ASSIGNMENT) op = EQUAL;
        if (op != EQUAL && op != UNEQUAL && op != LESS && op != GREATER &&
            op != LESS_OR_EQ && op != GREATER_OR_EQ) continue;

        auto& terms = atom.getTerms();
        if (terms.size() != 2) continue;
        auto& left = terms[0];
        auto& right = terms[1];

        // Only handle simple VARIABLE and CONSTANT terms
        if (left.getType() != VARIABLE && left.getType() != CONSTANT) continue;
        if (right.getType() != VARIABLE && right.getType() != CONSTANT) continue;

        string extVar;
        Value constVal;
        Binop finalOp = op;
        bool found = false;

        // Case 1: EXT_VAR op CONSTANT
        if (left.getType() == VARIABLE && varToNameIdx.contains(left.getVariable())
            && right.getType() == CONSTANT) {
            extVar = left.getVariable();
            constVal = right.getValue().clone();
            found = true;
        }
        // Case 2: CONSTANT op EXT_VAR (flip operator)
        else if (right.getType() == VARIABLE && varToNameIdx.contains(right.getVariable())
                 && left.getType() == CONSTANT) {
            extVar = right.getVariable();
            constVal = left.getValue().clone();
            finalOp = getFlippedBinop(op);
            found = true;
        }
        // Case 3: EXT_VAR op RESOLVED_VAR
        else if (left.getType() == VARIABLE && varToNameIdx.contains(left.getVariable())
                 && right.getType() == VARIABLE && constantMap.contains(right.getVariable())) {
            extVar = left.getVariable();
            constVal = constantMap[right.getVariable()].clone();
            found = true;
        }
        // Case 4: RESOLVED_VAR op EXT_VAR (flip operator)
        else if (right.getType() == VARIABLE && varToNameIdx.contains(right.getVariable())
                 && left.getType() == VARIABLE && constantMap.contains(left.getVariable())) {
            extVar = right.getVariable();
            constVal = constantMap[left.getVariable()].clone();
            finalOp = getFlippedBinop(op);
            found = true;
        }

        if (!found) continue;

        idx_t nameIdx = varToNameIdx[extVar];
        filters.pushFilter(nameIdx, std::make_unique<ConstantFilter>(finalOp, std::move(constVal)));
    }

    return filters;
}

PhysicalOptimizer::PhysicalOptimizer(ClientContext& context, bool recursiveRules)
    : context_(context), recursiveRules_(recursiveRules) {
}

std::unordered_map<Predicate *, vector<LogicalType>> PhysicalOptimizer::getHeadTypes(Rule &rule) {
    if (canBeSkipped(rule))return {};
    findColsAndTypes(rule);
    std::unordered_map<Predicate *, vector<LogicalType>> types;
    BB_ASSERT(rule.getHead().size() == 1);
    auto& headAtom = rule.getHead()[0];
    auto headCols = headCols_[0];
    vector<LogicalType> headTypes;
    for (auto c: headCols)
        headTypes.push_back(types_[c]);
    types[headAtom.getPredicate()] = headTypes;
    return types;
}

prule_ptr_vector_t PhysicalOptimizer::optimize(Rule &rule) {
    if (canBeSkipped(rule))return {};
    findColsAndTypes(rule);
    return createPhysicalRules(rule);
}


bool PhysicalOptimizer::canBeSkipped(Rule &rule) {
    // skip rules with classical literal with no data
    for (auto&atom : rule.getBody()) {
        switch (atom.getType()) {
            case EXTERNAL:
                continue;
            case CLASSICAL: {
                auto& pt = context_.defaultSchema_.getPredicateTable(atom.getPredicate());
                if (!atom.isNegative() && pt->getCount() == 0) return true;
                break;
            }case BUILTIN: {
                // TODO check if contains constant value comparison (1 = 0)
                break;
            }case AGGREGATE: {
                BB_ASSERT(atom.getAggsAtoms().size() == 1);
                auto& apt = context_.defaultSchema_.getPredicateTable(atom.getAggsAtoms()[0].getPredicate());
                // if agg tables size are 0
                if ( !apt->existPartitionedAggHashTable() || apt->getPartitionedAggHashTable()->getSize() == 0) return true;
                break;
            }
        }

    }
    return false;
}

void PhysicalOptimizer::findColsAndTypesBuiltin(Atom &atom) {
    BB_ASSERT(atom.getType() == BUILTIN);
    vector<string> vars;

    if (atom.isConstantAssignment()) {
        // is an ssignment with constant value
        auto& left = atom.getTerms()[0];
        auto& right = atom.getTerms()[1];
        BB_ASSERT(left.getType() == VARIABLE);
        BB_ASSERT(right.getType() == CONSTANT);
        BB_ASSERT(!colsMap_.contains(left.getVariable()));
        // we need to calculate the type of the left side
        typesMap_[left.getVariable()] = {right.getPhysicalType()};
        colsMap_[left.getVariable()] = colsMap_.size();
        vars.insert(vars.begin(),left.getVariable());
    }else if (atom.isOrBuiltin() || atom.getBinop() != ASSIGNMENT) {
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
        BB_ASSERT(!atom.isOrBuiltin());
        BB_ASSERT(atom.getBinop() == ASSIGNMENT);
        // if is assignment the left part is a new variable and we need to insert in the
        // map and type cols
        auto& left = atom.getTerms()[0];
        auto& right = atom.getTerms()[1];
        LogicalType resultType = PhysicalType::UNKNOWN; // result type of the right side operations
        if (right.getType() == VARIABLE) {
            BB_ASSERT(colsMap_.contains(right.getVariable()));
            vars.push_back(right.getVariable());
            resultType = typesMap_[right.getVariable()];
        }else {
            // expected ARITH (no constant or range)
            BB_ASSERT(right.getType() == ARITH);
            // push the variables in the same order in the arith
            int idx = 0;
            vector<LogicalType> types;
            for (idx_t i=0;i<right.getTerms().size();++i) {
                auto& t = right.getTerms()[i];
                BB_ASSERT(t.getType() == VARIABLE);
                types.push_back(typesMap_[t.getVariable()]);
                vars.push_back(t.getVariable());
            }
            resultType = Expression::getResultType(types, right.getOperators());
            // promote integer operands to DECIMAL when result is DECIMAL
            if (resultType.type() == LogicalTypeId::DECIMAL) {
                for (auto& t : right.getTerms()) {
                    auto& varType = typesMap_[t.getVariable()];
                    if (varType.type() != LogicalTypeId::DECIMAL) {
                        varType = LogicalType::createDecimal(Decimal::MAX_WIDTH_INT64, 0);
                    }
                }
            }
        }
        // if we have a diff and is unsigned set to signed
        bool diff = std::find(right.getOperators().begin(), right.getOperators().end(), Operator::MINUS) != right.getOperators().end();
        if (diff && isUnsigned(resultType))
            resultType = PhysicalType::BIGINT;

        BB_ASSERT(left.getType() == VARIABLE);
        BB_ASSERT(!colsMap_.contains(left.getVariable()));
        // we need to calculate the type of the left side
        typesMap_[left.getVariable()] = resultType;
        colsMap_[left.getVariable()] = colsMap_.size();
        vars.insert(vars.begin(),left.getVariable());
    }

    // populate the cols and types
    vector<idx_t> atomCols;
    for (auto& var : vars) {
        atomCols.push_back(colsMap_[var]);
    }
    cols_.push_back(std::move(atomCols));
    selectedCols_.emplace_back(); // push empty array as builtin does not have predicates
}

idx_t getAggPayloadIndex(Atom &atom, idx_t aggIndex) {
    BB_ASSERT(!atom.getAggTerms().empty());
    BB_ASSERT(atom.getAggTerms().size() > aggIndex);
    auto &payloadTerm = atom.getAggTerms()[aggIndex];
    BB_ASSERT(payloadTerm.getType() == VARIABLE);
    for (idx_t i = 0;i<atom.getAggsAtoms()[0].getTerms().size();i++) {
        auto &iterm = atom.getAggsAtoms()[0].getTerms()[i];
        BB_ASSERT(iterm.getType() == VARIABLE);
        if (iterm.getVariable() == payloadTerm.getVariable()) {
            return i;
        }
    }
    ErrorHandler::errorGeneric("Generic error with aggregates, payload not found");
    return 0;
}

vector<idx_t> findPayloadIndices(Atom& atom, PredicateTables* pt) {
    vector<idx_t> payloads;
    auto aggInfo = Predicate::parseAggregateInternalPredicate(pt->predicate_->getName());
    auto funcNames = atom.getAggregateFunctionNames();
    for (idx_t f = 0; f < funcNames.size(); ++f) {
        idx_t internalPayloadIndex = getAggPayloadIndex(atom, f);
        bool found = false;
        for (idx_t i = 0; i < aggInfo.funcNames_.size() && !found; i++) {
            if (aggInfo.payloads_[i] == internalPayloadIndex && aggInfo.funcNames_[i] == funcNames[f]) {
                found = true;
                payloads.push_back(i);
            }
        }
        BB_ASSERT(found);
    }
    return payloads;
}


void PhysicalOptimizer::findColsAndTypesAggregateAtom(Atom &atom) {
    BB_ASSERT(atom.getType() == AGGREGATE);
    BB_ASSERT(atom.getBinop() == ASSIGNMENT);
    BB_ASSERT(atom.getTerms().size() > 0);
    BB_ASSERT(atom.getAggsAtoms().size() == 1);
    string internalPredName = atom.getAggsAtoms()[0].getPredicate()->getName();
    BB_ASSERT(internalPredName.starts_with(Predicate::INTERNAL_PREDICATE_AGG_PREFIX));

    auto& pt = context_.defaultSchema_.getPredicateTable(atom.getAggsAtoms()[0].getPredicate());

    // Handle explicit group variables
    // Their types come from the single aggregate body predicate (always classical after AggregatesRewriter)
    if (atom.hasExplicitGroups()) {
        auto& aggBodyAtom = atom.getAggsAtoms()[0];
        BB_ASSERT(aggBodyAtom.getType() == CLASSICAL);
        for (auto& groupTerm: atom.getAggGroupTerms()) {
            BB_ASSERT(groupTerm.getType() == TermType::VARIABLE);
            BB_ASSERT(!groupTerm.isAnonymous());
            BB_ASSERT(!colsMap_.contains(groupTerm.getVariable()));
            // Find the group variable's position in the aggregate body predicate
            for (idx_t j = 0; j < aggBodyAtom.getTerms().size(); ++j) {
                if (aggBodyAtom.getTerms()[j].getType() == TermType::VARIABLE &&
                    aggBodyAtom.getTerms()[j].getVariable() == groupTerm.getVariable()) {
                    typesMap_[groupTerm.getVariable()] = pt->getTypes()[j];
                    colsMap_[groupTerm.getVariable()] = colsMap_.size();
                    break;
                }
            }
        }
    }

    cols_.emplace_back();
    auto funcNames = atom.getAggregateFunctionNames();
    auto assignmentTerms = atom.getAssignmentTerms();
    for (idx_t i = 0; i < funcNames.size(); ++i) {
        // now you need to insert the result type of the agg
        auto payloadIndex = getAggPayloadIndex(atom, i);
        BB_ASSERT(pt->getTypes().size() > payloadIndex);
        LogicalType payloadType = pt->getTypes()[payloadIndex];
        BB_ASSERT(payloadType != PhysicalType::UNKNOWN);
        auto function = context_.functionRegister_.getFunction(funcNames[i], {payloadType});
        LogicalType returnType = function->result_;
        auto& term = assignmentTerms[i]; // assignment variable
        typesMap_[term.getVariable()] = returnType;
        cols_.back().push_back(colsMap_.size()); // where to put the result of the agg
        colsMap_[term.getVariable()] = colsMap_.size();
        selectedCols_.emplace_back(); // push empty array as aggregate does not have predicates to select
    }

}

void PhysicalOptimizer::findColsAndTypesClassicalAtom(Atom &atom) {
    BB_ASSERT(atom.getType() == CLASSICAL);
    vector<idx_t> atomCols;
    vector<idx_t> prjCols;
    for (idx_t i=0;i<atom.getTerms().size();++i) {
        auto& term = atom.getTerms()[i];
        // expected variable
        BB_ASSERT(term.getType() == VARIABLE);
        if (term.isAnonymous())continue;
        if (!colsMap_.contains(term.getVariable())) {
            // first time we see this variable, will be a new column in data chunk
            // find the data types in the predicate tables
            auto& pt = context_.defaultSchema_.getPredicateTable(atom.getPredicate());
            BB_ASSERT(pt->getTypes()[i] != PhysicalType::UNKNOWN);
            typesMap_[term.getVariable()] = pt->getTypes()[i];
            colsMap_[term.getVariable()] = colsMap_.size();
        }
        atomCols.push_back(colsMap_[term.getVariable()]);
        prjCols.push_back(i);
    }
    selectedCols_.push_back(std::move(prjCols));
    cols_.push_back(std::move(atomCols));
}

void PhysicalOptimizer::findColsAndTypesExternalAtom(Atom &atom, idx_t index, atoms_vector_t& body) {
    BB_ASSERT(atom.getType() == EXTERNAL);
    vector<idx_t> atomCols;
    vector<idx_t> prjCols;
    BB_ASSERT(!externalBindData_.contains(index));

    vector<LogicalType> returnTypes;
    vector<string> names;
    bindExternalAtom(index, atom, returnTypes, names, body);

    for (idx_t i=0;i<names.size();++i) {
        auto& var = names[i];
        // expected variable
        if (!colsMap_.contains(var)) {
            // first time we see this variable, will be a new column in data chunk
            // find the data types in the predicate tables
            BB_ASSERT(returnTypes.size() > i);
            typesMap_[var] = returnTypes[i];
            colsMap_[var] = colsMap_.size();
        }
        atomCols.push_back(colsMap_[var]);
        prjCols.push_back(i);
    }
    selectedCols_.push_back(std::move(prjCols));
    cols_.push_back(std::move(atomCols));
}

void PhysicalOptimizer::bindExternalAtom(idx_t index, Atom& atom,vector<LogicalType>& returnTypes, vector<string>& names, atoms_vector_t& body){
    BB_ASSERT(!externalBindData_.contains(index));
    auto func = (PredFunction*) context_.functionRegister_.getFunction(atom.getExternalFunctionName(), atom.getInputValuesType()).get();
    auto inputTypes = atom.getInputValuesType();
    for (auto& term:atom.getTerms()) {
        if (term.isAnonymous()) continue;
        BB_ASSERT(term.getType() == VARIABLE);
        names.push_back(term.getVariable());
    }
    std::unordered_map<string, idx_t> bindVarName;
    for (auto& var:names)
        if (colsMap_.contains(var))
            bindVarName[var] = colsMap_[var];
    TableFilterSet filters = extractConstantFilters(body, names);
    auto bind = func->bindFunction_(context_, atom.getInputValues(), inputTypes, atom.getNamedParamters(), bindVarName, returnTypes, names,filters );
    externalBindData_[index] = std::move(bind);
}


void PhysicalOptimizer::findColsAndTypes(Rule &rule ) {
    // maps of variable and cols in the data chunk
    idx_t i = 0;
    for (auto& atom:rule.getBody()) {
        if (atom.getType() == CLASSICAL)
            findColsAndTypesClassicalAtom(atom);
        else if (atom.getType() == BUILTIN)
            findColsAndTypesBuiltin(atom);
        else if (atom.getType() == AGGREGATE)
            findColsAndTypesAggregateAtom(atom);
        else if (atom.getType() == EXTERNAL)
            findColsAndTypesExternalAtom(atom, i, rule.getBody());
        else
            ErrorHandler::errorNotImplemented("Optimizer: find columns implemented only for CLASSICAL, BUILTIN, AGGREGATION and EXTERNAL atoms");
        ++i;
    }
    // create the types for all the columns
    types_.clear();
    types_.resize(typesMap_.size());
    for (auto& [var, type] :typesMap_ ) {
        types_[colsMap_[var]] = type;
    }

    // find cols for the head
    idx_t index = rule.getBody().size();
    for (auto& atom : rule.getHead()) {
        BB_ASSERT(atom.getType() == CLASSICAL || atom.getType() == EXTERNAL);
        vector<idx_t> atomCols;
        for (auto& t: atom.getTerms()) {
            BB_ASSERT(t.getType() == VARIABLE);
            atomCols.push_back(colsMap_[t.getVariable()]);
        }

        if (atom.getType() == EXTERNAL) {
            vector<LogicalType> returnTypes;
            for (auto& col: atomCols) {
                BB_ASSERT(types_[col] != PhysicalType::UNKNOWN);
                returnTypes.push_back(types_[col]);
            }
            vector<string> names;
            atoms_vector_t emptyBody;
            bindExternalAtom(index, atom, returnTypes, names, emptyBody);
        }

        headCols_.push_back(std::move(atomCols));
        ++index;
    }
}

void PhysicalOptimizer::generatePhysicalAgg(Atom& atom, vector<idx_t>& cols, patom_ptr_vector_t &patoms) {

    vector<idx_t> selCols; // empty selCols
    BB_ASSERT(atom.getAggsAtoms().size() == 1);
    auto& pt = context_.defaultSchema_.getPredicateTable(atom.getAggsAtoms()[0].getPredicate());
    auto paht = pt->getPartitionedAggHashTable().get();

    vector<idx_t> payloads = findPayloadIndices(atom, pt.get());

    vector<idx_t> groupCols;
    // cols are the groups cols
    // you need to find out the shared cols (group cols)
    set_term_variable_t internalVars;
    BB_ASSERT(atom.getAggsAtoms().size() == 1);
    auto & internalVarias = atom.getAggsAtoms()[0].getTerms();
    for (auto& var:internalVarias) {
        BB_ASSERT(var.getType() == VARIABLE);
        if (colsMap_.contains(var.getVariable()))
            groupCols.push_back(colsMap_[var.getVariable()]);
    }
    // Use scan mode for explicit groups - group values come from the hash table, not from input
    bool scanMode = atom.hasExplicitGroups();
    patoms.emplace_back(new PhysicalPartitionedAggHT(context_, types_, cols,selCols,groupCols, payloads, paht, scanMode));
}

void PhysicalOptimizer::generatePhysicalAggSource(Atom& atom, vector<idx_t>& cols, patom_ptr_t& source) {
    BB_ASSERT(atom.getType() == AGGREGATE && atom.hasExplicitGroups());
    BB_ASSERT(atom.getAggsAtoms().size() == 1);

    auto& pt = context_.defaultSchema_.getPredicateTable(atom.getAggsAtoms()[0].getPredicate());
    auto paht = pt->getPartitionedAggHashTable().get();

    vector<idx_t> payloads = findPayloadIndices(atom, pt.get());

    // Find group columns
    vector<idx_t> groupCols;
    auto& internalVarias = atom.getAggsAtoms()[0].getTerms();
    for (auto& var : internalVarias) {
        BB_ASSERT(var.getType() == VARIABLE);
        if (colsMap_.contains(var.getVariable()))
            groupCols.push_back(colsMap_[var.getVariable()]);
    }

    source = patom_ptr_t(new PhysicalPartitionedAggHT(
        context_, types_, cols, groupCols, payloads, paht));
}

void PhysicalOptimizer::generatePhysicalExpression(Atom& atom, vector<idx_t>& cols,vector<LogicalType> types,patom_ptr_vector_t& patoms ) {
    BB_ASSERT(atom.getType() == BUILTIN);
    if (atom.isConstantAssignment()) {
        auto patom = patom_ptr_t(new PhysicalExpression(cols[0], atom.getTerms()[1].getValue(), types));
        patoms.push_back(std::move(patom));
        return;
    }
    vector<Expression> exprs;
    auto builtinTerms = atom.getBuiltinTerms();
    idx_t idx = 0;
    for (idx_t i=0;i<atom.getBuiltinsSize();++i) {
        auto bt = builtinTerms[i];
        Operands left, right;
        left.cols_.push_back(cols[idx++]);
        for (auto& o: bt.left.getOperators()) {
            BB_ASSERT(cols.size() > idx);
            left.cols_.push_back(cols[idx++]);
            left.operators_.push_back(o);
        }
        right.cols_.push_back(cols[idx++]);
        for (auto& o: bt.right.getOperators()) {
            BB_ASSERT(cols.size() > idx - bt.left.getOperators().size() - 1); // subtract the left iterms counter
            right.cols_.push_back(cols[idx++]);
            right.operators_.push_back(o);
        }

        // create the expression based on atom
        Expression expr(atom.getBinop(i), left, right);
        exprs.push_back(std::move(expr));
    }
    auto patom = patom_ptr_t(new PhysicalExpression(exprs, types));
    patoms.push_back(std::move(patom));
}

void PhysicalOptimizer::generateJoinRLHTBuildRules(PredicateTables* pred,
                                                   vector<idx_t>& keys, vector<idx_t>& payloads, prule_ptr_vector_t& prules, idx_t& priority) {

    auto & ht = pred->getJoinRLHashTable(keys, payloads);
    // check if the types changed
    if (!ht->checksTypes(pred->getTypes())) {
        // types changed we need to cast it
        LOG_WARNING("Warning, casting hash join of predicate: %s . Consider to specify the types for predicate table.", pred->predicate_->toString().c_str());
        RowLayoutJoinHashTable::cast(*context_.bufferManager_, ht, pred->getTypes());
    }
    predicatesRLJoins_[pred->predicate_.get()].push_back(ht.get());


    vector<idx_t> cols;
    for (idx_t i = 0; i <pred->predicate_->getArity(); i++)
        cols.push_back(i);


    auto types = pred->getTypes();

    patom_ptr_vector_t empty;
    {
        auto dbCols = cols, selCols = cols; // need to create a copy as constructor will move the data
        patom_ptr_t source = patom_ptr_t(new PhysicalChunkScan(types, dbCols, selCols, pred));

        dbCols = cols;
        selCols = cols;

        // create rl ht table
        patom_ptr_t sink = patom_ptr_t(new PhysicalRowLayoutHashJoin(context_, types, dbCols , selCols,pred, keys, payloads, COLLECT));

        prule_ptr_t prule(new PhysicalRule(source, sink, empty, 0));
        prules.push_back(std::move(prule));
    }
    if (priority == 0)priority = 1;
}


void PhysicalOptimizer::generateHTBuildRules(PredicateTables* pred,
    vector<idx_t>& keys, vector<idx_t>& payloads, prule_ptr_vector_t& prules, idx_t& priority) {

    vector<idx_t> cols;
    for (idx_t i = 0;i<pred->predicate_->getArity();++i)
        cols.push_back(i);

    auto types = pred->getTypes();

    patom_ptr_vector_t empty;
    {
        auto dbCols = cols, selCols = cols; // need to create a copy as constructor will move the data
        patom_ptr_t source = patom_ptr_t(new PhysicalChunkScan(types, dbCols, selCols, pred));

        dbCols = cols; selCols = cols;
        patom_ptr_t sink = patom_ptr_t(new PhysicalHashJoin(types, dbCols, selCols , pred, keys, payloads, COLLECT));
        prule_ptr_t pruleStats(new PhysicalRule(source, sink, empty, 0));
        prules.push_back(std::move(pruleStats));
    }
    {
        auto dbCols = cols, selCols = cols;
        patom_ptr_t source = patom_ptr_t(new PhysicalHashJoin(types, dbCols, selCols , pred, keys, payloads, BUILD));
        dbCols = cols; selCols = cols;
        patom_ptr_t sink = patom_ptr_t(new PhysicalNopeOutput(types, dbCols, selCols ));
        prule_ptr_t pruleBuild(new PhysicalRule(source, sink, empty, 1));
        prules.push_back(std::move(pruleBuild));
    }
    if (priority <= 1)priority = 2;
}

void PhysicalOptimizer::generatePhysicalExternal(const set_term_variable_t& vars, idx_t index, Rule& rule, patom_ptr_vector_t &patoms) {
    auto& dcCols = cols_[index];
    auto& selCols = selectedCols_[index];
    auto& types = types_;
    auto& atom = rule.getBody()[index];

    PredFunction* func  = (PredFunction*)context_.functionRegister_.getFunction(atom.getExternalFunctionName(), atom.getInputValuesType() ).get();
    BB_ASSERT(externalBindData_.contains(index));
    auto ext = patom_ptr_t(new PhysicalPredFunction(context_, types, dcCols, selCols, func, externalBindData_[index]));
    patoms.push_back(std::move(ext));
}

bool PhysicalOptimizer::isRowLayoutHTRuleHTCreated(Predicate* predicate, const vector<idx_t>& keys, const vector<idx_t>& payloads ) {
    for (auto& rl: predicatesRLJoins_[predicate]) {
        if (rl->checkKeysAndPayloads(keys, payloads))
            return true;
    }
    return false;
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
    vector<Expression> joinConditions;
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
        if (nextAtom.isOrBuiltin()) continue;
        // Only absorb EQUAL conditions: PhysicalRowLayoutHashJoin only uses
        // equality conditions as hash keys; non-equal conditions (NEQ, GT, etc.)
        // must remain as separate generatePhysicalExpression filter atoms.
        if (nextAtom.getBinop() != EQUAL) continue;
        for (auto &bt : nextAtom.getBuiltinTerms()) {
            auto &left = bt.left;
            auto &right = bt.right;
            if (left.getType() != VARIABLE || right.getType() != VARIABLE) continue;
            auto& lvar = left.getVariable();
            auto& rvar = right.getVariable();

            if (varMap.contains( rvar) && varMap.contains( lvar)) {
                // skip this binop as the condition does not involve left side
                continue;
            }
            if (!varMap.contains(rvar) && !varMap.contains(lvar)) {
                // Both variables are from the pipeline, not this join atom.
                // Leave this filter for generatePhysicalExpression to handle.
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
    }


    auto pred = schema.getPredicateTable(atom.getPredicate()).get();
    if (joinConditions.size() == 0 ) {
        // cross product join
        auto cp = patom_ptr_t(new PhysicalCrossProduct(types, dcCols, selCols, pred ));
        patoms.push_back(std::move(cp));
        return;
    }
    // check if hash join is possible finding if there are common variables
    vector<idx_t> keys; // keys on current predicate
    vector<idx_t> dcKeys; // keys in the input data chunk of the hash patom
    for (auto& condition: joinConditions)
        if (condition.op_ == EQUAL && condition.right_.cols_.size() == 1) {
            BB_ASSERT(condition.left_.cols_.size() == 1);
            keys.push_back(condition.right_.cols_[0]);
            dcKeys.push_back(condition.left_.cols_[0]);
        }

    if (keys.size() == 0) {
        // no hash join possible
        // TODO create sort merge join instead of nested loop
        auto nj = patom_ptr_t(new PhysicalNestedLoop(types, dcCols, selCols, pred, joinConditions ));
        patoms.push_back(std::move(nj));
        return;
    }
    // find payloads
    vector<idx_t> payloads = selCols;

    if (atom.isNegative() || recursiveRules_ || pred->isRecursive()) {
        if (atom.isNegative() && pred->getCount() == 0) {
            // negative atom with empty data, do not create patom as will be always true
            return;
        }

        BB_ASSERT(!atom.isNegative() || payloads.size() == 0 );


        // if atom is negative payloads is 0
        BB_ASSERT(!atom.isNegative() || payloads.size() == 0 );

        if (!pred->existJoinRLHashTable(keys, payloads) ||
            (pred->isRecursive() && !isRowLayoutHTRuleHTCreated(pred->predicate_.get(), keys, payloads)) ) {
            // generate also if exist because you need to add new data into the join hash table during the recursions
            // generate the build rule
            generateJoinRLHTBuildRules(pred, keys, payloads, prules, priority);
        }else if (isRowLayoutHTRuleHTCreated(pred->predicate_.get(), keys, payloads)) {
            // ht was created in this bucket so is not ready
            // then increment prio if is 0
            if (priority == 0)priority = 1;
        }

        auto nj = patom_ptr_t(new PhysicalRowLayoutHashJoin(context_, types, dcCols, selCols, pred, keys, payloads, dcKeys, atom.isNegative() ));
        patoms.push_back(std::move(nj));
        return;
    }

    if (!pred->existJoinRLHashTable(keys, payloads) ||
        (pred->isRecursive() && !isRowLayoutHTRuleHTCreated(pred->predicate_.get(), keys, payloads)) ) {
        // generate also if exist because you need to add new data into the join hash table during the recursions
        // generate the build rule
        generateJoinRLHTBuildRules(pred, keys, payloads, prules, priority);
        }else if (isRowLayoutHTRuleHTCreated(pred->predicate_.get(), keys, payloads)) {
            // ht was created in this bucket so is not ready
            // then increment prio if is 0
            if (priority == 0)priority = 1;
        }

    auto nj = patom_ptr_t(new PhysicalRowLayoutHashJoin(context_, types, dcCols, selCols, pred, keys, payloads, dcKeys, atom.isNegative() ));
    patoms.push_back(std::move(nj));

    // TODO use PhysicalHashJoin when join produce a lot or rows
    //
    //
    // // check if the hash table is present
    // if (!pred->existJoinHashTable(keys, payloads) ) {
    //     // we need to build the hash table
    //     generateHTBuildRules(pred, keys, payloads, prules, priority);
    // }
    // // check if the hash table is ready, otherwise we need to set the priority >= 2
    // if (!pred->getJoinHashTable(keys, payloads)->isReady())
    //     priority = (priority < 2)? 2 : priority;
    //
    //
    //
    // auto nj = patom_ptr_t(new PhysicalHashJoin(types, dcCols, selCols, pred, keys, payloads, dcKeys, joinConditions ));
    // patoms.push_back(std::move(nj));
}

void PhysicalOptimizer::generateOutputPhysicalAtom(Rule &rule, patom_ptr_t &sink, Schema &schema, prule_ptr_vector_t& prules, patom_ptr_t& source, idx_t priority) {
    prule_ptr_vector_t value;
    BB_ASSERT(rule.getHead().size() == 1);
    auto& headAtom = rule.getHead()[0];
    auto types = types_;
    auto headCols = headCols_[0];

    if (headAtom.getType() == EXTERNAL) {
        PredFunction* func  = (PredFunction*)context_.functionRegister_.getFunction(headAtom.getExternalFunctionName(), headAtom.getInputValuesType() ).get();
        BB_ASSERT(externalBindData_.contains(rule.getBody().size()));
        sink = patom_ptr_t(new PhysicalPredFunction(context_, types, headCols, func, externalBindData_[rule.getBody().size()] ));
        return;
    }

    string predicateName = headAtom.getPredicate()->getName();
    auto& ptSink = schema.getPredicateTable(headAtom.getPredicate());
    if (predicateName.starts_with(Predicate::INTERNAL_PREDICATE_AGG_PREFIX)) {
        // is the internal aggregate builds, set as patom the partitioned agg ht and
        // generate another rule for the build of agg ht

        auto aggInfo = Predicate::parseAggregateInternalPredicate(predicateName);
        BB_ASSERT(aggInfo.payloads_.size() == aggInfo.funcNames_.size());
        vector<LogicalType> arguments;
        for (auto p:aggInfo.payloads_) {
            BB_ASSERT(p < headCols.size());
            arguments.push_back(types[headCols[p]]);
        }
        // get the aggregate function
        vector<AggregateFunction*> aggFunctions;
        for (idx_t i = 0; i < aggInfo.funcNames_.size(); i++) {
            auto funcName = aggInfo.funcNames_[i];
            auto function = context_.functionRegister_.getFunction(funcName, {arguments[i]});
            BB_ASSERT(function);
            aggFunctions.push_back((AggregateFunction*) function.get());
        }
        // get source cardinality
        auto estimatedSourceCardinality = source->getMaxThreads() * MORSEL_SIZE;
        ptSink->createPartitionedAggHashTable(aggInfo.groups_, aggInfo.payloads_, aggFunctions, estimatedSourceCardinality);
        if (aggInfo.distinct_)
            ptSink->getPartitionedAggHashTable()->setDistinct();
        vector<idx_t> selCols;
        sink = patom_ptr_t(new PhysicalPartitionedAggHT(context_, types, headCols,selCols,ptSink.get(),aggInfo.groups_,aggInfo.payloads_, aggFunctions));

        return;
    }
    vector<LogicalType> headTypes;
    for (auto c: headCols)
        headTypes.push_back(types[c]);
    BB_ASSERT(headTypes == ptSink->getTypes());
    bool delta = recursiveRules_ && ptSink->isRecursive();

    if (ptSink->isDistinct())
        ptSink->getPartitionedPRLHashTable(); // init the HT
    if (delta)
        ptSink->initializeDelta(); // during recursion init delta tables

    if (!rule.getLimit()) {
        sink = patom_ptr_t(new PhysicalChunkOutput(context_, types, headCols, ptSink.get()));
    }else  {
        if (delta) {
            ErrorHandler::errorNotImplemented("Error, limit on recursive rule not supported.");
        }
        auto modifiers = rule.getModifiers();
        if (modifiers.empty()) {
            // create modifiers from the first var
            modifiers.push_back({.col_ = 0, .modifier_ = OrderType::ASCENDING});
        }
        sink = patom_ptr_t(new PhysicalTopNHOutput(types, headCols, ptSink.get(), rule.getModifiers(), rule.getLimit()));
    }
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

    BB_ASSERT(!rule.getBody().empty());
    auto& firstAtom = rule.getBody()[0];
    if (firstAtom.getType() == CLASSICAL) {
        firstAtom.getVariables(vars);
        auto& ptSource = schema.getPredicateTable(firstAtom.getPredicate());
        auto types = types_;
        source = patom_ptr_t(new PhysicalChunkScan(types, cols_[0], selectedCols_[0], ptSource.get()));
    }else if (firstAtom.getType() == EXTERNAL) {
        // get the function and bind it
        firstAtom.getVariables(vars);
        PredFunction* func  = (PredFunction*)context_.functionRegister_.getFunction(firstAtom.getExternalFunctionName(), firstAtom.getInputValuesType() ).get();
        source = patom_ptr_t(new PhysicalPredFunction(context_, types_,cols_[0],  selectedCols_[0], (PredFunction*) func, externalBindData_[0]));
    } else if (firstAtom.getType() == AGGREGATE && firstAtom.hasExplicitGroups()) {
        firstAtom.getVariables(vars);
        generatePhysicalAggSource(firstAtom, cols_[0], source);
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
            case AGGREGATE:
                generatePhysicalAgg(atom, cols_[i], patoms);
                break;
            case EXTERNAL:
                generatePhysicalExternal(vars, i,rule,  patoms);
                break;
        }
        atom.getVariables(vars);
    }

    generateOutputPhysicalAtom(rule, sink, schema, prules, source, priority);

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
    skipAtom_.clear();
    externalBindData_.clear();
}
}
