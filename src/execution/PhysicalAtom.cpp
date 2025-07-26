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

#include "bumblebee/execution/PhysicalAtom.h"

namespace bumblebee{


PhysicalAtom::PhysicalAtom(const std::vector<ConstantType> &types, idx_t estimated_cardinality): types_(types),
    estimatedCardinality_(estimated_cardinality) {
}

PhysicalAtom::PhysicalAtom(const std::vector<ConstantType> &types, std::vector<idx_t> cols,
idx_t estimated_cardinality): types_(types), estimatedCardinality_(estimated_cardinality), cols_(std::move(cols)) {
    BB_ASSERT(cols_.size() <= types_.size());
    for (auto c : cols_) colsType_.push_back(types_[c]);
}

AtomResultType PhysicalAtom::execute(ThreadContext& context, DataChunk &input, DataChunk &chunk, PhysicalAtomState &state) const {
    ErrorHandler::errorNotImplemented("Calling execute on general physical atom");
    return AtomResultType::FINISHED;
}

AtomResultType PhysicalAtom::getData(ThreadContext& context, DataChunk &chunk, PhysicalAtomState &state, GlobalPhysicalAtomState& gstate) const {
    ErrorHandler::errorNotImplemented("Calling getData on general physical atom");
    return AtomResultType::FINISHED;
}

AtomResultType PhysicalAtom::sink(ThreadContext& context, DataChunk &input, PhysicalAtomState &state, GlobalPhysicalAtomState& gstate) const {
    ErrorHandler::errorNotImplemented("Calling sink on general physical atom");
    return AtomResultType::FINISHED;
}

void PhysicalAtom::finalize(ThreadContext& context, GlobalPhysicalAtomState& gstate) const {
}

bool PhysicalAtom::isSource() const {
    return false;
}

bool PhysicalAtom::isSink() const {
    return false;
}

string PhysicalAtom::getName() const {
    return "";
}

string PhysicalAtom::toString() const {
    return "";
}

const std::vector<ConstantType> & PhysicalAtom::getTypes() const {
    return types_;
}

pstate_ptr_t PhysicalAtom::getState() const {
    return pstate_ptr_t(new PhysicalAtomState());
}

gpstate_ptr_t PhysicalAtom::getGlobalState() const {
    return gpstate_ptr_t(new GlobalPhysicalAtomState());
}

bool operator==(const PhysicalAtom &lhs, const PhysicalAtom &rhs) {
    return false;
}

bool operator!=(const PhysicalAtom &lhs, const PhysicalAtom &rhs) {
    return !(lhs == rhs);
}

idx_t PhysicalAtom::getMaxThreads() const {
    return INT_MAX;
}

void PhysicalAtom::setMaxThreads(idx_t threads) const {
}
}
