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

#include "Expression.h"
#include "bumblebee/common/types/DataChunk.h"
#include "bumblebee/parser/statement/Predicate.h"

namespace bumblebee{

using directory_t = std::unique_ptr<uint64_t[]>;


class JoinHashTable {
public:
    static constexpr idx_t BLOOM_SIZE = 16;
    static constexpr int BLOOM_SHIFT = 64 - BLOOM_SIZE;
    static constexpr uint64_t BLOOM_MASK64 = (~0ULL << BLOOM_SHIFT); // only high BLOOM_SIZE bits are 1

public:
    JoinHashTable(Predicate *predicate, const std::vector<idx_t> &keys, idx_t buckets);

    // add the chunk in the stats
    void addDataChunkSel(Vector& hash, DataChunk& chunk);
    // init directory and chunkone. Call after all the chunks was added with addDataChunkSel
    void initDirectory();
    // build hash table for specific bucket
    void build(idx_t bucket);
    // clear stats
    void clearStats();
    // get the data of the hash table
    DataChunk& getDataChunk();

    void setReady();
    bool isReady();
    // check if the keys are equal to the ht keys
    bool checkKeys(std::vector<idx_t> keys);

    idx_t probe(idx_t &ltuple, idx_t &rtuple, DataChunk &lchunk, Vector& lhash,
                         SelectionVector &lsel, SelectionVector &rsel, const std::vector<Expression> &conditions);

    string toString();
    directory_t& getDirectory();

    idx_t getBuckets();


private:
    // increment size of a bucket
    void incrementBucketSize(idx_t bucket, idx_t size){
        BB_ASSERT(bucket < stats_.bucketSize_.size());
        stats_.bucketSize_[bucket].fetch_add(size);
    }

    inline idx_t getBucket(hash_t hash) {
        return hash & (buckets_ -1);
    }


    Vector calculateBucketVector(Vector& hash, idx_t size);

    // This struct store the statistics for the building of the hash table
    struct JoinHashTableStats {
        JoinHashTableStats(idx_t buckets);;

        struct DataChunkSel {
            DataChunkSel(DataChunk &chunk_, Vector& hash_, SelectionVector &sel, idx_t size);

            DataChunk chunk_;
            SelectionVector sel_;
            Vector hash_;
            idx_t size_;
        };
        // Number of elements for each bucket
        std::vector<std::atomic<idx_t>> bucketSize_;
        // Chunks with selection vector for each bucket
        std::vector<std::vector<DataChunkSel>> bucketChunks_;
        // locks for each bucket
        std::vector<std::mutex> bucketMutex_;
    };

    idx_t buckets_;
    Predicate* predicate_;
    std::vector<idx_t> keys_;
    // store the index of elements in each bucket (for bucket i data are stored from directory[i-1] to directory[i],directory[i] excluded )
    directory_t directory_;
    // hash table data
    DataChunk chunkone_;
    JoinHashTableStats stats_;
    bool ready_{false};

public:

    JoinHashTableStats& getStats() {
        return stats_;
    }

    // static functions
    inline static uint64_t dirBegin(const uint64_t* d, idx_t b){ return (b ? (d[b-1] & ~BLOOM_MASK64) : 0); }
    inline static uint64_t dirEnd(const uint64_t* d, idx_t b){ return d[b] & ~BLOOM_MASK64; }
    inline static uint16_t dirBloom(const uint64_t* d, idx_t b){ return uint16_t(d[b] >> BLOOM_SHIFT); }


};

}
