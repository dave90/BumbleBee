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
#include "bumblebee/execution/atom/join/PhysicalPRLHashJoin.hpp"

namespace bumblebee{



class PRLJoinHTAtomState : public PhysicalAtomState {
public:
    PRLJoinHTAtomState(JoinPRLHashTable& ht):ht_(ht){};

    void reset() {
        rpos_ = 0;
        lpos_ = 0;
    }

    JoinPRLHashTable& ht_;
    // used for the execute
    idx_t rpos_{0};
    idx_t lpos_{0};
    // used for the scan
    bool isInitialized_{false};
    idx_t currentIdx_{0};
    idx_t endIdx_{0};
    // used for the sink
    join_prl_ht_ptr_t htState_;
};


class GlobalPRLJoinHTAtomState : public GlobalPhysicalAtomState {
public:
    // local ht for the sink state that will be merged with the finalize
    join_prl_ht_ptr_t htState_;
    std::mutex mutex_;

    GlobalPRLJoinHTAtomState(PredicateTables* pt): pt_(pt) {}
    GlobalPRLJoinHTAtomState(PredicateTables* pt, join_prl_ht_ptr_t& ht): pt_(pt), htState_(std::move(ht)) {}

    // return the start and end of data to read
    bool getNextChunksToRead(idx_t& start,idx_t& end) {
        // sync the function
        lock_guard lock(mutex_);
        if (!isPtInitialized_) {
            initPredicateTable();
        }
        if (currentOffset_>=pt_->getCount())
            return false;
        start = currentOffset_;
        end = minValue( pt_->getCount(), start + MORSEL_SIZE-1);
        currentOffset_ += MORSEL_SIZE;
        return true;
    }
private:
    void initPredicateTable() {
        isPtInitialized_ = true;
        pt_->initializeChunks();
    }
    PredicateTables* pt_;

    bool isPtInitialized_{false};
    idx_t currentOffset_{0};

};

PhysicalPRLHashJoin::PhysicalPRLHashJoin(const ClientContext &context, const vector<ConstantType> &types,
    vector<idx_t> &dcCols, vector<idx_t> &selectedCols, PredicateTables *pt, vector<idx_t> keys,
    vector<idx_t> lkeys, bool negative) : PhysicalAtom(types, dcCols,selectedCols),
    keys_(std::move(keys)), lkeys_(std::move(lkeys)),pt_(pt), context_(context), type_(PROBE), negative_(negative){
    initialize();
    BB_ASSERT(!negative || (keys_.size() == pt_->predicate_->getArity() && payloads_.empty()) );
}

PhysicalPRLHashJoin::PhysicalPRLHashJoin(const ClientContext &context, const vector<ConstantType> &types,
    vector<idx_t> &dcCols, vector<idx_t> &selectedCols, PredicateTables *pt, vector<idx_t> keys,
    PhysicalHashType type) : PhysicalAtom(types, dcCols,selectedCols),
    keys_(std::move(keys)),pt_(pt), context_(context), type_(type){
    initialize();
    // in build phase (source) all columns should be keys and pt is flagged as distinct
    BB_ASSERT(type_ != BUILD || payloads_.empty() );
    BB_ASSERT(type_ != BUILD || pt_->isDistinct() );
}

PhysicalPRLHashJoin::PhysicalPRLHashJoin(const ClientContext &context, const vector<ConstantType> &types,
    vector<idx_t> &dcCols, PredicateTables *pt): PhysicalAtom(types), pt_(pt), context_(context),type_(COLLECT) {
    dcCols_ = std::move(dcCols);
    for (auto c : dcCols_)dcColsType_.push_back(types_[c]);

    BB_ASSERT(dcCols_.size() == pt_->predicate_->getArity());
    // insert all the index as keys
    for (idx_t i=0;i<pt->predicate_->getArity();++i)
        keys_.push_back(i);
}

PhysicalPRLHashJoin::PhysicalPRLHashJoin(const ClientContext &context, const vector<ConstantType> &types,
    vector<idx_t> &dcCols, vector<idx_t> &selectedCols, PredicateTables *pt):
    PhysicalAtom(types, dcCols,selectedCols), type_(BUILD),pt_(pt), context_(context){
    // insert all the index as keys
    for (idx_t i=0;i<pt->predicate_->getArity();++i)
        keys_.push_back(i);
}

void PhysicalPRLHashJoin::initialize() {
    // payload columns are not key columns
    std::unordered_set<idx_t> keySet;
    keySet.insert(keys_.begin(), keys_.end());
    for (auto& col : selectCols_)
        if (!keySet.contains(col))
            payloads_.push_back(col);

}

PhysicalPRLHashJoin::~PhysicalPRLHashJoin() {}

idx_t PhysicalPRLHashJoin::getMaxThreads() const {
    BB_ASSERT(payloads_.empty());
    return pt_->getCount() / MORSEL_SIZE + 1;
}

bool PhysicalPRLHashJoin::isSource() const {
    return type_ == BUILD;
}

bool PhysicalPRLHashJoin::isSink() const {
    return type_ == COLLECT;
}

string PhysicalPRLHashJoin::getName() const {
    return "PHYSICAL_PRL_HASH_JOIN";
}

string PhysicalPRLHashJoin::toString() const {
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

pstate_ptr_t PhysicalPRLHashJoin::getState() const {
    if (type_ == COLLECT) {
        // create a new ht that will be merged in global state
        auto ht = join_prl_ht_ptr_t(new JoinPRLHashTable(*context_.bufferManager_,dcColsType_,keys_,payloads_));
        auto state = pstate_ptr_t(new PRLJoinHTAtomState( *ht));
        // put the ownership of the ht to the state
        ((PRLJoinHTAtomState&)*state).htState_ = std::move(ht);
        return state;
    }
    auto& ht = pt_->getJoinPRLHashTable( keys_, payloads_);
    return pstate_ptr_t(new PRLJoinHTAtomState(*ht));
}

gpstate_ptr_t PhysicalPRLHashJoin::getGlobalState() const {
    if (type_ == COLLECT) {
        // create a new ht that will be merged in predicate tables with the finalize
        auto ht = join_prl_ht_ptr_t(new JoinPRLHashTable(*context_.bufferManager_,dcColsType_,keys_,payloads_));
        return gpstate_ptr_t(new GlobalPRLJoinHTAtomState(pt_, ht));
    }
    return gpstate_ptr_t(new GlobalPRLJoinHTAtomState(pt_));
}

void PhysicalPRLHashJoin::executeProbe(PhysicalAtomState& state, DataChunk& input, DataChunk& chunk) const {
    auto& cstate = (PRLJoinHTAtomState&)state;

    DataChunk lchunk;
    lchunk.initAndReference(input, lkeys_);
    Vector hash(UBIGINT);
    lchunk.hash(hash);
    DataChunk result;
    SelectionVector lsel(STANDARD_VECTOR_SIZE),rsel(STANDARD_VECTOR_SIZE);
    cstate.ht_.probe(cstate.lpos_, cstate.rpos_, lchunk, hash,lsel, rsel, result);

    auto count = result.getSize();
    if (count > 0 ) {
        // we have output chunk to construct
        chunk.slice(input, lsel, count);
        for (idx_t i=0;i< result.columnCount();++i) {
            chunk.data_[dcCols_[i]].reference(result.data_[i]);
        }
    }
}

void PhysicalPRLHashJoin::executeExist(PhysicalAtomState& state, DataChunk& input, DataChunk& chunk) const{
    auto& cstate = (PRLJoinHTAtomState&)state;


    DataChunk lchunk;
    lchunk.initAndReference(input, lkeys_);
    Vector hash(UBIGINT);
    lchunk.hash(hash);
    SelectionVector mSel(lchunk.getSize());
    SelectionVector nmSel(lchunk.getSize());
    idx_t mCount =0, nmCount =0;
    cstate.ht_.match(lchunk, hash, mSel, mCount, nmSel, nmCount);
    // if is not negative slice with the matched index
    // otherwise slice with the non matched index
    if (!negative_)
        chunk.slice(input, mSel, mCount);
    else
        chunk.slice(input, nmSel, nmCount);
}

AtomResultType PhysicalPRLHashJoin::execute(ThreadContext &context, DataChunk &input, DataChunk &chunk,
    PhysicalAtomState &state) const {
    auto& cstate = (PRLJoinHTAtomState&)state;

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

    if (cstate.lpos_ > input.getSize() || payloads_.empty()) {
        // we completed the join with current chunk, request new input
        cstate.reset();
        return AtomResultType::NEED_MORE_INPUT;
    }
    return AtomResultType::HAVE_MORE_OUTPUT;
}

AtomResultType PhysicalPRLHashJoin::getData(ThreadContext &context, DataChunk &chunk, PhysicalAtomState &state,
    GlobalPhysicalAtomState &gstate) const {
    auto& cstate = (PRLJoinHTAtomState&)state;
    auto& cgstate = (GlobalPRLJoinHTAtomState&)gstate;

    context.profiler_.startPhysicalAtom(this);

    if (!cstate.isInitialized_) {
        // state is not initialized, so fetch the chunks from global state
        idx_t start, end;
        if (!cgstate.getNextChunksToRead(start, end)) {
            // no more data to read, return empty Chunk
            chunk.setCardinality(0);
            context.profiler_.endPhysicalAtom(chunk);
            return AtomResultType::FINISHED;
        }
        cstate.currentIdx_ = start;
        cstate.endIdx_ = end;
        cstate.isInitialized_ = true;
    }
    if (cstate.currentIdx_ > cstate.endIdx_) {
        // no more data to read return empty chunk
        chunk.setCardinality(0);
        context.profiler_.endPhysicalAtom(chunk);
        return AtomResultType::FINISHED;
    }
    DataChunk result;
    result.initialize(cstate.ht_.getTypes());
    cstate.ht_.scan(cstate.currentIdx_, result, STANDARD_VECTOR_SIZE);
    cstate.currentIdx_ += STANDARD_VECTOR_SIZE;

    chunk.reference(result, selectCols_);
    context.profiler_.endPhysicalAtom(chunk);
    return AtomResultType::HAVE_MORE_OUTPUT;
}

AtomResultType PhysicalPRLHashJoin::sink(ThreadContext &context, DataChunk &input, PhysicalAtomState &state,
    GlobalPhysicalAtomState &gstate) const {
    auto& cstate = (PRLJoinHTAtomState&)state;
    auto& cgstate = (GlobalPRLJoinHTAtomState&)gstate;

    context.profiler_.startPhysicalAtom(this);

    if (input.getSize() == 0) {
        context.profiler_.endPhysicalAtom(input);
        return AtomResultType::NEED_MORE_INPUT;
    }
    BB_ASSERT(input.getSize() > 0);
    DataChunk pinput = projectColumns(input);
    BB_ASSERT(pinput.columnCount() == dcColsType_.size());

    cstate.ht_.addChunk(pinput);

    context.profiler_.endPhysicalAtom(pinput);
    return AtomResultType::HAVE_MORE_OUTPUT;
}

void PhysicalPRLHashJoin::finalize(ThreadContext &context, GlobalPhysicalAtomState &gstate) const {
    if (type_ != COLLECT) return;
    // merge with the predicate tables ht
    auto& cstate = (GlobalPRLJoinHTAtomState&)gstate;
    context.profiler_.startPhysicalAtom(this);
    pt_->mergeIntoDistinctHT(*cstate.htState_);
    context.profiler_.endPhysicalAtomFinalize();
}

void PhysicalPRLHashJoin::combine(ThreadContext &context, PhysicalAtomState &state,
    GlobalPhysicalAtomState &gstate) const {
    auto& cstate = (PRLJoinHTAtomState&)state;
    auto& cgstate = (GlobalPRLJoinHTAtomState&)gstate;

    context.profiler_.startPhysicalAtom(this);
    auto size = cstate.ht_.getSize();
    if (size > 0) {
        // push the data into the global state
        lock_guard lock(cgstate.mutex_);
        cgstate.htState_->combine(cstate.ht_);
        cstate.htState_ = nullptr;
    }

    DataChunk result;
    result.setCapacity(size);
    result.setCardinality(size);
    context.profiler_.endPhysicalAtom(result);
}
}
