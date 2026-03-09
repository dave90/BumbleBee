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

#include "Expression.hpp"
#include "bumblebee/common/Mutex.hpp"
#include "bumblebee/common/types/ChunkCollection.hpp"
#include "bumblebee/common/types/DataChunk.hpp"
#include "bumblebee/parser/statement/Predicate.hpp"

namespace bumblebee{

using directory_t = std::unique_ptr<uint64_t[]>;

/**
 * JoinHashTable
 * Build-side hash table for hash joins with a compact directory layout and per-bucket Bloom filters.
 * Inspired by: https://db.in.tum.de/~birler/papers/hashtable.pdf
 *
 * JoinHashTable materializes the “right” side of a join into a single contiguous
 * DataChunk (chunkone_) and provides fast, bucketed probing for the “left” side.
 * It is designed for parallel construction (per-bucket stats and locking) and
 * vectorized probing, while keeping the directory metadata small and cache-friendly.
 *
 * High-level workflow
 * 1) Stats collection (addDataChunkSel):
 *    - For each incoming build chunk and its precomputed row hashes, the table
 *      computes a bucket id as `hash & (buckets_ - 1)` (buckets_ is a power of two).
 *    - Indices are grouped by bucket and recorded as `DataChunkSel` entries.
 *    - Per-bucket sizes are atomically accumulated, guarded by per-bucket mutexes.
 *
 * 2) Directory allocation :
 *    - Allocates directory of length buckets and computes a prefix sum of
 *      per-bucket sizes.
 *
 * 3) Bucket build :
 *    - Copies the selected rows for a single bucket from their source chunks into
 *      the bucket’s assigned slice of chunkone.
 *    - Computes a N-bit Bloom filter over that bucket’s hashes and packs it into
 *      the directory entry (see “Directory encoding”).
 *
 * 4) Probing :
 *    - For each left row (and its hash), compute the bucket id as above.
 *    - Bloom check: quickly reject buckets whose Bloom filter cannot contain
 *      the hash.
 *    - Candidate scan: linearly traverse the bucket’s slice of `chunkone_`,
 *      producing matches into selection vectors.
 *    - Refinement: If multiple join predicates are supplied, the first
 *      predicate seeds the candidate list; subsequent predicates refine it in place.
 *
 * Directory encoding
 * Each entry `directory_[i]` is a 64-bit word that packs:
 *   - Low (64 - BLOOM_SIZE) bits: the exclusive end offset of bucket `i`
 *     within `chunkone_` (i.e., `dirEnd(i)`). The begin offset is derived from
 *     the previous entry (`dirBegin(i)`).
 *   - High BLOOM_SIZE bits: a per-bucket Bloom filter (16 bits by default).
 *
 *
 *  Bloom filter
 * - A lightweight, N-bit Bloom is accumulated per bucket at build time from the
 *   bucket’s row hashes. The filter is stored in the high bits of the directory
 *   word, so no extra memory is needed.
 * - At probe time, `bloomCouldContains(bloom, hash)` guards the (small) linear
 *   scan of the bucket’s slice in `chunkone_`.
 *
 * Data layout
 * - chunkone: a single contiguous DataChunk holding all build rows, ordered
 *   by bucket. Each bucket occupies `[dirBegin(i), dirEnd(i))`.
 * - directory_: compact metadata array (length = buckets_), one word per bucket
 *   with packed end offset + Bloom bits.
 *
 */
class JoinHashTable {
public:
    static constexpr idx_t BLOOM_SIZE = 16; // supported size 0 (disabled) or 16
    static constexpr int BLOOM_SHIFT = 64 - BLOOM_SIZE;
    static constexpr uint64_t BLOOM_MASK64 = (~0ULL << BLOOM_SHIFT); // only high BLOOM_SIZE bits are 1


    struct DataChunkSel {
        DataChunkSel(DataChunk *chunk_, SelectionVector &sel, idx_t size);

        DataChunk* chunk_;
        SelectionVector sel_;
        idx_t size_;
    };
public:
    JoinHashTable(Predicate *predicate, const vector<idx_t> &keys, const vector<idx_t> &payloads, idx_t buckets);


    void addDataChunkSel(Vector& hash, DataChunk& chunk);
    // init directory and chunkone. Call after all the chunks was added with addDataChunkSel
    void initDirectory();
    // build hash table for specific bucket
    void build(idx_t start, idx_t end);
    // clear stats
    void clearStats();
    // get the data of the hash table
    DataChunk& getDataChunk();


    // Get and set ready
    void setReady();
    bool isReady();
    // check if the keys are equal to the ht keys
    bool checkKeys(const vector<idx_t>& keys,const vector<idx_t>& payloads );
    // Probe the left chunk
    idx_t probe(idx_t &ltuple, idx_t &rtuple, DataChunk &lchunk, Vector& lhash,
                         SelectionVector &lsel, SelectionVector &rsel, const vector<Expression> &conditions);

    // Return hash table as string
    string toString();
    // Return directory of hash table
    directory_t& getDirectory();
    // Get number of buckets
    idx_t getBuckets();

private:
    // increment size of a bucket
    void incrementBucketSize(idx_t bucket, idx_t size){
        BB_ASSERT(bucket < stats_.bucketSize_.size());
        stats_.bucketSize_[bucket].fetch_add(size);
    }

    // merge the string aux data mngr to chunkone
    void mergeStringDataMngr(vector<vector_data_mngr_ptr_t>& stringDataMngrs);
    void build(idx_t bucket, vector<vector_data_mngr_ptr_t>&);

    // Calculate the bucket vector from the hash
    Vector calculateBucketVector(Vector& hash, idx_t size);

    // This struct store the statistics for the building of the hash table
    struct JoinHashTableStats {
        JoinHashTableStats(idx_t buckets);

        // Number of elements for each bucket
        vector<std::atomic<idx_t>> bucketSize_;
        // Chunks with selection vector for each bucket
        vector<vector<DataChunkSel>> bucketChunks_;
        // locks for each bucket
        vector<std::mutex> bucketMutex_;
        ChunkCollection chunkCollection_;
        std::mutex mutex_;
    };
    // Number of buckets
    idx_t buckets_;
    // predicate of ht
    Predicate* predicate_;
    // Keys and payloads of the HT
    vector<idx_t> keys_;
    vector<idx_t> payloads_;
    vector<bool> usedCols_;
    // Store the element indices for each bucket.
    // For bucket i, the data range is [directory[i-1], directory[i])
    // (start inclusive, end exclusive).
    directory_t directory_;
    // hash table data
    DataChunk chunkone_;
    // Stats of HT
    JoinHashTableStats stats_;
    // If is ready to use
    bool ready_{false};
    // mutex for string vectors
    vector<mutex> mutex_;

public:

    JoinHashTableStats& getStats() {
        return stats_;
    }

    // static functions
    // Return a begin range for a directory
    inline static uint64_t dirBegin(const uint64_t* d, idx_t b){ return (b ? (d[b-1] & ~BLOOM_MASK64) : 0); }
    // Return a end range for a directory
    inline static uint64_t dirEnd(const uint64_t* d, idx_t b){ return d[b] & ~BLOOM_MASK64; }
    // Return the bloom filter of a directory
    inline static uint16_t dirBloom(const uint64_t* d, idx_t b){ return uint16_t(d[b] >> BLOOM_SHIFT); }
};

using joinht_ptr_t = std::unique_ptr<JoinHashTable>;

}
