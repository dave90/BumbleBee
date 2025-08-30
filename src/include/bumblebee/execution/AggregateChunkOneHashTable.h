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
#include "ChunkOneHashTable.h"
#include "bumblebee/function/AggregateFunction.h"

namespace bumblebee{

using distinct_ht_ptr_t = ChunkOneHashTable::distinct_ht_ptr_t;

class AggregateChunkOneHashTable : public ChunkOneHashTable {
public:
    using agg_ht_ptr_t = std::unique_ptr<AggregateChunkOneHashTable>;

    AggregateChunkOneHashTable(const vector<ConstantType> &types, idx_t capacity, bool resizable,
        const vector<AggregateFunction *> &functions);
    virtual ~AggregateChunkOneHashTable() = default;

    void addChunk(Vector& hash, DataChunk& groups, DataChunk& payload);
    // add the paylaod to the first state (in case of no groups)
    void addChunk(DataChunk& payload);

    void combine(AggregateChunkOneHashTable& other);

    // Fetch the aggregates for specific groups from the HT and place them in the result
    // filter out the groups that does not matched
    void fetchAggregates(Vector& hash, DataChunk& group, DataChunk& result, SelectionVector &sel);
    // fetch function results specified in the function value
    void fetchAggregates(Vector& hash, DataChunk& group, Vector& result, idx_t function ,SelectionVector &sel);
    // get the results of the first state (in case of no groups)
    void fetchAggregates(DataChunk& result);
    void fetchAggregates(Vector& result, idx_t function);


    void finalize();
    bool isReady();
    string toString(bool compact);

private:
    void resize(idx_t size) override;

    // Aggregates functions
    vector<AggregateFunction*> functions_;
    // Aggregated data for each function
    DataChunk payload_;
    // Holds the states for each aggregation
    vector<agg_states_ptr> states_;
    // If is ready to be used
    bool ready_;
};


}
