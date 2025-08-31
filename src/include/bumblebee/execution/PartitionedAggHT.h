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
#pragma once
#include <memory>

#include "AggregateChunkOneHashTable.h"
#include "bumblebee/common/Mutex.h"
#include "bumblebee/common/Vector.h"

namespace bumblebee{

using agg_ht_ptr_t = AggregateChunkOneHashTable::agg_ht_ptr_t;

// Inspired by : https://db.in.tum.de/~leis/papers/morsels.pdf
class PartitionedAggHT {
public:

    static constexpr idx_t PARTITIONS = 64; // consider that in Agg HT last bit is always 1

    explicit PartitionedAggHT(const vector<idx_t> &groupCols,const vector<idx_t>& payloadCols,const vector<AggregateFunction*>& functions, idx_t partitions = PARTITIONS);
    PartitionedAggHT(PartitionedAggHT &&other) noexcept;
    PartitionedAggHT & operator=(PartitionedAggHT &&other) noexcept;

    PartitionedAggHT & operator=(const PartitionedAggHT &other) = delete;
    PartitionedAggHT(const PartitionedAggHT &other) = delete;

    // partition the HT and push the partitioned HT in the partition vector
    void partitionHT(distinct_ht_ptr_t& ht);
    // compute the final aggregate HT
    void finalize();
    // merge the HT in the same partitions (no lock are used, so each thread should process different partitions)
    void aggregatePartition(idx_t partition);
    // merge the partitions
    void combinePartitions(idx_t start,idx_t end);

    idx_t getPartitionSize(idx_t partition) {
        return partitionEntries_[partition];
    }

    idx_t getNumPartitions() {
        return partitions_;
    }

    idx_t getNumPartitionsNotEmpty() {
        return partitionEntries_.size();
    }

    bool isReady() {
        return ready_;
    }

    agg_ht_ptr_t& getAggregateHT() {
        return table_;
    }

    vector<ConstantType> getTypes() {
        return types_;
    }

    idx_t getSize() {
        BB_ASSERT(isReady());
        return (table_)?table_->getSize() : 0;
    }

    bool checkGroups(const vector<idx_t>& groups);
    bool checkPayload(const vector<idx_t>& payload, const vector<AggregateFunction*>& functions);

private:
    // the final Aggregate HT table
    agg_ht_ptr_t table_;
    // vector of distinct HT
    vector<vector<distinct_ht_ptr_t>> partitionsVec_;
    // Aggregate HT for each partition
    vector<agg_ht_ptr_t> partitionsAggVec_;
    // number of partitions
    idx_t partitions_;
    // shift to apply on hash table to calculate the partition
    idx_t shift_;
    // if the table_ is ready
    bool ready_;
    // mutex of the partitioned HT
    mutex mutex_;
    // statistics of entries for each partition
    std::unordered_map<idx_t, idx_t> partitionEntries_;

    // Aggregate Group columns
    vector<idx_t> groupCols_;
    // Payload cols
    vector<idx_t> payloadCols_;
    // Aggregates functions
    vector<AggregateFunction*> functions_;

    vector<ConstantType> types_;
};

using partitioned_agg_ht_ptr_t = std::unique_ptr<PartitionedAggHT>;

}
