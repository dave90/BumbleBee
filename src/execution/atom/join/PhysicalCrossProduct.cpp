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
#include "bumblebee/execution/atom/join/PhysicalCrossProduct.h"

namespace bumblebee{

class CrossProductState : public  PhysicalAtomState {
public:
    CrossProductState() = default;

    void reset() {
        rightIdx_ = 0;
    }

    idx_t rightIdx_{0};
    bool isInitialized_{false};
};


PhysicalCrossProduct::PhysicalCrossProduct(const std::vector<ConstantType> &types,
    std::vector<idx_t>& dcCols,std::vector<idx_t>& selectedCols, idx_t estimated_cardinality,
    PredicateTables *pt): PhysicalAtom(types, dcCols, selectedCols, estimated_cardinality), pt_(pt) {
    BB_ASSERT(dcCols_.size() == selectCols_.size());
}

string PhysicalCrossProduct::getName() const {
    return "PHYSICAL_CROSS_PRODUCT_JOIN";
}

string PhysicalCrossProduct::toString() const {
    auto result = getName();
    result += " (" + pt_->predicate_.get()->toString()+"; ";
    for (auto c : dcCols_) {
        result += std::to_string(c) + ", ";
    }
    result += "; ";
    for (auto c : selectCols_) {
        result += std::to_string(c) + ", ";
    }
    for (auto c : colsType_) {
        result += ctypeToString(c) + ", ";
    }
    return result + ")";}

pstate_ptr_t PhysicalCrossProduct::getState() const {
    return pstate_ptr_t(new CrossProductState());
}

AtomResultType PhysicalCrossProduct::execute(ThreadContext &context, DataChunk &input, DataChunk &chunk,
    PhysicalAtomState &state) const {
    context.profiler_.startPhysicalAtom(this);
    auto& cstate = (CrossProductState&)state;
    if (input.getSize() == 0) {
        context.profiler_.endPhysicalAtom(chunk);
        cstate.reset();
        chunk.reset();
        return AtomResultType::NEED_MORE_INPUT;
    }
    if (!cstate.isInitialized_) {
        pt_->initializeChunks();
        cstate.isInitialized_ = true;
    }
    if (cstate.rightIdx_ >= pt_->getCount()) {
        context.profiler_.endPhysicalAtom(chunk);
        cstate.reset();
        chunk.reset();
        return AtomResultType::NEED_MORE_INPUT;
    }
    auto& leftChunk = input;
    chunk.setCardinality(leftChunk.getSize());
    for (idx_t i = 0; i < leftChunk.columnCount(); ++i) {
        chunk.data_[i].reference(leftChunk.data_[i]);
    }
    // find the row of right side and assign as constant
    for (idx_t i = 0; i < selectCols_.size(); ++i) {
        BB_ASSERT(pt_->predicate_->getArity() > selectCols_[i]);
        BB_ASSERT(chunk.columnCount() > dcCols_[i] );
        auto value = pt_->getValue(selectCols_[i], cstate.rightIdx_);
        chunk.data_[dcCols_[i]].reference(value);
    }
    ++cstate.rightIdx_;
    context.profiler_.endPhysicalAtom(chunk);
    if (cstate.rightIdx_ < pt_->getCount())
        return AtomResultType::HAVE_MORE_OUTPUT;
    cstate.reset();
    return AtomResultType::NEED_MORE_INPUT;
}
}
