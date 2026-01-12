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
#include "bumblebee/execution/atom/join/PhysicalCrossProduct.hpp"

namespace bumblebee{

class CrossProductState : public  PhysicalAtomState {
public:
    CrossProductState() = default;

    void reset() {
        rightIdx_ = 0;
        cacheIdx_ = 0;
        cacheChunk_.setCardinality(0);
    }

    // Load the cache chunk and return true if end is reached
    bool loadCache(PredicateTables* pt) {
        if (cacheIdx_ < cacheChunk_.getSize())
            return false;

        // cache the next right chunk
        cacheIdx_ = 0;
        bool finished = false;
        if (!pt->isDistinct()) {
            if (rightIdx_ < pt->chunkCount())
                cacheChunk_.reference(pt->getChunk(rightIdx_++));
            else
                finished = true;
        }else {
            BB_ASSERT(pt->existPartitionedPRLHashTable());
            auto& ht = pt->getPartitionedPRLHashTable();
            cacheChunk_.setCardinality(0);
            if (rightIdx_ < ht->getSize()) {
                ht->scan(rightIdx_, cacheChunk_);
                rightIdx_ += cacheChunk_.getSize();
            }else
                finished = true;
        }
        return finished;
    }

    idx_t rightIdx_{0};
    idx_t cacheIdx_{0};
    DataChunk cacheChunk_;
    bool isInitialized_{false};
};


PhysicalCrossProduct::PhysicalCrossProduct(const vector<LogicalType> &types,
    vector<idx_t>& dcCols,vector<idx_t>& selectedCols,
    PredicateTables *pt): PhysicalAtom(types, dcCols, selectedCols), pt_(pt) {
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
    result += "; ";
    for (auto c : dcColsType_) {
        result += c.toString() + ", ";
    }

    return result + ")";
}

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
        if (!pt_->isDistinct())
            cstate.cacheChunk_.initializeEmpty(pt_->getTypes());
        else
            cstate.cacheChunk_.initialize(pt_->getTypes());

        cstate.cacheChunk_.setCardinality(0);
    }
    if (cstate.loadCache(pt_)){
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
        auto value = cstate.cacheChunk_.getValue(selectCols_[i], cstate.cacheIdx_);
        chunk.data_[dcCols_[i]].reference(value);
    }
    ++cstate.cacheIdx_;
    context.profiler_.endPhysicalAtom(chunk);
    return AtomResultType::HAVE_MORE_OUTPUT;
}
}
