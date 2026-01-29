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
#include "bumblebee/common/Mutex.hpp"
#include "bumblebee/common/types/ChunkCollection.hpp"
#include "bumblebee/parser/statement/Predicate.hpp"
#include "bumblebee/parser/statement/Atom.hpp"
#include "bumblebee/function/AggregateFunction.hpp"
#include "bumblebee/execution/JoinHashTable.hpp"
#include "bumblebee/execution/JoinPRLHashTable.hpp"
#include "bumblebee/execution/PartitionedAggHT.hpp"
#include "bumblebee/execution/PartitionedPRLHashTable.hpp"
#include "bumblebee/execution/RowLayoutJoinHashTable.hpp"

namespace bumblebee{


class PredicateTables {

public:
    // public pointer to the predicate
    predicate_ptr_t predicate_;

    PredicateTables(ClientContext* context_, const char* name, unsigned arity);
    PredicateTables(const PredicateTables &other) = delete;
    PredicateTables(PredicateTables &&other) noexcept = delete;
    ~PredicateTables() = default;

    // move the atom in the fact vector
    void addFact(Atom& atom);
    // Move the fact into the chunk if any
    // Call before sourcing the chunks
    void initializeChunks();
    // The amount of rows in the ChunkCollection or facts or range
    idx_t getCount() const;

    // The amount of columns
    idx_t columnCount() const {
        return predicate_->getArity();
    }
    // The amount of chunks
    idx_t chunkCount() const {
        return chunks_.chunkCount();
    }
    // Gets a reference to the chunk at the given index
    DataChunk & getChunk(idx_t index) {
        return chunks_.getChunk(index);
    }
    // Gets to the chunk at the given index from distinct HT
    DataChunk getChunkFromDistinct(idx_t index, idx_t size = STANDARD_VECTOR_SIZE);

    const vector<Atom>& getFacts() const {
        return facts_;
    }

    vector<idx_t> getKeys() const{
        vector<idx_t> keys;
        for (idx_t i = 0; i < predicate_->getArity(); ++i)
            keys.push_back(i);
        return keys;
    }

    void setTypes(const vector<LogicalType>& types) {
        types_ = types;
    }

    bool isDistinct() const {
        return predicate_->isDistinct();
    }

    // Get a value given a row and column index
    Value getValue(idx_t column, idx_t index);
    // Update the types of the pt and in case cast all the tables
    bool updateTypes(const vector<LogicalType>& types);

    // Append a data chunk
    void append(DataChunk& chunk) {
        if (!isDistinct())
            appendInChunkCollection(chunk);
        else
            appendInDistinctHT(chunk);
    }

    // Return the Constat types for each column
    vector<LogicalType> getTypes();

    // Return a join hash table with the same keys. If does not exist create it
    joinht_ptr_t& getJoinHashTable(const vector<idx_t>& keys, const vector<idx_t>& payloads);
    bool existJoinHashTable(const vector<idx_t>& keys, const vector<idx_t>& payloads);

    // Return a join Row Layout hash table with the same keys and payloads.
    rl_join_ht_ptr_t& getJoinRLHashTable(const vector<idx_t>& keys,const vector<idx_t>& payload );
    void createJoinRLHashTable(const vector<LogicalType>& types, const vector<idx_t>& keys,const vector<idx_t>& payload );
    bool existJoinRLHashTable(const vector<idx_t>& keys,const vector<idx_t>& payload) const;

    bool existPartitionedPRLHashTable() const{
        return partitionedPRLHT_ != nullptr;
    }
    partitioned_prl_ht_ptr_t&  getPartitionedPRLHashTable();

    // Return a partitioned aggregate join hash table with the same groups, payload and functions. If does not exist create it
    partitioned_agg_ht_ptr_t& getPartitionedAggHashTable() {
        return partitionedAggHT_;
    }
    partitioned_agg_ht_ptr_t& createPartitionedAggHashTable( const vector<idx_t>& groups,const vector<idx_t>& payloads, const vector<AggregateFunction*>& aggregateFunctions, idx_t estimatedSourceCardinality = 0 );
    bool existPartitionedAggHashTable();

    PredicateTables & operator=(const PredicateTables &other) = delete;
    PredicateTables & operator=(PredicateTables &&other) noexcept = delete;

    friend bool operator==(const PredicateTables &lhs, const PredicateTables &rhs);
    friend bool operator!=(const PredicateTables &lhs, const PredicateTables &rhs);

    void setRecursive(bool recursive);
    bool isRecursive() const;
    void initializeDelta();
    void mergeDelta();
    idx_t getDeltaCount() const;
    void resetDeltaCount();
    void clearDelta() {
        delta_.deltaCount_ = 0;
        delta_.nextDelta_ = nullptr;
        delta_.delta_ = nullptr;
    }

protected:
    void loadFacts();
    void loadRanges();
    void appendInChunkCollection(DataChunk& chunk);
    void appendInDistinctHT(DataChunk& chunk);

    void castEntirePredicateTable(const vector<LogicalType> &newTypes);


    // Types of the columns
    vector<LogicalType> types_;

    // Data Chunk collections of atoms (columnar version)
    ChunkCollection chunks_;
    // Fact cached during the parsing (not loaded)
    // after the initializeChunks() will be cleared
    // TODO posible optimization store in a row chunks
    vector<Atom> facts_;
    // facts that contains ranges
    // after the initializeChunks() will be cleared
    vector<Atom> ranges_;

    // mutex for sync functions
    // TODO test with light semaphore
    mutex mutex_;

    // hash tables data structures
    vector<joinht_ptr_t> jhtables_;
    // Partitioned PRL hash table (distinct table)
    partitioned_prl_ht_ptr_t partitionedPRLHT_;
    // Row Layout hash tables (for joins)
    vector<rl_join_ht_ptr_t> rlHTables_;
    // partitioned aggregate hash table (for aggregates)
    partitioned_agg_ht_ptr_t partitionedAggHT_;

    ClientContext* context_;

    struct RecursiveDelta {
        partitioned_prl_ht_ptr_t delta_;
        partitioned_prl_ht_ptr_t nextDelta_;
        atomic<idx_t> deltaCount_{0};
    };

    RecursiveDelta delta_;
    bool recursive_;
};

using predicate_table_ptr_t = std::unique_ptr<PredicateTables>;

}
