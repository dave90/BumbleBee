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
#include "bumblebee/execution/atom/join/PhysicalHJBuild.h"

#include "CLI11.hpp"

namespace bumblebee{


class GlobalHTBuildAtomState : public GlobalPhysicalAtomState {
public:
    JoinHashTable& ht_;
    PredicateTables* pt_;

    GlobalHTBuildAtomState(JoinHashTable& ht, PredicateTables* pt): ht_(ht), pt_(pt) {}

    // return the start and end buckets to build
    // return false if no buckets are available to build
    bool getNextBucketsToProcess(idx_t& start,idx_t& end) {
        // sync the function
        lock_guard lock(mutex_);
        if (!isPtInitialized_) {
            initPredicateTable();
        }
        if (bucketsProcessed_ >= bucketsSize_.size()) {
            return false;
        }

        start = bucketsProcessed_;
        end = start;
        auto size = bucketsSize_[start];
        while (end < bucketsSize_.size() - 1) {
            size += bucketsSize_[end];
            if (size > MORSEL_SIZE) break;
            ++end;
        }
        bucketsProcessed_ = end + 1;
        return true;
    }
private:
    void initPredicateTable() {
        isPtInitialized_ = true;
        bucketsProcessed_ = 0;
        for (idx_t b=0; b < ht_.getBuckets();++b)
            bucketsSize_.push_back(ht_.getStats().bucketSize_[b].load());
    }

    std::mutex mutex_;
    bool isPtInitialized_{false};
    idx_t bucketsProcessed_{0};
    std::vector<idx_t> bucketsSize_;

};

PhysicalHJBuild::~PhysicalHJBuild() {}

PhysicalHJBuild::PhysicalHJBuild(const std::vector<ConstantType> &types, std::vector<idx_t>& dcCols, std::vector<idx_t>& selectedCols,
    idx_t estimated_cardinality, PredicateTables *pt, std::vector<idx_t> keys, bool isSink):
    PhysicalAtom(types, dcCols, selectedCols, estimated_cardinality),
    pt_(pt),
    keys_(keys),
    sink_(isSink){
}


bool PhysicalHJBuild::isSource() const {
    return !sink_;
}

bool PhysicalHJBuild::isSink() const {
    return sink_;
}

string PhysicalHJBuild::getName() const {
    return "PHYSICAL_HJ_BUILD";
}

string PhysicalHJBuild::toString() const {
    string result = getName() + "( "+pt_->predicate_->getName()+", ";
    for (auto k:keys_)
        result += std::to_string(k) + ", ";

    return result + " )";
}

gpstate_ptr_t PhysicalHJBuild::getGlobalState() const {
    return gpstate_ptr_t(new GlobalHTBuildAtomState(pt_->getJoinHashTable(keys_), pt_));
}

idx_t PhysicalHJBuild::getMaxThreads() const {
    // each thread should process one MORSEL
    return estimatedCardinality_ / MORSEL_SIZE + 1;
}

DataChunk PhysicalHJBuild::projectColumns(DataChunk &input) const{
    DataChunk newChunk;
    newChunk.initializeEmpty(colsType_);
    newChunk.reference(input, dcCols_);
    return newChunk;
}

AtomResultType PhysicalHJBuild::sink(ThreadContext &context, DataChunk &input, PhysicalAtomState &state,
        GlobalPhysicalAtomState &gstate) const {
    auto& cgstate = (GlobalHTBuildAtomState&)gstate;

    context.profiler_.startPhysicalAtom(this);
    if (input.getSize() == 0 ) {
        context.profiler_.endPhysicalAtom(input);
        return AtomResultType::NEED_MORE_INPUT;
    }

    // filter the columns
    auto pchunk = projectColumns(input);
    Vector hash(UBIGINT);
    pchunk.hash(hash, keys_);

    cgstate.ht_.addDataChunkSel(hash, pchunk);
    context.profiler_.endPhysicalAtom(input);
    return AtomResultType::NEED_MORE_INPUT;
}

AtomResultType PhysicalHJBuild::getData(ThreadContext &context, DataChunk &chunk, PhysicalAtomState &state,
    GlobalPhysicalAtomState &gstate) const {
    auto& cgstate = (GlobalHTBuildAtomState&)gstate;
    context.profiler_.startPhysicalAtom(this);

    // build the hash table
    idx_t start, end;
    auto process = cgstate.getNextBucketsToProcess(start, end);
    if (!process) {
        chunk.setCardinality(0);
        context.profiler_.endPhysicalAtom(chunk);
        return AtomResultType::FINISHED;
    }
    for (idx_t i=start; i<=end; ++i) {
        cgstate.ht_.build(i);
    }
    chunk.setCardinality(0);
    context.profiler_.endPhysicalAtom(chunk);
    return AtomResultType::FINISHED;
}

void PhysicalHJBuild::finalize(ThreadContext &context, GlobalPhysicalAtomState &gstate) const {
    auto& cgstate = (GlobalHTBuildAtomState&)gstate;
    context.profiler_.startPhysicalAtom(this);

    if (!sink_) {
        // Is running as source, clean the stats and set to ready
        cgstate.ht_.clearStats();
        cgstate.ht_.setReady();
    }else {
        // running as sink, build the directory
        cgstate.ht_.initDirectory();
    }

    context.profiler_.endPhysicalAtomFinalize();

}
}
