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
#include "bumblebee/execution/atom/join/PhysicalRowLayoutHashJoin.hpp"

#include "bumblebee/execution/RowLayoutJoinHashTable.hpp"

namespace bumblebee{



class RLJoinHTAtomState : public PhysicalAtomState {
public:
    explicit RLJoinHTAtomState(rl_join_ht_ptr_t& ht)
        : ht_(ht.get()) {
    }
    RLJoinHTAtomState() = default;

    void reset() {
        rpos_ = 0;
        lpos_ = 0;
    }

    // used for the execute
    RowLayoutJoinHashTable* ht_;


    idx_t rpos_{0};
    idx_t lpos_{0};
    idx_t currentIdx_{0};
    idx_t endIdx_{0};
};


class GlobalRLJoinHTAtomState : public GlobalPhysicalAtomState {
public:
    // local ht for the sink state that will be merged with the finalize
    RowLayoutJoinHashTable* ht_;

    GlobalRLJoinHTAtomState(rl_join_ht_ptr_t& ht): ht_(ht.get()) {}
    GlobalRLJoinHTAtomState(): ht_(nullptr) {}
};

PhysicalRowLayoutHashJoin::PhysicalRowLayoutHashJoin(const ClientContext &context, const vector<ConstantType> &types,
    vector<idx_t> &dcCols, vector<idx_t> &selectedCols, PredicateTables *pt, vector<idx_t> keys, vector<idx_t> payloads,
    vector<idx_t> lkeys, bool negative) : PhysicalAtom(types, dcCols,selectedCols),
    keys_(std::move(keys)), lkeys_(std::move(lkeys)),pt_(pt), context_(context), type_(PROBE), payloads_(std::move(payloads)), negative_(negative){
}

PhysicalRowLayoutHashJoin::PhysicalRowLayoutHashJoin(const ClientContext &context, const vector<ConstantType> &types,
    vector<idx_t> &dcCols, vector<idx_t> &selectedCols, PredicateTables *pt, vector<idx_t> keys, vector<idx_t> payloads,
    PhysicalHashType type) : PhysicalAtom(types, dcCols,selectedCols),
    keys_(std::move(keys)),pt_(pt), context_(context), type_(type), payloads_(std::move(payloads)), negative_(false){
    // in build phase (source) all columns should be keys and pt is flagged as distinct
    BB_ASSERT(type_ != BUILD || payloads_.empty() );
    BB_ASSERT(type_ != BUILD || pt_->isDistinct() );
}


PhysicalRowLayoutHashJoin::~PhysicalRowLayoutHashJoin() {}


bool PhysicalRowLayoutHashJoin::isSink() const {
    return type_ == COLLECT;
}

string PhysicalRowLayoutHashJoin::getName() const {
    return "PHYSICAL_RL_HASH_JOIN";
}

string PhysicalRowLayoutHashJoin::toString() const {
    string result = getName() + "( "+pt_->predicate_->getName()+", ";
    for (auto k:keys_)
        result += std::to_string(k) + ", ";
    result += "; ";
    for (auto k:payloads_)
        result += std::to_string(k) + ", ";
    result += "; ";
    for (auto k:lkeys_)
        result += std::to_string(k) + ", ";
    result += "; ";
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

    return result + " )";
}

pstate_ptr_t PhysicalRowLayoutHashJoin::getState() const {
    if (type_ == COLLECT)
        return pstate_ptr_t(new RLJoinHTAtomState());

    return pstate_ptr_t(new RLJoinHTAtomState(pt_->getJoinRLHashTable(keys_, payloads_)));
}

gpstate_ptr_t PhysicalRowLayoutHashJoin::getGlobalState() const {
    if (type_ == COLLECT) {
        return gpstate_ptr_t(new GlobalRLJoinHTAtomState(pt_->getJoinRLHashTable(keys_, payloads_)));
    }
    return gpstate_ptr_t(new GlobalRLJoinHTAtomState());
}

void PhysicalRowLayoutHashJoin::executeProbe(PhysicalAtomState& state, DataChunk& input, DataChunk& chunk) const {
    auto& cstate = (RLJoinHTAtomState&)state;

    DataChunk lchunk;
    lchunk.initAndReference(input, lkeys_);
    Vector hash(UBIGINT);
    lchunk.hash(hash);
    DataChunk result;
    SelectionVector lsel(STANDARD_VECTOR_SIZE),rsel(STANDARD_VECTOR_SIZE);
    cstate.ht_->probe(cstate.lpos_, cstate.rpos_, lchunk, hash,lsel, rsel, result);

    auto count = result.getSize();
    if (count > 0 ) {
        // we have output chunk to construct
        chunk.slice(input, lsel, count);
        for (idx_t i=0;i< result.columnCount();++i) {
            chunk.data_[dcCols_[i]].reference(result.data_[i]);
        }
    }
}


void PhysicalRowLayoutHashJoin::executeExist(PhysicalAtomState& state, DataChunk& input, DataChunk& chunk) const{
    auto& cstate = (RLJoinHTAtomState&)state;


    DataChunk lchunk;
    lchunk.initAndReference(input, lkeys_);
    Vector hash(UBIGINT);
    lchunk.hash(hash);
    SelectionVector mSel(lchunk.getSize());
    SelectionVector nmSel(lchunk.getSize());
    idx_t mCount =0, nmCount =0;
    cstate.ht_->match(lchunk, hash, mSel, mCount, nmSel, nmCount);
    // if is not negative slice with the matched index
    // otherwise slice with the non matched index
    if (!negative_)
        chunk.slice(input, mSel, mCount);
    else
        chunk.slice(input, nmSel, nmCount);
}

AtomResultType PhysicalRowLayoutHashJoin::execute(ThreadContext &context, DataChunk &input, DataChunk &chunk,
    PhysicalAtomState &state) const {
    auto& cstate = (RLJoinHTAtomState&)state;

    context.profiler_.startPhysicalAtom(this);
    if (input.getSize() == 0) {
        chunk.reset();
        cstate.reset();
        context.profiler_.endPhysicalAtom(chunk);
        return AtomResultType::NEED_MORE_INPUT;
    }

    if (!payloads_.empty())
        executeProbe(state, input, chunk);
    else
        executeExist(state, input, chunk);

    context.profiler_.endPhysicalAtom(chunk);

    if (cstate.lpos_ >= input.getSize() || payloads_.empty()) {
        // we completed the join with current chunk, request new input
        cstate.reset();
        return AtomResultType::NEED_MORE_INPUT;
    }
    return AtomResultType::HAVE_MORE_OUTPUT;
}

AtomResultType PhysicalRowLayoutHashJoin::sink(ThreadContext &context, DataChunk &input, PhysicalAtomState &state,
    GlobalPhysicalAtomState &gstate) const {
    auto& cgstate = (GlobalRLJoinHTAtomState&)gstate;

    context.profiler_.startPhysicalAtom(this);

    if (input.getSize() == 0) {
        context.profiler_.endPhysicalAtom(input);
        return AtomResultType::NEED_MORE_INPUT;
    }
    BB_ASSERT(input.getSize() > 0);

    DataChunk pinput = projectColumns(input);

    cgstate.ht_->addChunk(pinput);
    context.profiler_.endPhysicalAtom(input);
    return AtomResultType::HAVE_MORE_OUTPUT;
}

void PhysicalRowLayoutHashJoin::finalize(ThreadContext &context, GlobalPhysicalAtomState &gstate) const {
    if (type_ != COLLECT) return;
    auto& cstate = (GlobalRLJoinHTAtomState&)gstate;
    context.profiler_.startPhysicalAtom(this);
    cstate.ht_->finalize();
    BB_ASSERT(cstate.ht_->isReady());
    context.profiler_.endPhysicalAtomFinalize();
}

}
