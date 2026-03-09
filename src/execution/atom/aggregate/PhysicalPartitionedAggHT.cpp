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
#include "bumblebee/execution/atom/aggregate/PhysicalPartitionedAggHT.hpp"

namespace bumblebee{
class PartitionedAggHTJoinAtomState : public PhysicalAtomState {
public:
    PartitionedAggHTJoinAtomState() = default;

    // ht used to build the partitioned ht
    distinct_ht_ptr_t ht_;
    // thread-local aggregate HT for total aggregations (no groups, no distinct)
    agg_ht_ptr_t localAggHt_;
    // scan offset for scan mode (explicit groups or scan mode)
    idx_t scanOffset_{0};
    // data to scan (for scan mode)
    int scanCount_{0};
    // cache index within current cached scan results
    idx_t cacheIdx_{0};
    // cached groups from scan
    DataChunk cacheGroups_;
    // cached aggregate results from scan
    DataChunk cacheAggResults_;
    bool scanFinished_{false};
    bool cacheInitialized_{false};
};



class GlobalAggSourceState : public GlobalPhysicalAtomState {
public:
    GlobalAggSourceState(PartitionedAggHT* aht): aht_(aht),
        totalEntries_(aht ? aht->getSize() : 0) {}

    bool getNextScanRange(idx_t& start, idx_t& count) {
        lock_guard lock(mutex_);
        if (currentOffset_ >= totalEntries_) return false;
        start = currentOffset_;
        count = minValue(static_cast<idx_t>(MORSEL_SIZE),
                         totalEntries_ - currentOffset_);
        currentOffset_ += count;
        return true;
    }

    idx_t estimateMaxThreads() const {
        if (totalEntries_ == 0) return 1;
        return totalEntries_ / MORSEL_SIZE + 1;
    }

private:
    PartitionedAggHT* aht_;
    std::mutex mutex_;
    idx_t totalEntries_;
    idx_t currentOffset_{0};
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


PhysicalPartitionedAggHT::PhysicalPartitionedAggHT(const ClientContext& context, const vector<LogicalType> &types, vector<idx_t> &dcCols,
    vector<idx_t> &selectedCols, PredicateTables *pt, const vector<idx_t> &group_cols,
    const vector<idx_t> &payload_cols, const vector<AggregateFunction *> &aggregate_functions): PhysicalAtom(types, dcCols, selectedCols),
                                context_(context),
                                pt_(pt),
                                groupCols_(group_cols),
                                payloadCols_(payload_cols),
                                aggregateFunctions_(aggregate_functions),
                                type_(COLLECT),
                                aht_(nullptr){
    for (auto& i: groupCols_)
        groupColsTypes_.push_back(types_[i]);
    for (auto& i: payloadCols_)
        payloadColsTypes_.push_back(types_[i]);
}

PhysicalPartitionedAggHT::PhysicalPartitionedAggHT(const ClientContext& context, const vector<LogicalType> &types, vector<idx_t> &dcCols,
    vector<idx_t> &selectedCols, const vector<idx_t> &group_cols, const vector<idx_t> &payload_cols,
    PartitionedAggHT *aht, bool scanMode): PhysicalAtom(types, dcCols, selectedCols) ,
                                    context_(context),
                                    aht_(aht),
                                    pt_(nullptr),
                                    groupCols_(group_cols),
                                    payloadCols_(payload_cols),
                                    type_(PROBE),
                                    scanMode_(scanMode){
    for (auto& i: groupCols_)
        groupColsTypes_.push_back(types_[i]);
    // cache payload types to avoid repeated allocation in probe path
    cachedPayloadTypes_ = aht_->getPayloadsTypes();
}

PhysicalPartitionedAggHT::PhysicalPartitionedAggHT(const ClientContext& context,
    const vector<LogicalType> &types, vector<idx_t> &dcCols,
    const vector<idx_t> &group_cols, const vector<idx_t> &payload_cols,
    PartitionedAggHT *aht): PhysicalAtom(types),
                                context_(context), aht_(aht), pt_(nullptr),
                                groupCols_(group_cols), payloadCols_(payload_cols),
                                type_(SOURCE) {
    dcCols_ = dcCols;
    for (auto& i: groupCols_)
        groupColsTypes_.push_back(types_[i]);
    cachedPayloadTypes_ = aht_->getPayloadsTypes();
}

idx_t PhysicalPartitionedAggHT::getMaxThreads() const {
    if (type_ == SOURCE) {
        BB_ASSERT(aht_);
        GlobalAggSourceState gstate(aht_);
        return gstate.estimateMaxThreads();
    }
    // existing COLLECT logic
    BB_ASSERT(pt_->existPartitionedAggHashTable());
    auto& pht = pt_->getPartitionedAggHashTable();
    return pht->getNumPartitionsNotEmpty(); // max parallelism 1 partition x thread
}

bool PhysicalPartitionedAggHT::isSink() const {
    return type_ == COLLECT;
}

bool PhysicalPartitionedAggHT::isSource() const {
    return type_ == SOURCE;
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
        result += c.toString() + ", ";
    }
    return result + " )";
}

pstate_ptr_t PhysicalPartitionedAggHT::getState() const {
    return pstate_ptr_t(new PartitionedAggHTJoinAtomState());
}

gpstate_ptr_t PhysicalPartitionedAggHT::getGlobalState() const {
    if (type_ == SOURCE) {
        BB_ASSERT(aht_);
        return gpstate_ptr_t(new GlobalAggSourceState(aht_));
    }
    // existing COLLECT logic
    auto& paht = pt_->getPartitionedAggHashTable();
    BB_ASSERT(paht);
    BB_ASSERT(!paht->isReady()); // during build or collect we do not expect is ready
    return gpstate_ptr_t(new GlobalAggHTJoinAtomState(*paht));
}


AtomResultType PhysicalPartitionedAggHT::sink(ThreadContext &context, DataChunk &input, PhysicalAtomState &state,
    GlobalPhysicalAtomState &gstate) const {
    context.profiler_.startPhysicalAtom(this);
    auto& cgstate = (GlobalAggHTJoinAtomState&)gstate;
    auto& cstate = (PartitionedAggHTJoinAtomState&)state;

    DataChunk sinput = projectColumns(input);

    // Path 1: Total aggregation (no groups, not distinct) - thread-local accumulation
    if (cgstate.pht_.isTotalAggregation()) {
        if (!cstate.localAggHt_) {
            cstate.localAggHt_ = agg_ht_ptr_t(new AggregatePRLHashTable(
                *context_.bufferManager_, {}, 2, false, aggregateFunctions_));
        }
        DataChunk payloads;
        payloads.initializeEmpty(payloadColsTypes_);
        payloads.reference(sinput, payloadCols_);
        cstate.localAggHt_->addChunk(payloads);

        context.profiler_.endPhysicalAtom(input);
        return AtomResultType::HAVE_MORE_OUTPUT;
    }

    // Path 2: Grouped aggregation (non-distinct) - thread-local grouped HT
    if (!cgstate.pht_.isDistinct()) {
        if (!cgstate.pht_.isInitialized())
            cgstate.pht_.initialize(sinput);
        if (!cstate.localAggHt_) {
            cstate.localAggHt_ = agg_ht_ptr_t(new AggregatePRLHashTable(
                *context_.bufferManager_, cgstate.pht_.getGroupColsType(),
                HT_INIT_CAPACITY, true, aggregateFunctions_));
        }
        DataChunk groups, payloads;
        groups.initializeEmpty(groupColsTypes_);
        groups.reference(sinput, groupCols_);
        payloads.initializeEmpty(payloadColsTypes_);
        payloads.reference(sinput, payloadCols_);
        Vector hash(LogicalTypeId::HASH, sinput.getSize());
        groups.hash(hash);
        cstate.localAggHt_->addChunk(hash, groups, payloads);

        context.profiler_.endPhysicalAtom(input);
        return AtomResultType::HAVE_MORE_OUTPUT;
    }

    // Path 3: Distinct aggregation - use partitioned path directly
    cgstate.pht_.addChunk(sinput);

    context.profiler_.endPhysicalAtom(input);
    return AtomResultType::HAVE_MORE_OUTPUT;
}


void PhysicalPartitionedAggHT::finalize(ThreadContext &context, GlobalPhysicalAtomState &gstate) const {
    auto& cgstate = (GlobalAggHTJoinAtomState&)gstate;
    context.profiler_.startPhysicalAtom(this);

    if (type_ == COLLECT) {
        // populate the type columns of the predicate table
        auto htTypes = cgstate.pht_.getTypes();
        if (htTypes.size() == pt_->getTypes().size()) {
            pt_->setTypes(htTypes);
        }
        // merge the partitions if data is small
        if (cgstate.pht_.getSize() < MORSEL_SIZE)
            cgstate.pht_.finalize();
    }
    context.profiler_.endPhysicalAtomFinalize();
}

void PhysicalPartitionedAggHT::combine(ThreadContext &context, PhysicalAtomState &state,
    GlobalPhysicalAtomState &gstate) const {
    context.profiler_.startPhysicalAtom(this);
    auto& cstate = (PartitionedAggHTJoinAtomState&)state;
    auto& cgstate = (GlobalAggHTJoinAtomState&)gstate;

    if (cstate.localAggHt_) {
        if (cgstate.pht_.isTotalAggregation())
            cgstate.pht_.merge(0, std::move(cstate.localAggHt_));
        else
            cgstate.pht_.combineLocalHt(std::move(cstate.localAggHt_));
    }
    context.profiler_.endPhysicalAtomCombine();
}

AtomResultType PhysicalPartitionedAggHT::execute(ThreadContext &context, DataChunk &input, DataChunk &chunk,
    PhysicalAtomState &state) const {
    context.profiler_.startPhysicalAtom(this);

    BB_ASSERT(aht_);

    // Scan mode: iterate over hash table entries and push group values + aggregate results
    if (scanMode_) {
        return executeScanMode(context, input, chunk, state);
    }

    // Standard probe mode
    BB_ASSERT(dcCols_.size() == payloadCols_.size());

    // groupCols contains the group columns to select in the input chunk
    // payload cols contains one index and is the payload to extract
    // dcCols contains the result column where to put the result


    // used for total aggregations (no groups)
    if (groupCols_.empty()) {
        // no group to fetch, call directly the fetchAggregate with the result chunk
        chunk.reference(input);
        for (idx_t i = 0; i < payloadCols_.size(); ++i) {
            Vector result(chunk.data_[dcCols_[i]].getLogicalType(), 1);
            aht_->fetchAggregates(result, payloadCols_[i]);
            BB_ASSERT(result.getVectorType() == VectorType::CONSTANT_VECTOR);
            // reference in the return chunk
            chunk.data_[dcCols_[i]].reference(result);
        }

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

    Vector hash(LogicalTypeId::HASH, group.getSize());
    group.hash(hash);

    DataChunk result;
    result.initialize(cachedPayloadTypes_);

    SelectionVector sel(group.getSize());
    aht_->fetchAggregates(hash, group, result, sel);
    if (group.getSize() == 0) {
        chunk.reset();
        context.profiler_.endPhysicalAtom(chunk);
        return AtomResultType::NEED_MORE_INPUT;
    }
    chunk.reference(input);
    if (group.getSize() < chunk.getSize())
        chunk.slice(sel, group.getSize());
    for (idx_t i = 0; i < payloadCols_.size(); ++i) {
        chunk.data_[dcCols_[i]].reference(result.data_[payloadCols_[i]]);
    }
    context.profiler_.endPhysicalAtom(chunk);
    return AtomResultType::NEED_MORE_INPUT;
}

AtomResultType PhysicalPartitionedAggHT::executeScanMode(ThreadContext &context, DataChunk &input, DataChunk &chunk,
    PhysicalAtomState &state) const {
    auto& cstate = (PartitionedAggHTJoinAtomState&)state;

    // Initialize cache if needed
    if (!cstate.cacheInitialized_) {
        cstate.cacheGroups_.initialize(groupColsTypes_);
        cstate.cacheAggResults_.initialize(cachedPayloadTypes_);
        cstate.cacheInitialized_ = true;
    }

    // Check if we need to load more from cache (scan more from HT)
    if (cstate.cacheIdx_ >= cstate.cacheGroups_.getSize()) {
        if (cstate.scanFinished_) {
            // If scan is finished and we receive empty input, the previous aggregate is done
            // Don't reset and rescan - just return NEED_MORE_INPUT
            if (input.getSize() == 0) {
                chunk.reset();
                context.profiler_.endPhysicalAtom(chunk);
                return AtomResultType::NEED_MORE_INPUT;
            }
            // Reset scan state for next input batch - continue to scan below
            cstate.scanFinished_ = false;
            cstate.scanOffset_ = 0;
            cstate.cacheIdx_ = 0;
            cstate.cacheGroups_.setCardinality(0);
        }

        // Scan more from hash table
        cstate.cacheIdx_ = 0;
        cstate.cacheAggResults_.setCardinality(0);
        idx_t scanned = aht_->scanWithAggregates(cstate.scanOffset_, cstate.cacheGroups_, cstate.cacheAggResults_);
        cstate.scanOffset_ += scanned;
        cstate.cacheAggResults_.setCardinality(scanned);

        if (scanned == 0) {
            cstate.scanFinished_ = true;
            chunk.reset();
            context.profiler_.endPhysicalAtom(chunk);
            return AtomResultType::NEED_MORE_INPUT;
        }
    }

    // Output: reference all input columns, set cached row values as constants
    if (input.getSize() > 0) {
        // Cross product: for each cached row, output all input rows
        chunk.setCardinality(input.getSize());
        for (idx_t col = 0; col < input.columnCount(); ++col) {
            chunk.data_[col].reference(input.data_[col]);
        }

        // Set current cached group values as constants
        for (idx_t i = 0; i < groupCols_.size(); ++i) {
            auto val = cstate.cacheGroups_.getValue(i, cstate.cacheIdx_);
            chunk.data_[groupCols_[i]].reference(val);
        }

        // Set current cached aggregate result as constant
        for (idx_t i = 0; i < payloadCols_.size(); ++i) {
            auto aggVal = cstate.cacheAggResults_.getValue(payloadCols_[i], cstate.cacheIdx_);
            chunk.data_[dcCols_[i]].reference(aggVal);
        }
        cstate.cacheIdx_++;
    } else {
        // No prior input: output all cached groups and aggregates directly
        idx_t toOutput = cstate.cacheGroups_.getSize();
        chunk.setCardinality(toOutput);

        // Reference groups directly
        for (idx_t i = 0; i < groupCols_.size(); ++i) {
            chunk.data_[groupCols_[i]].reference(cstate.cacheGroups_.data_[i]);
        }

        // Reference aggregate results directly
        for (idx_t i = 0; i < payloadCols_.size(); ++i) {
            chunk.data_[dcCols_[i]].reference(cstate.cacheAggResults_.data_[payloadCols_[i]]);
        }

        // Mark all cache as consumed
        cstate.cacheIdx_ = toOutput;
    }

    context.profiler_.endPhysicalAtom(chunk);
    return AtomResultType::HAVE_MORE_OUTPUT;
}

AtomResultType PhysicalPartitionedAggHT::getData(ThreadContext &context, DataChunk &chunk,
    PhysicalAtomState &state, GlobalPhysicalAtomState &gstate) const {
    BB_ASSERT(type_ == SOURCE);
    context.profiler_.startPhysicalAtom(this);

    auto& cstate = (PartitionedAggHTJoinAtomState&)state;
    auto& cgstate = (GlobalAggSourceState&)gstate;

    // Initialize cache DataChunks (persist across calls in local state)
    if (!cstate.cacheInitialized_) {
        cstate.cacheGroups_.initialize(groupColsTypes_);
        cstate.cacheAggResults_.initialize(cachedPayloadTypes_);
        cstate.cacheInitialized_ = true;

        idx_t start, count;
        if (!cgstate.getNextScanRange(start, count)) {
            chunk.setCardinality(0);
            context.profiler_.endPhysicalAtom(chunk);
            return AtomResultType::FINISHED;
        }
        cstate.scanOffset_ = start;
        cstate.scanCount_ = (int)count;
    }

    if (cstate.scanCount_ <= 0) {
        chunk.setCardinality(0);
        context.profiler_.endPhysicalAtom(chunk);
        return AtomResultType::FINISHED;
    }

    // Scan aggregate HT for this range
    idx_t scanned = aht_->scanWithAggregates(cstate.scanOffset_, cstate.cacheGroups_,
                                              cstate.cacheAggResults_, STANDARD_VECTOR_SIZE);
    cstate.scanOffset_ += scanned;
    cstate.scanCount_ -= scanned;

    if (scanned == 0) {
        chunk.setCardinality(0);
        context.profiler_.endPhysicalAtom(chunk);
        return AtomResultType::FINISHED;
    }

    // Build output chunk: reference groups and aggregate results in target positions
    chunk.setCardinality(scanned);
    for (idx_t i = 0; i < groupCols_.size(); ++i)
        chunk.data_[groupCols_[i]].reference(cstate.cacheGroups_.data_[i]);
    for (idx_t i = 0; i < payloadCols_.size(); ++i)
        chunk.data_[dcCols_[i]].reference(cstate.cacheAggResults_.data_[payloadCols_[i]]);

    cstate.cacheGroups_.reset();
    cstate.cacheAggResults_.reset();

    context.profiler_.endPhysicalAtom(chunk);
    return AtomResultType::HAVE_MORE_OUTPUT;
}

}
