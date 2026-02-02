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

#include "bumblebee/common/TypeDefs.hpp"
#include "bumblebee/common/types/DataChunk.hpp"
#include "bumblebee/common/types/RowDataCollection.hpp"
#include "bumblebee/common/types/RowLayout.hpp"
#include "bumblebee/common/types/Vector.hpp"
#include "bumblebee/storage/BufferHandle.hpp"

namespace bumblebee{

// Hash table bucket entry
struct HTEntry64 {
    uint64_t hash_;
    uint32_t pageNum_;
    uint32_t pageOffset_;
};

/*
 *  Probe Row Layout Hash Table
 *
 * This hash table stores tuples in row layout blocks managed by a BufferManager,
 * while a separate open-addressed hash array (HTEntry64) maps hash buckets to tuple
 * locations. Collisions are handled with linear probing. The table is designed for
 * high-throughput vectorized execution:
 *
 *
 * - Core method is findOrCreateGroups that optionally
 *   creates new groups, and return row addresses of the rows.
 *   Matched and/or newly created indices can be reported via
 *   SelectionVector`s.
 *
 * Storage layout
 * - Payload rows are packed into fixed-size blocks (Storage::BLOCK_SIZE) according
 *   to RowLayout. Variable-length data live in stringHeap_ (RowDataCollection)
 *   which is merged during partitioning/combining.
 * - The hash directory is an array of HTEntry64 {hash_, pageOffset_, pageNum_}.
 *   pageNum_ == 0 denotes an empty bucket. pageNum_ is 1-based to avoid the
 *   empty sentinel.
 *
 * Resizing policy
 * - The table grows by powers of two when `LOAD_FACTOR` would be exceeded or
 *   when initial capacity is insufficient. Resize rehashes the directory and preserves
 *   existing payload blocks. If `resizable_ == false`, only the initial build is allowed
 *   to set capacity (`initResize == true`).
 *
*- Partitioning/Combining:
 *   - partition redistributes entries across a set of PRLHashTables using higher
 *     hash bits. For each hash entry it computes the partition index as
 *     hash >> shift, where shift determines how many of the most significant
 *     bits are used to decide the partition.
 *   - Each partition receives its subset of rows and the associated string heap data,
 *     so partitions are self-contained and can be processed independently
 *     (e.g., in parallel or as input to distributed operators).
 *   - Buffers are flushed to the target partition once a vector-size batch fills,
 *     ensuring efficient batched movement of tuples.
 *
 * Thread-safety
 * - This structure is not thread-safe; external synchronization is required for
 *   concurrent writers/readers.
 *
 */
class PRLHashTable {
public:

    using distinct_ht_ptr_t = std::unique_ptr<PRLHashTable>;

    // The hash table load factor, when a resize is triggered
    constexpr static float LOAD_FACTOR = 0.5;


    PRLHashTable(BufferManager& manager, const vector<LogicalType>& types, idx_t capacity = HT_INIT_CAPACITY, bool resizable = true);
    PRLHashTable(BufferManager& manager);
    virtual ~PRLHashTable() {};

    void initialize(const vector<LogicalType> &types, idx_t capacity,bool resizable);

    PRLHashTable(const PRLHashTable &other) = delete;
    PRLHashTable(PRLHashTable &&other) noexcept = delete;
    PRLHashTable & operator=(const PRLHashTable &other) = delete;
    PRLHashTable & operator=(PRLHashTable &&other) noexcept = delete;

    // Add a given data to HT
    void addChunk(Vector& hash, DataChunk& chunk);

    virtual void addChunk(DataChunk& chunk);

    // Scan the HT starting from the position until the result and group
    idx_t scan(idx_t offset, DataChunk& result, idx_t size = STANDARD_VECTOR_SIZE);
    // Scan the HT and also populate the addresses vector (for subclasses that need addresses)
    idx_t scan(idx_t offset, DataChunk& result, Vector& addresses, idx_t size = STANDARD_VECTOR_SIZE);

    // Find or creates groups
    void findOrCreateGroups(Vector &hash, DataChunk &groups, Vector &addresses);
    // Find or creates groups and return the matched index ( elements not created)
    void findOrCreateGroups(Vector &hash, DataChunk &groups, Vector &addresses, idx_t& matchedCount, bool createGroups, SelectionVector& matchedSel);
    // Find or creates groups and return the new groups index
    void findOrCreateGroups(Vector &hash, DataChunk &groups, Vector &addresses, idx_t& newGroupsCount, SelectionVector& newGroupSel);



    // Combine with other HT
    virtual void combine(PRLHashTable& other);
    // Partition the HT
    void partition(vector<distinct_ht_ptr_t>& partitions, idx_t shift);

    idx_t getSize() const;
    idx_t getCapacity()const;
    string toString(bool compact = true);
    vector<LogicalType> getTypes() const;

protected:

    // Resize the HT
    void resize(idx_t size, bool initResize = false);

    void findOrCreateGroupsInternal(Vector &hash, DataChunk &groups, Vector &addresses, idx_t& matchedCount, idx_t& newGroupsCount, bool createGroups = true, SelectionVector* matchedSel = nullptr, SelectionVector* newGroupSel = nullptr);

    template <class FUNC = std::function<void(idx_t, idx_t, data_ptr_t)>>
    void payloadApply(FUNC fun);

    void newBlock();

    Vector move(Vector &addresses, Vector &hashes, idx_t count);
    Vector move(Vector &addresses, Vector &hashes, idx_t count, SelectionVector* newGroupSel, idx_t& newGroupsCount);


    // Types of the columns in the hash table
    vector<LogicalType> types_;
    // Buffer manager of hash and data blocks
    BufferManager &bufferManager_;
    // Layout of the payload data in row layout
    RowLayout layout_;

    // The stringheap of the AggregateHashTable
    std::unique_ptr<RowDataCollection> stringHeap_;

    // The total tuple size
    idx_t tupleSize_;
    // The amount of tuples that fit in a single block
    idx_t tuplesPerBlock_;
    // The capacity of the HT. This can be increased using
    idx_t capacity_;
    // The amount of entries stored in the HT currently
    idx_t entries_;
    // If is resizable
    bool resizable_;
    // The data of the HT
    vector<buffer_handle_ptr_t> payload_;
    vector<data_ptr_t> payloadPtrs_;

    // The hashes of the HT
    buffer_handle_ptr_t hashes_;
    data_ptr_t hashesPtr_;
    data_ptr_t hashesEndPtr_; // end of of hashes

    // current offset of the tuple inside the payload (block is full when payloadPageOffset_ == tuplesPerBlock_ )
    idx_t payloadPageOffset_;

    // Bitmask for getting relevant bits from the hashes to determine the position
    hash_t bitmask_;
};

using distinct_ht_ptr_t = PRLHashTable::distinct_ht_ptr_t;

template <class FUNC>
void PRLHashTable::payloadApply(FUNC fun) {
    if (entries_ == 0) {
        return;
    }
    idx_t apply_entries = entries_;
    idx_t page_nr = 0;
    idx_t page_offset = 0;

    for (auto &payload_chunk_ptr : payloadPtrs_) {
        auto this_entries = minValue(tuplesPerBlock_, apply_entries);
        page_offset = 0;
        auto end = payload_chunk_ptr + this_entries * tupleSize_;
        for (data_ptr_t ptr = payload_chunk_ptr; ptr < end; ptr += tupleSize_)
            fun(page_nr, page_offset++, ptr);

        apply_entries -= this_entries;
        page_nr++;
    }
    BB_ASSERT(apply_entries == 0);
}
}
