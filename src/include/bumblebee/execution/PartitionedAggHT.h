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

#include "AggregateHashTable.h"
#include "bumblebee/common/Mutex.h"
#include "bumblebee/common/Vector.h"

namespace bumblebee{

using agg_ht_ptr = AggregateHashTable::agg_ht_ptr;

class PartitionedAggHT {
public:

    static constexpr idx_t PARTITIONS = 64; // consider that in Agg HT last bit is always 1

    explicit PartitionedAggHT(idx_t partitions = PARTITIONS);
    PartitionedAggHT(const PartitionedAggHT &other) = delete;
    PartitionedAggHT(PartitionedAggHT &&other) noexcept = delete;
    PartitionedAggHT & operator=(const PartitionedAggHT &other) = delete;
    PartitionedAggHT & operator=(PartitionedAggHT &&other) noexcept = delete;

    // partition the agg HT and push the partitioned HT in the partition vector
    void partitionAggregateHT(agg_ht_ptr ht);
    // compute the final aggregate HT
    void finalize();
    // merge the HT in the same partitions (no lock are used, so each thread should process different partitions)
    void processPartition(idx_t partition);


    idx_t getPartitionSize(idx_t partition) {
        return partitionEntries_[partition];
    }

    bool isReady() {
        return ready_;
    }

private:
    // the final HT table
    agg_ht_ptr table_;
    // vector of partitioned agg HT
    vector<vector<agg_ht_ptr>> partitionsVec_;
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
};


}
