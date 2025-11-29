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
#include "PRLHashTable.hpp"

namespace bumblebee{

/*
 * Class AggregatePRLHashTable
 * Aggregate hash table built on top of PRLHashTable that maintains aggregate states per group
 *
 * Overview
 * - Extends PRLHashTable to append aggregate state columns after the group columns inside the row layout
 *
 * Data layout and storage
 * - Each row consists of the group key columns followed by aggregate state slots determined by AggregateFunction metadata
 * - Variable length data is stored in the inherited string heap and is preserved when moving or combining tables
 * - Layout offsets and tuple size are recomputed in the constructor to include aggregate states
 *
 */
class AggregatePRLHashTable : public PRLHashTable{
public:
    using agg_ht_ptr_t = std::unique_ptr<AggregatePRLHashTable>;

    AggregatePRLHashTable(BufferManager &manager, const vector<ConstantType> &types, idx_t capacity, bool resizable,
        const vector<AggregateFunction *> &functions);

    ~AggregatePRLHashTable() override;


    void addChunk(Vector& hash, DataChunk& groups, DataChunk& payload);
    // add the paylaod to the first state (in case of no groups)
    void addChunk(DataChunk& payload) override;

    void combine(AggregatePRLHashTable& other);


    // Fetch the aggregates for specific groups from the HT and place them in the result
    // filter out the groups that does not matched
    void fetchAggregates(Vector& hash, DataChunk& group, DataChunk& result, SelectionVector &sel);
    // fetch function results specified in the function value
    void fetchAggregates(Vector& hash, DataChunk& group, Vector& result, idx_t function ,SelectionVector &sel);
    // get the results of the first state (in case of no groups)
    void fetchAggregates(DataChunk& result);
    void fetchAggregates(Vector& result, idx_t function);

private:
    void findAddresses(Vector &hash, DataChunk &groups, SelectionVector &sel, Vector &addresses, idx_t &matchedGroups);
    void moveAndMergeStates(idx_t count, Vector &addresses, Vector &hashes);

    // Aggregates functions
    vector<AggregateFunction*> functions_;
};


}
