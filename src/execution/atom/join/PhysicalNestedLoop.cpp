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
#include "bumblebee/execution/atom/join/PhysicalNestedLoop.hpp"
#include "bumblebee/execution/NestedLoopJoin.hpp"

namespace bumblebee{

class NestedLoopJoinState : public  PhysicalAtomState {
public:
    NestedLoopJoinState() = default;

    void reset() {
        rpos_ = 0;
        lpos_ = 0;
        rchunk_ = 0;
        cacheChunk_.setCardinality(0);
    }


    // Load the cache chunk and return true if end is reached
    bool loadCache(PredicateTables* pt) {
        if (rpos_ < cacheChunk_.getSize())
            return false;

        // cache the next right chunk
        rpos_ = 0;
        bool finished = false;
        if (!pt->isDistinct()) {
            if (rchunk_ < pt->chunkCount())
                cacheChunk_.reference(pt->getChunk(rchunk_++));
            else
                finished = true;
        }else {
            BB_ASSERT(pt->existPartitionedPRLHashTable());
            auto& ht = pt->getPartitionedPRLHashTable();
            if (rchunk_ < ht->getSize()) {
                ht->scan(rchunk_, cacheChunk_);
                rchunk_ += cacheChunk_.getSize();
            }else
                finished = true;
        }
        return finished;
    }

    idx_t rpos_{0};
    idx_t rchunk_{0};
    idx_t lpos_{0};
    DataChunk cacheChunk_;

    bool isInitialized_{false};
};



PhysicalNestedLoop::PhysicalNestedLoop(const vector<ConstantType> &types, vector<idx_t> &dcCols,
    vector<idx_t> &selectedCols, PredicateTables *pt,
    vector<Expression>& conditions): PhysicalAtom(types, dcCols, selectedCols), pt_(pt), conditions_(std::move(conditions)) {
}

PhysicalNestedLoop::~PhysicalNestedLoop() {}

string PhysicalNestedLoop::getName() const {
    return "PHYSICAL_NESTED_LOOP";
}

string PhysicalNestedLoop::toString() const {
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
        result += ctypeToString(c) + ", ";
    }
    result += "; ";
    for (auto& e : conditions_) {
        result += std::to_string(e.left_.cols_[0]) + " "+ getBinopStr(e.op_)+ " "+ std::to_string(e.right_.cols_[0])  + ", ";
    }
    return result + ")";
}

pstate_ptr_t PhysicalNestedLoop::getState() const {
    return pstate_ptr_t(new NestedLoopJoinState());
}


AtomResultType PhysicalNestedLoop::execute(ThreadContext &context, DataChunk &input, DataChunk &chunk,
    PhysicalAtomState &state) const {

    context.profiler_.startPhysicalAtom(this);
    auto& cstate = (NestedLoopJoinState&)state;
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

    }

    auto& lchunk = input;
    auto count = 0;
    // loop until we produce output data chunk, or we complete the join
    while (!count && !cstate.loadCache(pt_)) {
        auto& rchunk = cstate.cacheChunk_;

        SelectionVector lsel(STANDARD_VECTOR_SIZE);
        SelectionVector rsel(STANDARD_VECTOR_SIZE);

        count = NestedLoopJoinInner::execute(cstate.lpos_, cstate.rpos_, lchunk, rchunk, lsel, rsel, conditions_);
        if (count > 0) {
            // we have output chunk to construct
            chunk.slice(input, lsel, count);
            // remove the keys (same columns between left and right) before the slice
            auto rSelChunk = selectColumns(rchunk);
            chunk.slice(rSelChunk, rsel, count, dcCols_);
        }

    }
    context.profiler_.endPhysicalAtom(chunk);

    if (count == 0) {
        // no output generated, ask new input
        cstate.reset();
        chunk.reset();
        return AtomResultType::NEED_MORE_INPUT;
    }
    return AtomResultType::HAVE_MORE_OUTPUT;
}

}
