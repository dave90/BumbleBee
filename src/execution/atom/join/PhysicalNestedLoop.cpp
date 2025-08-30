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
#include "bumblebee/execution/atom/join/PhysicalNestedLoop.h"
#include "bumblebee/execution/NestedLoopJoin.h"

namespace bumblebee{

class NestedLoopJoinState : public  PhysicalAtomState {
public:
    NestedLoopJoinState() = default;

    void reset() {
        rpos_ = 0;
        lpos_ = 0;
        rchunk_ = 0;
    }

    idx_t rpos_{0};
    idx_t rchunk_{0};
    idx_t lpos_{0};

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
    }

    auto& lchunk = input;
    auto count = 0;
    // loop until we produce output data chunk, or we complete the join
    while (!count && cstate.rchunk_ < pt_->chunkCount()) {
        auto& rchunk = pt_->getChunk(cstate.rchunk_);

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

        if (cstate.rpos_ >= rchunk.getSize()) {
            ++cstate.rchunk_;
            cstate.rpos_ = 0;
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
