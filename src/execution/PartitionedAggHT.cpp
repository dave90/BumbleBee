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
PartitionedAggHT::PartitionedAggHT(idx_t partitions):ready_(false), partitions_(partitions) {
    BB_ASSERT(partitions_ != 0 && (partitions_ & (partitions_ - 1)) == 0); // partitions_ should be power of 2
    shift_ = (sizeof(hash_t)*8) - std::bit_width(partitions_);
}

void PartitionedAggHT::partitionAggregateHT(agg_ht_ptr ht) {
    if (ht->getSize() == 0)return;

    vector<agg_ht_ptr> partitions;
    partitions.reserve(partitions_);
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
    for (idx_t i = 0; i < partitions_; i++) {
        auto &ht = partitionsVec_[i][0];
        if (!ht)continue;
        if (!table_)
            table_ = std::move(ht);
        else
            table_->combine(*ht);
    }
    table_->finalize();
    ready_ = true;
}

void PartitionedAggHT::processPartition(idx_t partition) {
    BB_ASSERT(partition < partitions_);
    if (partitionsVec_.empty()) return;
    auto& finalPartitionHT = partitionsVec_[0][partition];
    for (idx_t i=1;i < partitions_; i++) {
        auto &ht = partitionsVec_[i][partition];
        if (!ht) continue;
        finalPartitionHT->combine(*ht);
        // clear the memory
        partitionsVec_[i][partition] = nullptr;
    }
}
}
