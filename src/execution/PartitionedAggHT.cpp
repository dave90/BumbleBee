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
    idx_t totalSize = 0;
    for (auto& aht: pAggHts_)
        if (aht)
            totalSize += aht->getSize();

    for (auto& aht: pAggHts_) {
        if (!aht) continue;
        if (!table_) {
            // init the final table
            table_ = std::move(aht);
            if (table_->capacity_ < totalSize)
                table_->resize(nextPowerOfTwo(totalSize));
            aht = nullptr;
            continue;
        }
        table_->combineUnsafe(*aht);
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
    // hash based on group cols
    Vector hash(LogicalTypeId::HASH, chunk.getSize());
    if (!groupCols_.empty())
        chunk.hash(hash, groupCols_);
    else
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
        pchunk.initAndReference(chunk);
        Vector ghash(hash);
        // slice the chunk with the partition data
        pchunk.slice(pi.sel_, pi.size_);
        ghash.slice(pi.sel_, pi.size_);

        lock_guard  lock(partitionsMutex_[p]);
        ensurePartitionAggHt(p);
        if (distinct_) {
            // calculate hash on all the columns as we need to check if the rows are unique
            Vector phash(LogicalTypeId::HASH, pchunk.getSize());
            pchunk.hash(phash);
            // find the new data to add to the agg ht
            idx_t newRows = 0;
            pDistinctHts_[p]->findOrCreateGroups(phash, pchunk, paddresses, newRows, sel);
            if (newRows == 0) continue;
            pchunk.slice(sel, newRows);
            ghash.slice(sel, newRows);
        }
        // insert the new data into the agg ht
        groups.reference(pchunk, groupCols_);
        payloads.reference(pchunk, payloadCols_);
        pAggHts_[p]->addChunk(ghash, groups, payloads);
    }
}

void PartitionedAggHT::ensurePartitionAggHt(idx_t p) {
    if (!pAggHts_[p]) {
        auto htInitSize = (estimatedInputCardinality_ > 0)
                            ? nextPowerOfTwo(estimatedInputCardinality_ / partitions_) * 2
                            : HT_INIT_CAPACITY;
        if (distinct_)
            pDistinctHts_[p] = distinct_ht_ptr_t(new PRLHashTable(*context_.bufferManager_, types_, htInitSize));
        else if (groupCols_.empty())
            htInitSize = 2;

        pAggHts_[p] = agg_ht_ptr_t(new AggregatePRLHashTable(*context_.bufferManager_, groupColsType_, htInitSize, true, functions_));
    }
}

// Per-partition batch buffers
struct PartitionBatch {
    Vector addresses{LogicalTypeId::ADDRESS};
    Vector hashes{LogicalTypeId::HASH};
    data_ptr_t* addrPtr;
    hash_t* hashPtr;
    idx_t size{0};

    PartitionBatch() {
        addrPtr = FlatVector::getData<data_ptr_t>(addresses);
        hashPtr = FlatVector::getData<hash_t>(hashes);
    }
};

void PartitionedAggHT::combineLocalHt(agg_ht_ptr_t localHt) {
    if (!localHt || localHt->getSize() == 0) return;


    vector<PartitionBatch> batches(partitions_);

    // Scan all entries from the local HT
    Vector scanAddresses(LogicalTypeId::ADDRESS);
    Vector scanHashes(LogicalTypeId::HASH);
    idx_t offset = 0;

    while (true) {
        idx_t count = localHt->scanRawEntries(offset, scanAddresses, scanHashes);
        if (count == 0) break;

        auto addrPtr = FlatVector::getData<data_ptr_t>(scanAddresses);
        auto hashPtr = FlatVector::getData<hash_t>(scanHashes);

        for (idx_t i = 0; i < count; ++i) {
            auto p = hashPtr[i] >> shift_;
            BB_ASSERT(p < partitions_);
            auto& batch = batches[p];
            batch.addrPtr[batch.size] = addrPtr[i];
            batch.hashPtr[batch.size] = hashPtr[i];
            batch.size++;

            if (batch.size >= STANDARD_VECTOR_SIZE) {
                lock_guard lock(partitionsMutex_[p]);
                ensurePartitionAggHt(p);
                pAggHts_[p]->moveAndMergeStates(batch.size, batch.addresses, batch.hashes);
                batch.size = 0;
            }
        }
    }

    // Flush remaining entries
    for (idx_t p = 0; p < partitions_; ++p) {
        auto& batch = batches[p];
        if (batch.size == 0) continue;
        lock_guard lock(partitionsMutex_[p]);
        ensurePartitionAggHt(p);
        pAggHts_[p]->moveAndMergeStates(batch.size, batch.addresses, batch.hashes);
    }

    // Transfer string heap from local HT to each partition that received entries,
    // so aggregate states referencing variable-length data remain valid.
    for (idx_t p = 0; p < partitions_; ++p) {
        if (pAggHts_[p])
            pAggHts_[p]->mergeStringHeap(*localHt);
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
