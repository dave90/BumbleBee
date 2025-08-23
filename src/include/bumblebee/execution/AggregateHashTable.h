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
#include "bumblebee/common/types/DataChunk.h"
#include "bumblebee/function/AggregateFunction.h"

namespace bumblebee{


class AggregateHashTable {

public:
    using agg_ht_ptr = std::unique_ptr<AggregateHashTable>;

    // The hash table load factor, when a resize is triggered
    constexpr static float LOAD_FACTOR = 0.7;

    AggregateHashTable(const vector<AggregateFunction*>& aggFunctions, const vector<ConstantType>& types, idx_t capacity = MORSEL_SIZE, bool resizable = false);

    AggregateHashTable(const AggregateHashTable &other) = delete;
    AggregateHashTable(AggregateHashTable &&other) noexcept = delete;
    AggregateHashTable & operator=(const AggregateHashTable &other) = delete;
    AggregateHashTable & operator=(AggregateHashTable &&other) noexcept = delete;

    // Add a given data to HT and computing the aggregates
    void addChunk(Vector& hash, DataChunk& group, DataChunk& payload);
    // find the groups (bucket) not empty starting from a position with specific size
    // return the size of the groups in the sel vector
    idx_t getGroups(idx_t position, SelectionVector &sel, idx_t size);

    // Scan the HT starting from the position until the result and group
    idx_t scan(idx_t position, DataChunk& result);
    // Fetch the aggregates for specific groups from the HT and place them in the result
    // filter out the groups that does not matched
    void fetchAggregates(Vector& hash, DataChunk& group, DataChunk& result);
    // finalize
    void finalize();

    // Finds or creates groups in the hashtable using the specified group keys. The groupSel selection vector will point to the
    // groups. The return value is the amount of matched groups.
    // Is possible to use it to find only the match group calling with createGroups to false, matched sel will return the matched index
    idx_t findOrCreateGroups(Vector &hash, DataChunk &groups, SelectionVector &groupSel,bool createGroups = true, SelectionVector* matched = nullptr);
    // Combine with other HT
    void combine(AggregateHashTable& other);
    // Partition the HT
    void partition(vector<agg_ht_ptr>& partitions, idx_t shift);

    idx_t getSize() const;
    idx_t getCapacity()const;
    bool isReady() const;
    void setReady();
    string toString(bool compact = true);

private:
    // Resize the HT
    void resize(idx_t size);

    void initStates(DataChunk &groups,SelectionVector& emptyBucketSel, SelectionVector& emptySel, idx_t new_entry_count);

    void matchChunks(DataChunk &groups, SelectionVector& compareSel, SelectionVector& compareBucketSel,
                     SelectionVector& notMatchSel, idx_t &need_compare_count, idx_t &no_match_count);

    // Aggregates functions
    vector<AggregateFunction*> functions_;
    // Max entries in HT
    idx_t capacity_;
    // Elements in the HT
    idx_t entries_;
    // Holds the groups of HT
    DataChunk chunkone_;
    // Aggregated data for each function
    DataChunk payload_;
    // Holds the states for each aggregation
    vector<agg_states_ptr> states_;
    // Hash of group data, last bit store if the bucket is empty or not
    Vector hash_;
    // Mask to apply on the hash to set bucket not empty
    idx_t mask_;
    // If is ready to be used
    bool ready_;
    // if the hash table can resize
    bool resizable_;

};


}
