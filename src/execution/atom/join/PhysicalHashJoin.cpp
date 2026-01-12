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
#include "bumblebee/execution/atom/join/PhysicalHashJoin.hpp"

namespace bumblebee{

class HTJoinAtomState : public PhysicalAtomState {
public:
    HTJoinAtomState(JoinHashTable& ht):ht_(ht){};

    void reset() {
        rpos_ = 0;
        lpos_ = 0;
    }

    // data for probe phase
    JoinHashTable& ht_;
    idx_t rpos_{0};
    idx_t lpos_{0};
};


class GlobalHTJoinAtomState : public GlobalPhysicalAtomState {
public:
    JoinHashTable& ht_;

    GlobalHTJoinAtomState(JoinHashTable& ht): ht_(ht) {}

    // return the start and end buckets to build
    // return false if no buckets are available to build
    bool getNextBucketsToProcess(idx_t& start,idx_t& end) {
        // sync the function
        lock_guard lock(mutex_);
        if (!isPtInitialized_) {
            initPredicateTable();
        }
        return getNextBucket(start, end, bucketsProcessed_, bucketsSize_);
    }

    idx_t estimateMaxThreads() {
        initPredicateTable();
        idx_t maxThreads = 1;
        idx_t start, end;
        while (getNextBucket(start, end, bucketsProcessed_, bucketsSize_)) ++maxThreads;
        return maxThreads;
    }
private:
    bool isPtInitialized_{false};

    void initPredicateTable() {
        isPtInitialized_ = true;
        bucketsProcessed_ = 0;
        for (idx_t b=0; b < ht_.getBuckets();++b)
            bucketsSize_.push_back(ht_.getStats().bucketSize_[b].load());
    }



    std::mutex mutex_;
    idx_t bucketsProcessed_{0};
    vector<idx_t> bucketsSize_;
};


PhysicalHashJoin::PhysicalHashJoin(const vector<LogicalType> &types, vector<idx_t> &dcCols,
    vector<idx_t> &selectedCols,
    PredicateTables *pt, vector<idx_t> keys, vector<idx_t> payloads, vector<idx_t> lkeys,
    vector<Expression>& conditions)
        :PhysicalAtom(types, dcCols, selectedCols),
        pt_(pt),
        conditions_(std::move(conditions)),
        keys_(std::move(keys)),
        payloads_(std::move(payloads)),
        type_(PROBE),
        lkeys_(std::move(lkeys)){
}

PhysicalHashJoin::PhysicalHashJoin(const vector<LogicalType> &types, vector<idx_t> &dcCols,
    vector<idx_t> &selectedCols, PredicateTables *pt, vector<idx_t> keys, vector<idx_t> payloads,
    PhysicalHashType type):PhysicalAtom(types, dcCols, selectedCols),
    pt_(pt),
    keys_(std::move(keys)),
    payloads_(std::move(payloads)),
    type_(type) {
}

PhysicalHashJoin::~PhysicalHashJoin() {}

string PhysicalHashJoin::getName() const {
    return "PHYSICAL_HASH_JOIN";
}

string PhysicalHashJoin::toString() const {
    string result = getName() + "( "+pt_->predicate_->getName()+", ";
    for (auto k:keys_)
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
        result += c.toString() + ", ";
    }
    result += "; ";

    return result + " )";
}

pstate_ptr_t PhysicalHashJoin::getState() const {
    return pstate_ptr_t(new HTJoinAtomState(*pt_->getJoinHashTable(keys_, payloads_)));
}

idx_t PhysicalHashJoin::getMaxThreads() const {
    auto state = getGlobalState();
    auto& gstate = (GlobalHTJoinAtomState&)*state;
    return gstate.estimateMaxThreads();
}

bool PhysicalHashJoin::isSource() const {
    return type_ == BUILD;
}

bool PhysicalHashJoin::isSink() const {
    return type_ == COLLECT;
}

gpstate_ptr_t PhysicalHashJoin::getGlobalState() const {
    return gpstate_ptr_t(new GlobalHTJoinAtomState(*pt_->getJoinHashTable(keys_, payloads_)));
}

AtomResultType PhysicalHashJoin::execute(ThreadContext &context, DataChunk &input, DataChunk &chunk,
    PhysicalAtomState &state) const {
    auto& cstate = (HTJoinAtomState&)state;
    context.profiler_.startPhysicalAtom(this);

    BB_ASSERT(cstate.ht_.isReady());

    if (input.getSize() == 0) {
        context.profiler_.endPhysicalAtom(chunk);
        cstate.reset();
        chunk.reset();
        return AtomResultType::NEED_MORE_INPUT;
    }

    auto& lchunk = input;
    Vector hash(LogicalTypeId::HASH);
    lchunk.hash(hash, lkeys_);

    auto& rchunk = cstate.ht_.getDataChunk();

    auto count = 0;
    // loop until we produce output data chunk, or we complete the join
    while (!count && cstate.lpos_ < lchunk.getSize()) {

        SelectionVector lsel(STANDARD_VECTOR_SIZE);
        SelectionVector rsel(STANDARD_VECTOR_SIZE);

        count = cstate.ht_.probe(cstate.lpos_, cstate.rpos_, lchunk, hash,lsel, rsel, conditions_ );

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


void PhysicalHashJoin::finalize(ThreadContext &context, GlobalPhysicalAtomState &gstate) const {
    context.profiler_.startPhysicalAtom(this);
    auto& cgstate = (GlobalHTJoinAtomState&)gstate;

    if (type_ == BUILD) {
        // Is running as source, clean the stats and set to ready
        cgstate.ht_.clearStats();
        cgstate.ht_.setReady();
    }else if (type_ == COLLECT) {
        // running as sink, build the directory
        cgstate.ht_.initDirectory();
    }

    context.profiler_.endPhysicalAtomFinalize();
}

void PhysicalHashJoin::combine(ThreadContext &context, PhysicalAtomState &state,
    GlobalPhysicalAtomState &gstate) const {
    context.profiler_.startPhysicalAtom(this);

    auto& cgstate = (GlobalHTJoinAtomState&)gstate;
    auto& cstate = (HTJoinAtomState&)state;

    context.profiler_.endPhysicalAtom();
}

AtomResultType PhysicalHashJoin::getData(ThreadContext &context, DataChunk &chunk, PhysicalAtomState &state,
                                         GlobalPhysicalAtomState &gstate) const {
    auto& cgstate = (GlobalHTJoinAtomState&)gstate;
    context.profiler_.startPhysicalAtom(this);

    // build the hash table
    idx_t start, end;
    auto process = cgstate.getNextBucketsToProcess(start, end);
    if (!process) {
        chunk.setCardinality(0);
        context.profiler_.endPhysicalAtom(chunk);
        return AtomResultType::FINISHED;
    }
    cgstate.ht_.build(start, end);

    chunk.setCardinality(0);
    context.profiler_.endPhysicalAtom(chunk);
    return AtomResultType::FINISHED;
}

AtomResultType PhysicalHashJoin::sink(ThreadContext &context, DataChunk &input, PhysicalAtomState &state,
    GlobalPhysicalAtomState &gstate) const {
    auto& cgstate = (GlobalHTJoinAtomState&)gstate;
    auto& cstate = (HTJoinAtomState&)state;

    context.profiler_.startPhysicalAtom(this);
    if (input.getSize() == 0 ) {
        context.profiler_.endPhysicalAtom(input);
        return AtomResultType::NEED_MORE_INPUT;
    }

    // filter the columns
    auto pchunk = projectColumns(input);
    Vector hash(LogicalTypeId::HASH);
    pchunk.hash(hash, keys_);

    cgstate.ht_.addDataChunkSel(hash, pchunk);
    context.profiler_.endPhysicalAtom(input);
    return AtomResultType::NEED_MORE_INPUT;
}

}
