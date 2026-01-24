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

namespace bumblebee{
PartitionedAggHT::PartitionedAggHT(ClientContext& context, const vector<idx_t>& groupCols,const vector<idx_t> &payloadCols,
    const vector<AggregateFunction*>& functions, idx_t partitions)
    : context_(context), ready_(false), partitions_(partitions), groupCols_(groupCols), payloadCols_(payloadCols), functions_(functions), partitionEntries_(partitions){
    BB_ASSERT(partitions_ != 0 && (partitions_ & (partitions_ - 1)) == 0); // partitions_ should be power of 2
    BB_ASSERT(payloadCols_.size() == functions_.size());
    shift_ = (sizeof(hash_t)*8) - std::bit_width(partitions_) + 1;
    partitionsAggVec_.resize(partitions_);
    for (idx_t i = 0; i < partitions_; i++)
        partitionEntries_[i].store(0);
}


void PartitionedAggHT::partitionHT(distinct_ht_ptr_t& ht) {
    if (ht->getSize() == 0) return;


    vector<distinct_ht_ptr_t> partitions;
    partitions.resize(partitions_);
    vector<LogicalType> types = ht->getTypes();
    ht->partition(partitions, shift_);
    // log the statistics
    for (idx_t i = 0; i < partitions_; i++) {
        auto& ht = partitions[i];
        if (!ht)continue;
        partitionEntries_[i] += ht->getSize();
    }
    // lock the vector
    lock_guard guard(mutex_);
    if (types_.empty()) {
        types_ = types;
    }
    // push the partitions in the vector
    partitionsVec_.push_back(std::move(partitions));
}

void PartitionedAggHT::finalize() {
    for (auto& aht: partitionsAggVec_) {
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

void PartitionedAggHT::aggregatePartition(idx_t partition) {
    BB_ASSERT(partition < partitions_);
    if (partitionsVec_.empty()) return;
    distinct_ht_ptr_t dht = nullptr;;
    for (idx_t i=0;i < partitionsVec_.size(); i++) {
        BB_ASSERT(partition < partitionsVec_[i].size());
        auto &ht = partitionsVec_[i][partition];
        if (!ht) continue;
        if (!dht) {
            dht = std::move(ht);
            partitionsVec_[i][partition] = nullptr;
            continue;
        }
        // combine larger with smallest
        if (dht->getSize() > ht->getSize())
            dht->combine(*ht);
        else {
            ht->combine(*dht);
            dht = std::move(ht);
        }

        // clear the memory
        partitionsVec_[i][partition] = nullptr;
    }
    if (!dht) return; // no data for the partition
    auto dhtCapacity = dht->getCapacity();
    auto& pht = partitionsAggVec_[partition];
    auto types = dht->getTypes();
    vector<LogicalType> groupColsType;
    for (auto& col: groupCols_) {
        BB_ASSERT(col < types.size());
        groupColsType.push_back(types[col]);
    }
    partitionsAggVec_[partition] = agg_ht_ptr_t(new AggregatePRLHashTable(*context_.bufferManager_, groupColsType, dhtCapacity, true, functions_));
    vector<LogicalType> payloadTypeCols;
    for (idx_t i = 0; i < payloadCols_.size(); i++) {
        auto col = payloadCols_[i];
        BB_ASSERT(col < types.size());
        payloadTypeCols.push_back(types[col]);
    }

    idx_t position = 0;
    DataChunk groups, payloads;
    groups.initializeEmpty(pht->getTypes());
    payloads.initializeEmpty(payloadTypeCols);
    DataChunk result;
    result.initialize(types);
    Vector hash(LogicalTypeId::HASH);

    while (position <= dht->getSize()) {
        idx_t toScan = minValue<idx_t>(STANDARD_VECTOR_SIZE, dht->getSize());
        // fetch the distinct values from final ht
        dht->scan(position, result, toScan);

        // split the result chunk in groups and payload
        groups.reference(result, groupCols_);
        payloads.reference(result, payloadCols_);
        groups.hash(hash);
        // add into the agg HT
        pht->addChunk(hash, groups, payloads);
        position += STANDARD_VECTOR_SIZE;
    }
}

void PartitionedAggHT::combinePartitions(idx_t start, idx_t end) {
    BB_ASSERT(start >= 0 && end < partitionsAggVec_.size());
    int partitionIndex = -1;
    for (idx_t i=start; i<=end; i++) {
        if (!partitionsAggVec_[i])continue;
        if (partitionIndex < 0) {
            partitionIndex = i;
            continue;
        }
        // merge large partition with smaller
        if (partitionsAggVec_[partitionIndex]->getSize() >partitionsAggVec_[i]->getSize()) {
            partitionsAggVec_[partitionIndex]->combine(*partitionsAggVec_[i]);
            partitionsAggVec_[i] = nullptr;
        }else {
            partitionsAggVec_[i]->combine(*partitionsAggVec_[partitionIndex]);
            partitionsAggVec_[partitionIndex] = nullptr;
            partitionIndex = (int)i;
        }
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

}
