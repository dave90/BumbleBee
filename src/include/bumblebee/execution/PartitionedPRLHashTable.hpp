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

#include "PRLHashTable.hpp"
#include "bumblebee/common/TypeDefs.hpp"
#include "bumblebee/common/types/DataChunk.hpp"
#include "bumblebee/common/types/RowDataCollection.hpp"
#include "bumblebee/common/types/RowLayout.hpp"
#include "bumblebee/common/types/Vector.hpp"
#include "bumblebee/storage/BufferHandle.hpp"

namespace bumblebee{


class PartitionedPRLHashTable {
public:

    using pprl_ht_ptr_t = std::unique_ptr<PartitionedPRLHashTable>;

    // The hash table load factor, when a resize is triggered
    constexpr static float LOAD_FACTOR = 0.5;

    PartitionedPRLHashTable(BufferManager& manager, const vector<LogicalType>& types, idx_t partitions = HT_PARTITIONS);
    PartitionedPRLHashTable(BufferManager& manager, idx_t partitions = HT_PARTITIONS);
    virtual ~PartitionedPRLHashTable() {};


    PartitionedPRLHashTable(const PartitionedPRLHashTable &other) = delete;
    PartitionedPRLHashTable(PartitionedPRLHashTable &&other) noexcept = delete;
    PartitionedPRLHashTable & operator=(const PartitionedPRLHashTable &other) = delete;
    PartitionedPRLHashTable & operator=(PartitionedPRLHashTable &&other) noexcept = delete;

    void initialize(const vector<LogicalType> &types);

    bool equalTypes(const vector<LogicalType>& types) const;

    // Add a given data to HT
    void addChunk(Vector& hash, DataChunk& chunk);
    void addChunk( DataChunk& chunk, idx_t& newCount, SelectionVector& newGroupSel);
    void addChunk(DataChunk& chunk);

    // Scan the HT starting from the position until the result and group
    idx_t scan(idx_t offset, DataChunk& result, idx_t size = STANDARD_VECTOR_SIZE);

    // Combine with other HT
    virtual void combine(PartitionedPRLHashTable& other);

    idx_t getSize() const;
    vector<idx_t> getPartitionsSize() const;
    string toString(bool compact = true);
    vector<LogicalType> getTypes() const;

    void setTypes(const std::vector<LogicalType> & vector);

protected:
    void addChunkInternal(Vector &hash, DataChunk &chunk, idx_t &newGroupsCount, SelectionVector *newGroupSel);


    // Types of the columns in the hash table
    vector<LogicalType> types_;
    // Buffer manager of hash and data blocks
    BufferManager &bufferManager_;

    // The amount of entries stored in the HT currently
    atomic<idx_t> entries_;

    // shift to apply on hash table to calculate the partition
    idx_t shift_;

    // partitioned data
    vector<distinct_ht_ptr_t> partitions_;

    // mutex for each partition
    vector<mutex> partitionMutex_;

    vector<idx_t> partitionHashIndex_;


public:
    // Combine h1 with h2 using the common types. Results are in h1
    static void castAndCombine(BufferManager &manager, std::unique_ptr<PartitionedPRLHashTable> &h1, PartitionedPRLHashTable &h2);

    static void cast(BufferManager &manager, std::unique_ptr<PartitionedPRLHashTable> &h1, const vector<LogicalType> &types);
};

using partitioned_prl_ht_ptr_t = std::unique_ptr<PartitionedPRLHashTable>;

}
