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
#include "bumblebee/execution/PartitionedAggHT.hpp"
#include <bit>

#include "bumblebee/ClientContext.hpp"
#include "bumblebee/common/Helper.hpp"
#include "bumblebee/common/Profiler.hpp"

namespace bumblebee{
PartitionedAggHT::PartitionedAggHT(ClientContext& context, const vector<idx_t>& groupCols,const vector<idx_t> &payloadCols,
    const vector<AggregateFunction*>& functions,idx_t estimatedSourceCardinality, idx_t partitions)
    : context_(context), ready_(false), partitions_(partitions), groupCols_(groupCols), payloadCols_(payloadCols),
        functions_(functions), partitionEntries_(partitions), partitionsMutex_(partitions), estimatedInputCardinality_(estimatedSourceCardinality){
    BB_ASSERT(partitions_ != 0 && (partitions_ & (partitions_ - 1)) == 0); // partitions_ should be power of 2
    BB_ASSERT(payloadCols_.size() == functions_.size());
    shift_ = (sizeof(hash_t)*8) - std::bit_width(partitions_) + 1;
    pAggHts_.resize(partitions_);
    pDistinctHts_.resize(partitions_);
    for (idx_t i = 0; i < partitions_; i++)
        partitionEntries_[i].store(0);
}

void PartitionedAggHT::finalize() {
    for (auto& aht: pAggHts_) {
        if (!aht) continue;
        if (!table_) {
            // init the final table
            table_ = std::move(aht);
            aht = nullptr;
            continue;
        }
        table_->combine(*aht);
        aht = nullptr; // free memory
    }
    ready_ = true;
}



struct PartitionInfo {
    PartitionInfo (idx_t size): sel_(size), size_(0){
    };

    SelectionVector sel_;
    idx_t size_;
};


void PartitionedAggHT::initialize(DataChunk &chunk) {
    lock_guard lock(mutex_);
    if (!initialized_) {
        types_ = chunk.getTypes();
        for (idx_t i = 0; i < payloadCols_.size(); i++) {
            auto col = payloadCols_[i];
            BB_ASSERT(col < types_.size());
            payloadColsType_.push_back(types_[col]);
        }
        for (auto& col: groupCols_) {
            BB_ASSERT(col < types_.size());
            groupColsType_.push_back(types_[col]);
        }
    }
    initialized_ = true;
}

void PartitionedAggHT::addChunk(DataChunk &chunk) {

    Vector hash(LogicalTypeId::HASH, chunk.getSize());
    chunk.hash(hash);

    // first find the partitions
    vector<PartitionInfo> partitionsInfo;
    auto size = chunk.getSize();
    for (idx_t i=0;i<partitions_;++i)
        partitionsInfo.emplace_back(size);

    BB_ASSERT(hash.getType() == PhysicalType::UBIGINT);
    auto hashesPtr = FlatVector::getData<hash_t>(hash);
    for (idx_t i = 0; i < size; ++i) {
        auto h = hashesPtr[i];
        auto partition = h >> shift_;
        BB_ASSERT(partition < partitionsInfo.size());
        auto &info = partitionsInfo[partition];
        info.sel_.setIndex(info.size_++, i);
    }

    // check if we need to initialize the partitions vec
    if (!initialized_) {
        initialize(chunk);
    }
    BB_ASSERT(getTypes() == chunk.getTypes());

    Vector paddresses(LogicalTypeId::ADDRESS, chunk.getSize());
    SelectionVector sel(chunk.getSize());
    DataChunk groups, payloads;
    groups.initializeEmpty(groupColsType_);
    payloads.initializeEmpty(payloadColsType_);

    for (idx_t p=0;p<partitionsInfo.size();++p) {
        // insert into the partitions
        auto& pi = partitionsInfo[p];
        if (pi.size_ == 0) continue;

        partitionEntries_[p] += pi.size_;
        DataChunk pchunk;
        Vector phash(hash);
        pchunk.initAndReference(chunk);
        // slice the chunk with the partition data
        pchunk.slice(pi.sel_, pi.size_);
        phash.slice(pi.sel_, pi.size_);

        lock_guard  lock(partitionsMutex_[p]);
        if (!pAggHts_[p]) {
            // init partition ht and agg ht
            auto htInitSize = (estimatedInputCardinality_ > 0)
                                ?nextPowerOfTwo(estimatedInputCardinality_ / partitionsInfo.size()) * 2 // *2to make the HT to be 50% free
                                : HT_INIT_CAPACITY;
            if (distinct_)
                pDistinctHts_[p] = distinct_ht_ptr_t(new PRLHashTable(*context_.bufferManager_, pchunk.getTypes(), htInitSize ));
            else if(groupCols_.empty())
                // no groups so will collapse in one group
                htInitSize = 2;

            pAggHts_[p] = agg_ht_ptr_t(new AggregatePRLHashTable(*context_.bufferManager_, groupColsType_, htInitSize, true, functions_));
        }
        if (distinct_) {
            // find the new data to add to the agg ht
            idx_t newRows = 0;
            pDistinctHts_[p]->findOrCreateGroups(phash, pchunk, paddresses, newRows, sel);
            if (newRows == 0) continue;
            pchunk.slice(sel, newRows);
        }
        // insert the new data into the agg ht
        groups.reference(pchunk, groupCols_);
        payloads.reference(pchunk, payloadCols_);
        Vector ghash(LogicalTypeId::HASH, groups.getSize());
        groups.hash(ghash);
        pAggHts_[p]->addChunk(ghash, groups, payloads);
    }
}

bool PartitionedAggHT::checkGroups(const vector<idx_t> &groups) {
    return compareVectors(groups, groupCols_);
}

bool PartitionedAggHT::checkPayload(const vector<idx_t> &payload, const vector<AggregateFunction *> &functions) {
    // check the functions of the payloads are the same
    std::unordered_map<idx_t, AggregateFunction*> payloadFunc; // map of paylaod col and agg function name
    BB_ASSERT(functions_.size() == payloadCols_.size());
    for (id_t i=0;i<functions_.size();++i)
        payloadFunc[payloadCols_[i]] = functions_[i];
    for (idx_t i=0;i<functions.size();++i) {
        if (!payloadFunc.contains( payload[i])) return false;
        if (functions_[i] != payloadFunc[payload[i]]) return false;
    }
    return true;
}

void PartitionedAggHT::merge(idx_t partition, agg_ht_ptr_t localHt) {
    BB_ASSERT(partition < partitions_);
    if (!localHt || localHt->getSize() == 0) return;

    // Initialize types if not already done
    if (!initialized_) {
        lock_guard lock(mutex_);
        if (!initialized_) {
            for (auto* func : functions_)
                types_.push_back(func->arguments_[0]);
            initialized_ = true;
        }
    }

    lock_guard lock(partitionsMutex_[partition]);
    if (!pAggHts_[partition]) {
        // First merge: move the local HT to be the partition HT
        pAggHts_[partition] = std::move(localHt);
        return;
    }
    pAggHts_[partition]->combine(*localHt);
}

}
