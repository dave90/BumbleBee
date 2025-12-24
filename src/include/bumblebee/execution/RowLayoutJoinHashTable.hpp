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
#include "bumblebee/parser/statement/Predicate.hpp"

namespace bumblebee{

/*
 * Hash table for join operations with support for incremental updates.
 *
 * Compared to `JoinHashTable`, this implementation is more flexible: it can be
 * updated in place without rebuilding the entire hash table. This makes it a good
 * fit for recursive predicates where each iteration produces new data.
 *
 * Trade-off: `JoinHashTable` is generally faster for static inputs.
 * Choose `RowLayoutJoinHashTable` when incremental maintenance is required.
 */
class RowLayoutJoinHashTable: public PRLHashTable {

public:
    RowLayoutJoinHashTable(BufferManager &manager, const vector<ConstantType> &types,
        const vector<idx_t> &key_columns, const vector<idx_t> &payload_columns, idx_t capacity = HT_INIT_CAPACITY, bool resizable = true);

    // Compare the types of the ht with the types of predicate
    bool checksTypes(const vector<ConstantType>& predicateTypes);

    // Add a given data to HT
    void addChunk(DataChunk& payload) override;

    // Probe the left chunk (classical join)
    void probe(idx_t &ltuple, idx_t &rtuple, DataChunk &lchunk, Vector& lhash,SelectionVector &lsel, SelectionVector &rsel, DataChunk &result);

    void finalize();

    // Compare the keys and payload with the current JoinPRLHashTable
    bool checkKeysAndPayloads(const vector<idx_t>& keys,const vector<idx_t>& payloads);


    bool isReady();

    vector<idx_t> getKeys();
    vector<idx_t> getPayloads();

protected:
    void newBlocks(idx_t size, Vector& addresses);

private:
    // Keys of the HT
    vector<idx_t> keyColumns_;
    // Payload of the HT
    vector<idx_t> payloadColumns_;
    // Keys and payload columns
    vector<idx_t> cols_;
    // keys row layout
    RowLayout keyLayout_;
    // global mutex for the ht
    mutex mutex_;
    // Index of the data sync in hash table
    idx_t idxEntries_{0};

    atomic<bool> ready_{false};

public:
    // Combine h1 with h2 using the common types. Results are in h1
    static void castAndCombine(BufferManager &manager, std::unique_ptr<RowLayoutJoinHashTable> &h1, RowLayoutJoinHashTable &h2);
    static void cast(BufferManager &manager, std::unique_ptr<RowLayoutJoinHashTable> &h1, vector<ConstantType> predicateTypes);

};

using rl_join_ht_ptr_t = std::unique_ptr<RowLayoutJoinHashTable>;

}
