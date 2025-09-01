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
#include "bumblebee/execution/atom/aggregate/PhysicalPartitionedAggHT.h"



namespace bumblebee{
class PartitionedAggHTJoinAtomState : public PhysicalAtomState {
public:
    PartitionedAggHTJoinAtomState() = default;

    // ht used to build the partitioned ht
    distinct_ht_ptr_t ht_;
};



class GlobalAggHTJoinAtomState : public GlobalPhysicalAtomState {
public:
    PartitionedAggHT &pht_;

    GlobalAggHTJoinAtomState(PartitionedAggHT& pht): pht_(pht) {}

    // return the start and end partition to build
    // return false if no partition are available to build
    bool getNextpartitionToProcess(idx_t& start,idx_t& end) {
        // sync the function
        lock_guard lock(mutex_);
        if (!isPtInitialized_) {
            initPredicateTable();
        }
        return getNextBucket(start, end, partitionProcessed_, partitionSize_);
    }
private:
    void initPredicateTable() {
        isPtInitialized_ = true;
        partitionProcessed_ = 0;
        for (idx_t b=0; b < pht_.getNumPartitions(); ++b)
            partitionSize_.push_back(pht_.getPartitionSize(b));
    }

    std::mutex mutex_;
    bool isPtInitialized_{false};
    idx_t partitionProcessed_{0};
    vector<idx_t> partitionSize_;
};


PhysicalPartitionedAggHT::PhysicalPartitionedAggHT(const vector<ConstantType> &types, vector<idx_t> &dcCols,
    vector<idx_t> &selectedCols, PredicateTables *pt, const vector<idx_t> &group_cols,
    const vector<idx_t> &payload_cols, const vector<AggregateFunction *> &aggregate_functions,
    PhysicalHashType type): PhysicalAtom(types, dcCols, selectedCols),
                                pt_(pt),
                                groupCols_(group_cols),
                                payloadCols_(payload_cols),
                                aggregateFunctions_(aggregate_functions),
                                type_(type),
                                aht_(nullptr){
    for (auto& i: groupCols_)
        groupColsTypes_.push_back(types_[i]);
    for (auto& i: payloadCols_)
        payloadColsTypes_.push_back(types_[i]);
}

PhysicalPartitionedAggHT::PhysicalPartitionedAggHT(const vector<ConstantType> &types, vector<idx_t> &dcCols,
    vector<idx_t> &selectedCols, const vector<idx_t> &group_cols, const vector<idx_t> &payload_cols,
    AggregateChunkOneHashTable *aht): PhysicalAtom(types, dcCols, selectedCols) ,
                                    aht_(aht),
                                    pt_(nullptr),
                                    groupCols_(group_cols),
                                    payloadCols_(payload_cols),
                                    type_(PROBE){
    for (auto& i: groupCols_)
        groupColsTypes_.push_back(types_[i]);
}

idx_t PhysicalPartitionedAggHT::getMaxThreads() const {
    BB_ASSERT(pt_->existPartitionedAggHashTable());
    auto& pht = pt_->getPartitionedAggHashTable();
    return pht->getNumPartitionsNotEmpty(); // max parallelism 1 partition x thread
}

bool PhysicalPartitionedAggHT::isSource() const {
    return type_ == BUILD;
}

bool PhysicalPartitionedAggHT::isSink() const {
    return type_ == COLLECT;
}

string PhysicalPartitionedAggHT::getName() const {
    return "PHYSICAL_PARTITIONED_AGGREGATE";
}

string PhysicalPartitionedAggHT::toString() const {
    string result = getName() + "( ";
    if (pt_)
        result += string(pt_->predicate_->getName())+", ";
    for (auto k:groupCols_)
        result += std::to_string(k) + ", ";
    result += "; ";
    for (auto k:payloadCols_)
        result += std::to_string(k) + ", ";
    result += "; ";
    for (auto k:aggregateFunctions_)
        result += k->name_ + ", ";
    result += "; ";
    for (auto c : dcCols_) {
        result += std::to_string(c) + ", ";
    }
    result += "; ";
    for (auto c : types_) {
        result += ctypeToString(c) + ", ";
    }
    return result + " )";
}

pstate_ptr_t PhysicalPartitionedAggHT::getState() const {
    return pstate_ptr_t(new PartitionedAggHTJoinAtomState());
}

gpstate_ptr_t PhysicalPartitionedAggHT::getGlobalState() const {
    auto& paht = pt_->createPartitionedAggHashTable(groupCols_, payloadCols_, aggregateFunctions_);
    BB_ASSERT(!paht->isReady()); // during build or collect we do not expect is ready
    return gpstate_ptr_t(new GlobalAggHTJoinAtomState(*paht));
}


AtomResultType PhysicalPartitionedAggHT::getData(ThreadContext &context, DataChunk &chunk, PhysicalAtomState &state,
    GlobalPhysicalAtomState &gstate) const {
    context.profiler_.startPhysicalAtom(this);
    auto& cgstate = (GlobalAggHTJoinAtomState&)gstate;
    // build the partitioned hash table
    idx_t start, end;
    auto process = cgstate.getNextpartitionToProcess(start, end);
    if (!process) {
        chunk.setCardinality(0);
        context.profiler_.endPhysicalAtom(chunk);
        return AtomResultType::FINISHED;
    }
    for (idx_t i=start; i<=end; ++i) {
        if (cgstate.pht_.getPartitionSize(i) == 0)continue;
        cgstate.pht_.aggregatePartition(i);
    }
    // merge the partitions
    cgstate.pht_.combinePartitions(start, end);

    chunk.setCardinality(0);
    context.profiler_.endPhysicalAtom(chunk);
    return AtomResultType::FINISHED;


}

AtomResultType PhysicalPartitionedAggHT::sink(ThreadContext &context, DataChunk &input, PhysicalAtomState &state,
    GlobalPhysicalAtomState &gstate) const {
    context.profiler_.startPhysicalAtom(this);
    auto& cgstate = (GlobalAggHTJoinAtomState&)gstate;
    auto& cstate = (PartitionedAggHTJoinAtomState&)state;

    // init ht if null
    if (!cstate.ht_) {
        cstate.ht_ = distinct_ht_ptr_t(new ChunkOneHashTable(dcColsType_, MORSEL_SIZE, false));
    }
    if (input.getSize() == 0 && cstate.ht_->getSize() == 0) {
        context.profiler_.endPhysicalAtom(input);
        return AtomResultType::NEED_MORE_INPUT;
    }
    if (input.getSize() == 0
        || (((float)input.getSize() + (float)cstate.ht_->getSize() ) / (float)cstate.ht_->getCapacity()) > ChunkOneHashTable::LOAD_FACTOR ) {
        // flush the ht to partitioned agg table
        cgstate.pht_.partitionHT(cstate.ht_);
        cstate.ht_ = distinct_ht_ptr_t(new ChunkOneHashTable(dcColsType_, MORSEL_SIZE, false));
    }

    if (input.getSize() == 0) {
        context.profiler_.endPhysicalAtom(input);
        return AtomResultType::NEED_MORE_INPUT;
    }
    DataChunk sinput = projectColumns(input);
    Vector hash(UBIGINT, input.getSize());
    sinput.hash(hash);
    cstate.ht_->addChunk(hash, sinput);

    return AtomResultType::HAVE_MORE_OUTPUT;
}

void PhysicalPartitionedAggHT::finalize(ThreadContext &context, GlobalPhysicalAtomState &gstate) const {
    auto& cgstate = (GlobalAggHTJoinAtomState&)gstate;
    context.profiler_.startPhysicalAtom(this);

    if (type_ == COLLECT) {
        // populate the type columns of the predicate table
        pt_->setTypes(cgstate.pht_.getTypes());
    }
    if (type_ == BUILD) {
        // create the final aggregate hash table
        cgstate.pht_.finalize();
    }
    context.profiler_.endPhysicalAtomFinalize();

}

AtomResultType PhysicalPartitionedAggHT::execute(ThreadContext &context, DataChunk &input, DataChunk &chunk,
    PhysicalAtomState &state) const {
    context.profiler_.startPhysicalAtom(this);

    BB_ASSERT(aht_ && aht_->isReady());
    BB_ASSERT(payloadCols_.size() == 1);
    BB_ASSERT(dcCols_.size() == payloadCols_.size());

    // groupCols contains the group columns to select in the input chunk
    // payload cols contains one index and is the payload to extract
    // dcCols contains the result column where to put the result

    idx_t payloadInternalCol = payloadCols_[0];

    // used for total aggregations (no groups)
    if (groupCols_.empty()) {
        // no group to fetch, call directly the fetchAggregate with the result chunk
        Vector result(Value(0).cast(chunk.data_[dcCols_[0]].getType())); // init with 0 value, will be updated wih the fetchAggregates
        aht_->fetchAggregates(result, payloadInternalCol);
        // reference in the return chunk
        chunk.data_[dcCols_[0]].reference(result);
        chunk.setCardinality(1);

        context.profiler_.endPhysicalAtom(chunk);
        return AtomResultType::NEED_MORE_INPUT;
    }

    if (input.getSize() == 0) {
        chunk.reset();
        context.profiler_.endPhysicalAtom(chunk);
        return AtomResultType::NEED_MORE_INPUT;
    }

    DataChunk group;
    group.initializeEmpty(groupColsTypes_);
    group.reference(input, groupCols_);

    Vector hash(UBIGINT, group.getSize());
    group.hash(hash);
    Vector result(dcColsType_[0], STANDARD_VECTOR_SIZE);

    SelectionVector sel(group.getSize());
    aht_->fetchAggregates(hash, group, result, payloadInternalCol, sel);

    if (group.getSize() == 0) {
        chunk.reset();
        context.profiler_.endPhysicalAtom(chunk);
        return AtomResultType::NEED_MORE_INPUT;
    }

    // reference in the return chunk
    BB_ASSERT(chunk.columnCount() > dcCols_[0]);
    chunk.reference(input);
    chunk.slice(sel, group.getSize());

    chunk.data_[dcCols_[0]].reference(result);
    context.profiler_.endPhysicalAtom(chunk);
    return AtomResultType::NEED_MORE_INPUT;
}

}
