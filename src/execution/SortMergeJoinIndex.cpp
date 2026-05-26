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
#include "bumblebee/execution/SortMergeJoinIndex.hpp"

#include "bumblebee/common/Helper.hpp"
#include "bumblebee/common/vector_operations/CreateSortKey.hpp"
#include "bumblebee/common/vector_operations/VectorOperations.hpp"

namespace bumblebee {
Piece::Piece(const vector<LogicalType> &types):keyBlobs_(LogicalTypeId::STRING), minKey_(nullptr), maxKey_(nullptr) {
    payload_.initializeEmpty(types);
    sortedRowIdx_.reserve(STANDARD_VECTOR_SIZE);
}

SortMergeJoinIndex::SortMergeJoinIndex(BufferManager &mgr, const vector<LogicalType> &types, idx_t key_col,
                                       const vector<idx_t> &payloads): mgr_(mgr),
                                                                       types_(types),
                                                                       keyCol_(key_col),
                                                                       payloads_(payloads),
                                                                       open_(types)
{
    resetOpen();
}

void SortMergeJoinIndex::addChunk(DataChunk &full) {
    BB_ASSERT(full.getSize() <= STANDARD_VECTOR_SIZE);
    BB_ASSERT(full.columnCount() > keyCol_);
    BB_ASSERT(full.getTypes() == types_);

    // Fast path: a full-size chunk with nothing pending in open_ becomes its own piece via
    // zero-copy reference. Only valid when open_ is empty — otherwise referencing would discard
    // open_'s pending rows, so we fall through to the accumulate path below.
    if (full.getSize() == STANDARD_VECTOR_SIZE && open_.payload_.getSize() == 0) {
        open_.payload_.initAndReference(full); // open_ payload is empty here; init then zero-copy reference
        sealOpen();
        return;
    }
    if (open_.payload_.getSize() == 0) {
        open_.payload_.initialize(types_); // init data as we need to copy the vectors
    }
    // input data chunk size < STANDARD_VECTOR_SIZE and is not empty so we need to merge with open
    idx_t freeSpace = STANDARD_VECTOR_SIZE - open_.payload_.getSize();
    idx_t realSize = full.getSize();
    full.setCardinality(minValue<idx_t>(freeSpace, realSize));
    open_.payload_.append(full, false);
    if (open_.payload_.getSize() == STANDARD_VECTOR_SIZE) {
        sealOpen();
    }
    if (realSize > full.getSize()) {
        idx_t remaining = realSize - full.getSize();
        // we need to append the remaining, means that
        BB_ASSERT(open_.payload_.getSize() == 0); // expecting size 0 after the call of sealOpen
        open_.payload_.initialize(types_);
        // copy() takes (source END index, source offset, target offset) and copies [offset, end);
        // the leftover rows are source[full.getSize() .. realSize) -> pass realSize as the end, not `remaining`.
        for (idx_t i=0; i < full.columnCount();++i ) {
            VectorOperations::copy(full.data_[i], open_.payload_.data_[i], realSize, full.getSize(), 0);
        }
        open_.payload_.setCardinality(remaining);
    }
    full.setCardinality(realSize);

}

void SortMergeJoinIndex::sealOpen() {
    if (open_.payload_.getSize() == 0)
        return;
    // Build the byte-comparable keys ONCE from the finalized payload key column — one key-construction
    // path for both the full and accumulate cases (avoids fragile incremental key copying).
    CreateSortKey::createSortKey(open_.payload_.data_[keyCol_], open_.payload_.getSize(),
                                 {OrderType::ASCENDING}, open_.keyBlobs_);
    for (idx_t i =0;i<open_.payload_.getSize();++i)
        open_.sortedRowIdx_.push_back(i);
    BB_ASSERT(open_.keyBlobs_.getVectorType() == VectorType::FLAT_VECTOR);
    auto keys = FlatVector::getData<string_t>(open_.keyBlobs_);
    std::sort(open_.sortedRowIdx_.begin(), open_.sortedRowIdx_.end(),
          [&keys](idx_t a, idx_t b) {
              return keys[a] < keys[b];
            });

    pieces_.push_back(std::move(open_));
    // Set min/max AFTER the piece is in its final home: the pointers must reference the piece's
    // OWN keyBlobs_ buffer. Computing them from open_ before the move is unsafe because resetOpen()
    // reallocates open_.keyBlobs_, which can free/replace the buffer the pointers referenced.
    Piece& sealed = pieces_.back();
    auto sealedKeys = FlatVector::getData<string_t>(sealed.keyBlobs_);
    sealed.minKey_ = sealedKeys + sealed.sortedRowIdx_.front();
    sealed.maxKey_ = sealedKeys + sealed.sortedRowIdx_.back();

    resetOpen();
}

void SortMergeJoinIndex::resetOpen() {
    // open_ may be moved-from (data_ empty, but count_ stale) or freshly ctor'd (initializeEmpty);
    // destroy() clears data_ AND resets count_/capacity_, so the later initialize() in addChunk is valid.
    open_.payload_.destroy();
    open_.keyBlobs_.initialize(false, STANDARD_VECTOR_SIZE);
    open_.sortedRowIdx_.clear();
    open_.sortedRowIdx_.reserve(STANDARD_VECTOR_SIZE);
    open_.minKey_ = nullptr;
    open_.maxKey_ = nullptr;
}

void SortMergeJoinIndex::mergeFrom(SortMergeJoinIndex &other) {
    BB_ASSERT(other.ready_);
    if (other.pieces_.size() == 0)
        return;
    // move the n-1 full blocks
    for (idx_t i =0;i<other.pieces_.size()-1;++i) {
        BB_ASSERT(other.pieces_[i].payload_.getSize() == STANDARD_VECTOR_SIZE);
        pieces_.push_back(std::move(other.pieces_[i]));
    }
    idx_t last = other.pieces_.size() - 1;
    if (other.pieces_[last].payload_.getSize() == STANDARD_VECTOR_SIZE) {
        pieces_.push_back(std::move(other.pieces_[last]));
        return;
    }
    // last is not full
    addChunk(other.pieces_[last].payload_);
}

void SortMergeJoinIndex::finalize() {
    sealOpen();
    ready_ = true;
}

void SortMergeJoinIndex::rangeFor(const Piece &p, const string_t &probeKey, Binop op, idx_t &lo, idx_t &hi) {
    const idx_t n = p.sortedRowIdx_.size();
    const bool gt = (op == Binop::GREATER || op == Binop::GREATER_OR_EQ);
    if (probeKey > *p.maxKey_) {
        // every build key < probeKey: GREATER/GREATER_OR_EQ match the whole piece, LESS/LESS_OR_EQ match nothing
        lo = 0;
        hi = gt ? n : 0;
        return;
    }
    if (probeKey < *p.minKey_) {
        // every build key > probeKey: LESS/LESS_OR_EQ match the whole piece, GREATER/GREATER_OR_EQ match nothing
        lo = 0;
        hi = gt ? 0 : n;
        return;
    }
    auto keyValues = FlatVector::getData<string_t>(p.keyBlobs_);
    auto& keyIndex = p.sortedRowIdx_;

    // Each operator needs only ONE boundary, so do a single binary search:
    //   GREATER (build < probe) -> [0, lb)   LESS_OR_EQ (build >= probe) -> [lb, n)   need lb = lower_bound (# keys < probe)
    //   GREATER_OR_EQ (build <= probe) -> [0, ub)   LESS (build > probe) -> [ub, n)   need ub = upper_bound (# keys <= probe)
    const bool useLower = (op == Binop::GREATER || op == Binop::LESS_OR_EQ);
    const idx_t pos = useLower
        ? std::lower_bound(keyIndex.begin(), keyIndex.end(), probeKey,
              [&keyValues](sel_t idx, const string_t& value) { return keyValues[idx] < value; }) - keyIndex.begin()
        : std::upper_bound(keyIndex.begin(), keyIndex.end(), probeKey,
              [&keyValues](const string_t& value, sel_t idx) { return value < keyValues[idx]; }) - keyIndex.begin();

    if (gt) {
        lo = 0;   hi = pos;
    } // GREATER / GREATER_OR_EQ: prefix [0, pos)
    else {
        lo = pos; hi = n;
    } // LESS / LESS_OR_EQ:       suffix [pos, n)
}
}
