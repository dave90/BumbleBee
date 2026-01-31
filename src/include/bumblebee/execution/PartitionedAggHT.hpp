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

#include "bumblebee/common/Mutex.hpp"
#include "bumblebee/common/Vector.hpp"
#include "bumblebee/execution/AggregatePRLHashTable.hpp"

namespace bumblebee{

using agg_ht_ptr_t = AggregatePRLHashTable::agg_ht_ptr_t;

/*
 * Class PartitionedAggHT
 * Partitioned driver for grouped and global aggregation based on PRLHashTable and AggregatePRLHashTable
 *
 * Overview
 * - Splits a stream of distinct hash tables into power of two partitions and aggregates each partition independently
 * - Uses high hash bits to choose the destination partition and preserves row layout including aggregate state columns
 * - Enables parallelism by allowing different threads to process different partitions without locks
 *
 *
 * Aggregation per partition
 * - aggregatePartition(p) merges all PRLHashTable instances that landed in partition p into a single PRLHashTable
 * - Builds an AggregatePRLHashTable for that partition using the group column types, capacity of the merged distinct table, and the provided aggregate functions
 * - Scans the merged distinct table in vectors, splits each chunk into groups and payloads by column index, hashes groups, and updates aggregate states
 * - No locks are taken inside this method, different partitions must be processed by different threads
 *
 * Concurrency
 * - partitionHT uses a mutex to protect shared bookkeeping when pushing new partition vectors and updating statistics
 * - aggregatePartition and combinePartitions assume disjoint partition indices are processed by different threads and therefore avoid locking
 *
 * Inspired by : https://db.in.tum.de/~leis/papers/morsels.pdf
 */

class PartitionedAggHT {
public:

    static constexpr idx_t PARTITIONS = 8;

    explicit PartitionedAggHT(ClientContext& context, const vector<idx_t> &groupCols,
        const vector<idx_t>& payloadCols, const vector<AggregateFunction*>& functions,
        idx_t estimatedSourceCardinality, idx_t partitions = PARTITIONS);

    PartitionedAggHT & operator=(const PartitionedAggHT &other) = delete;
    PartitionedAggHT(const PartitionedAggHT &other) = delete;

    // compute the final aggregate HT
    void finalize();
    void initialize(DataChunk &chunk);
    // add a chunk into the partitioned agg ht
    void addChunk(DataChunk& chunk);

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

    vector<LogicalType> getTypes() {
        return types_;
    }

    void setDistinct(bool distinct = true) {
        distinct_ = distinct;
    }

    idx_t getSize() {
        BB_ASSERT(isReady());
        return (table_)?table_->getSize() : 0;
    }

    bool checkGroups(const vector<idx_t>& groups);
    bool checkPayload(const vector<idx_t>& payload, const vector<AggregateFunction*>& functions);

    // Merge a local aggregate HT into a specific partition (takes ownership)
    void merge(idx_t partition, agg_ht_ptr_t localHt);

    // Check if this is a total aggregation (no groups and not distinct)
    bool isTotalAggregation() const { return groupCols_.empty() && !distinct_; }

    // Get the aggregate functions for creating thread-local HTs
    const vector<AggregateFunction*>& getFunctions() const { return functions_; }

private:
    const ClientContext& context_;

    // the final Aggregate HT table
    agg_ht_ptr_t table_;

    // Aggregate HT for each partition
    vector<agg_ht_ptr_t> pAggHts_;
    // number of partitions
    idx_t partitions_;
    // if the table_ is ready
    bool ready_;
    // vector of distinct HT
    vector<distinct_ht_ptr_t> pDistinctHts_;
    // statistics of entries for each partition
    vector<atomic<idx_t>> partitionEntries_;
    // mutex of the partitioned HT
    mutex mutex_;
    vector<mutex> partitionsMutex_;
    atomic<bool> initialized_{false};

    // shift to apply on hash table to calculate the partition
    idx_t shift_;
    // Aggregate Group columns
    vector<idx_t> groupCols_;
    vector<LogicalType> groupColsType_;
    // Payload cols
    vector<idx_t> payloadCols_;
    vector<LogicalType> payloadColsType_;
    // Aggregates functions
    vector<AggregateFunction*> functions_;
    // Statistics of the input data (more or less expected input). Note it is a upper bound not exact cardinality
    idx_t estimatedInputCardinality_;
    // Types of the agg ht
    vector<LogicalType> types_;
    // If we need to calculate distinct of values
    bool distinct_{false};
};

using partitioned_agg_ht_ptr_t = std::unique_ptr<PartitionedAggHT>;

}
