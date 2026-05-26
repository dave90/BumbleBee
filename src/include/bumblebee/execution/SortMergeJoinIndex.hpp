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
#include "bumblebee/common/types/DataChunk.hpp"
#include "bumblebee/storage/BufferManager.hpp"

namespace bumblebee {
struct Piece {
    Piece(const vector<LogicalType> &types);

    DataChunk        payload_;       // FULL build columns (all pt cols), columnar, <= STANDARD_VECTOR_SIZE rows
    Vector           keyBlobs_;      // STRING, one ASC byte-comparable key per row (kept alive)
    vector<sel_t>    sortedRowIdx_;  // row indices into payload_, sorted ASC by memcmp(keyBlobs_)
    string_t         *minKey_, *maxKey_;
};

class SortMergeJoinIndex {

public:
    SortMergeJoinIndex(BufferManager &mgr, const vector<LogicalType> &types, idx_t key_col,
                       const vector<idx_t> &payloads);

    // true if this index was built for the given key column and payload set (cache lookup, mirrors RL HT)
    bool checkKeyAndPayloads(idx_t keyCol, const vector<idx_t> &payloads) const {
        return keyCol_ == keyCol && payloads_ == payloads;
    }
    idx_t keyCol() const { return keyCol_; }

    // build (thread-local): append full build chunk; seal open_ when it reaches STANDARD_VECTOR_SIZE
    void addChunk(DataChunk& full);
    void sealOpen();                 // createSortKey on open_.payload_[keyCol_] -> keyBlobs_; sort sortedRowIdx_; set min/max
    // combine: splice another (thread-local) index's pieces into this one (move, no payload copy)
    void mergeFrom(SortMergeJoinIndex& other);
    void finalize();                 // sealOpen() if non-empty; ready_=true
    bool isReady() const { return ready_; }
    const vector<Piece>& pieces() const { return pieces_; }
    // probe support: matched [lo,hi) within piece.sortedRowIdx_ for `probe OP build`
    static void rangeFor(const Piece& p, const string_t& probeKey, Binop op, idx_t& lo, idx_t& hi);

private:
    // reset open_ to a fresh, reusable empty state (payload data_ cleared + count_=0, keyBlobs_ allocated)
    void resetOpen();

    BufferManager& mgr_;
    vector<LogicalType> types_;      // full pt column types
    idx_t keyCol_;                   // key column index in the full pt chunk
    vector<idx_t> payloads_;         // payload columns this index was created for (cache key)
    vector<Piece> pieces_;
    Piece open_;                     // current unsealed piece being filled
    std::atomic<bool> ready_{false};
};

using sort_merge_index_ptr_t = std::unique_ptr<SortMergeJoinIndex>;

}
