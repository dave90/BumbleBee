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
#include "bumblebee/common/TypeDefs.h"
#include "bumblebee/common/Vector.h"
#include "bumblebee/common/types/DataChunk.h"

namespace bumblebee{


class ChunkOneHashTable {
public:

    using distinct_ht_ptr_t = std::unique_ptr<ChunkOneHashTable>;

    // The hash table load factor, when a resize is triggered
    constexpr static float LOAD_FACTOR = 0.7;

    ChunkOneHashTable(const vector<ConstantType>& types, idx_t capacity, bool resizable);
    virtual ~ChunkOneHashTable() {};

    ChunkOneHashTable(const ChunkOneHashTable &other) = delete;
    ChunkOneHashTable(ChunkOneHashTable &&other) noexcept = delete;
    ChunkOneHashTable & operator=(const ChunkOneHashTable &other) = delete;
    ChunkOneHashTable & operator=(ChunkOneHashTable &&other) noexcept = delete;


    // Add a given data to HT
    void addChunk(Vector& hash, DataChunk& chunk);

    // Scan the HT starting from the position until the result and group
    idx_t scan(idx_t position, DataChunk& result, idx_t size = STANDARD_VECTOR_SIZE);

    // Find or creates groups
    void findOrCreateGroups(Vector &hash, DataChunk &groups, SelectionVector &groupSel);
    // Find or creates groups and return the matched index ( elements not created)
    void findOrCreateGroups(Vector &hash, DataChunk &groups, SelectionVector &groupSel, idx_t& matchedCount, bool createGroups, SelectionVector& matchedSel);
    // Find or creates groups and return the new groups index
    void findOrCreateGroups(Vector &hash, DataChunk &groups, SelectionVector &groupSel, idx_t& newGroupsCount, SelectionVector& newGroupSel);

    // Combine with other HT
    void combine(ChunkOneHashTable& other);
    // Partition the HT
    void partition(vector<distinct_ht_ptr_t>& partitions, idx_t shift);

    idx_t getSize() const;
    idx_t getCapacity()const;
    string toString(bool compact = true);
    vector<ConstantType> getTypes() const;

protected:

    // Resize the HT
    virtual void resize(idx_t size);
    // init the states
    void copyNewGroups(DataChunk &groups,SelectionVector& emptyBucketSel, SelectionVector& emptySel, idx_t new_entry_count);
    // try to match the groups
    void matchChunks(DataChunk &groups, SelectionVector& compareSel, SelectionVector& compareBucketSel,
                     SelectionVector& notMatchSel, idx_t &need_compare_count, idx_t &no_match_count);
    // find the groups (bucket) not empty starting from a position with specific size
    // return the size of the groups in the sel vector
    idx_t getGroups(idx_t position, SelectionVector &sel, idx_t size);
    // Finds or creates groups in the hashtable using the specified group keys. The groupSel selection vector will point to the groups.
    // Is possible to use it to find only the match group calling with createGroups to false, matched sel will return the matched index
    void findOrCreateGroupsInternal(Vector &hash, DataChunk &groups, SelectionVector &groupSel, idx_t& matchedCount, idx_t& newGroupsCount, bool createGroups = true, SelectionVector* matchedSel = nullptr, SelectionVector* newGroupSel = nullptr);


    // Max entries in HT
    idx_t capacity_;
    // Elements in the HT
    idx_t entries_;
    // Holds the groups of HT
    DataChunk chunkone_;
    // Hash of data, last bit store if the bucket is empty or not
    Vector hash_;
    // Mask to apply on the hash to set bucket not empty
    idx_t mask_;
    // if the hash table can resize
    bool resizable_;
};


}
