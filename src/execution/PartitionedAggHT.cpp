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
}

void PartitionedAggHT::finalize() {
}

vector<agg_ht_ptr> PartitionedAggHT::getPartitionedHT(idx_t) {
}
}
