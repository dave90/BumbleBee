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

#include "bumblebee/execution/PhysicalAtom.hpp"

namespace bumblebee{



PhysicalAtom::PhysicalAtom(const vector<ConstantType> &types, vector<idx_t>& dcCols, vector<idx_t>& selectedCols): types_(types),
    dcCols_(std::move(dcCols)),
    selectCols_(std::move(selectedCols)) {
    BB_ASSERT(dcCols_.size() <= types_.size());
    for (auto c : dcCols_) dcColsType_.push_back(types_[c]);
}

PhysicalAtom::PhysicalAtom(const vector<ConstantType> &types): types_(types){}


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

void PhysicalAtom::combine(ThreadContext &context, PhysicalAtomState &state, GlobalPhysicalAtomState &gstate) const {
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

const vector<ConstantType> & PhysicalAtom::getTypes() const {
    return types_;
}

pstate_ptr_t PhysicalAtom::getState() const {
    return pstate_ptr_t(new PhysicalAtomState());
}

gpstate_ptr_t PhysicalAtom::getGlobalState() const {
    return gpstate_ptr_t(new GlobalPhysicalAtomState());
}

DataChunk PhysicalAtom::selectColumns(DataChunk &chunk) const {
    vector<ConstantType> selectColsType;
    for (auto& i:selectCols_)
        selectColsType.push_back(chunk.data_[i].getType());
    DataChunk newChunk;
    newChunk.initializeEmpty(selectColsType);
    newChunk.reference(chunk, selectCols_);
    return newChunk;
}

DataChunk PhysicalAtom::projectColumns(DataChunk &input) const{
    DataChunk newChunk;
    newChunk.initializeEmpty(dcColsType_);
    newChunk.reference(input, dcCols_);
    return newChunk;
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

bool GlobalPhysicalAtomState::getNextBucket(idx_t &start, idx_t &end, idx_t &current, vector<idx_t> &size) {
    if (current >= size.size()) {
        return false;
    }

    auto s = size[0];
    start = current;
    end = start;
    while (end < size.size() - 1) {
        if (s + size[end+1] > MORSEL_SIZE) break;
        ++end;
        s += size[end];
    }
    current = end + 1;
    return true;
}
}
