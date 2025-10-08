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
 * Choose `JoinPRLHashTable` when incremental maintenance is required.
 */
class JoinPRLHashTable: public PRLHashTable {

public:
    JoinPRLHashTable(BufferManager &manager, const vector<ConstantType> &types,
        const vector<idx_t> &key_columns, const vector<idx_t> &payload_columns, idx_t capacity = HT_INIT_CAPACITY, bool resizable = true);


    // Add a given data to HT
    void addChunk(DataChunk& payload) override;

    // Probe the left chunk (classical join)
    void probe(idx_t &ltuple, idx_t &rtuple, DataChunk &lchunk, Vector& lhash,SelectionVector &lsel, SelectionVector &rsel, DataChunk &result);


    // Execute the match operation (if row exist or not)
    void match(DataChunk &lchunk, Vector& lhash,SelectionVector &matchSel, idx_t& matchCount, SelectionVector &nonMatchSel, idx_t& noMatchCount);

    // Compare the keys and payload with the current JoinPRLHashTable
    bool checkKeysAndPayloads(const vector<idx_t>& keys,const vector<idx_t>& payloads);

    bool isReady();
    void setReady();

    vector<idx_t> getKeys();
    vector<idx_t> getPayloads();

private:
    void incrementBuckets(Vector& buckets, SelectionVector& notEqual, idx_t notEqualCount);
    void findAddresses(const SelectionVector* sel, idx_t size, Vector &buckets, Vector &lhash, SelectionVector &nonMatchSel, idx_t& noMatchCount,SelectionVector& toMatch , idx_t& toMatchCount, Vector& addresses);


    // Keys of the HT
    vector<idx_t> keyColumns_;
    // Payload of the HT
    vector<idx_t> payloadColumns_;
    // Keys and payload columns
    vector<idx_t> cols_;
    // keys row layout
    RowLayout keyLayout_;

    bool ready_{false};

public:
    // scan the data chunks of h1  and cast and add into h2
    static void insert(JoinPRLHashTable &h1, JoinPRLHashTable &h2);
    // Combine h1 with h2 using the common types. Results are in h1
    static void castAndCombine(BufferManager &manager, std::unique_ptr<JoinPRLHashTable> &h1, JoinPRLHashTable &h2);

};

using join_prl_ht_ptr_t = std::unique_ptr<JoinPRLHashTable>;

}
