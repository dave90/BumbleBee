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
#include "bumblebee/execution/PartitionedAggHT.h"
#include <bit>

namespace bumblebee{
PartitionedAggHT::PartitionedAggHT(vector<idx_t>& groupCols,vector<idx_t> &payloadCols,
    vector<AggregateFunction*>& functions, idx_t partitions)
    :ready_(false), partitions_(partitions), groupCols_(std::move(groupCols)), payloadCols_(std::move(payloadCols)), functions_(std::move(functions)) {
    BB_ASSERT(partitions_ != 0 && (partitions_ & (partitions_ - 1)) == 0); // partitions_ should be power of 2
    BB_ASSERT(payloadCols_.size() == functions_.size());
    shift_ = (sizeof(hash_t)*8) - std::bit_width(partitions_) + 1;
    partitionsAggVec_.resize(partitions_);
}

void PartitionedAggHT::partitionHT(distinct_ht_ptr_t& ht) {
    if (ht->getSize() == 0)return;

    vector<distinct_ht_ptr_t> partitions;
    partitions.resize(partitions_);
    ht->partition(partitions, shift_);
    // lock the vector
    lock_guard guard(mutex_);
    // log the statistics
    for (idx_t i = 0; i < partitions_; i++) {
        auto& ht = partitions[i];
        if (!ht)continue;
        if (!partitionEntries_.contains(i))partitionEntries_[i] = 0;
        partitionEntries_[i] += ht->getSize();
    }
    // push the partitions in the vector
    partitionsVec_.push_back(std::move(partitions));
}

void PartitionedAggHT::finalize() {
    for (auto& aht: partitionsAggVec_) {
        if (!aht) continue;
        if (!table_) {
            // init the final table
            table_ = agg_ht_ptr_t(new AggregateChunkOneHashTable(aht->getTypes(), MORSEL_SIZE, true, functions_));
        }
        table_->combine(*aht);
        aht = nullptr; // free memory
    }
    table_->finalize();
    ready_ = true;
}

void PartitionedAggHT::aggregatePartition(idx_t partition) {
    BB_ASSERT(partition < partitions_);
    if (partitionsVec_.empty()) return;
    distinct_ht_ptr_t dht;
    for (idx_t i=0;i < partitionsVec_.size(); i++) {
        auto &ht = partitionsVec_[i][partition];
        if (!ht) continue;
        if (!dht) {
            dht = std::move(ht);
            continue;
        }
        dht->combine(*ht);

        // clear the memory
        partitionsVec_[i][partition] = nullptr;
    }
    if (!dht) return; // no data for the partition

    // fetch the distinct values from final ht
    auto types = dht->getTypes();
    DataChunk result;
    result.initializeEmpty(types);
    dht->scan(0, result, dht->getSize());

    vector<ConstantType> groupColsType;
    for (auto& col: groupCols_) {
        BB_ASSERT(col < types.size());
        groupColsType.push_back(types[col]);
    }
    partitionsAggVec_[partition] = agg_ht_ptr_t(new AggregateChunkOneHashTable(groupColsType, MORSEL_SIZE, true, functions_));
    auto& pht = partitionsAggVec_[partition];

    // split the result chunk in groups and payload
    DataChunk groups, payloads;
    groups.initializeEmpty(pht->getTypes());
    groups.reference(result, groupCols_);
    vector<ConstantType> payloadTypeCols;
    for (idx_t i = 0; i < payloadCols_.size(); i++) {
        auto col = payloadCols_[i];
        BB_ASSERT(col < types.size());
        BB_ASSERT(functions_[i]->result_ == types[col]);
        payloadTypeCols.push_back(types[col]);
    }
    payloads.initializeEmpty(payloadTypeCols);
    payloads.reference(result, payloadCols_);
    Vector hash(UBIGINT, groups.getSize());
    groups.hash(hash);

    // add into the agg HT
    pht->addChunk(hash, groups, payloads);
}
}
