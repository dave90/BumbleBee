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
#include "bumblebee/common/Hash.h"
#include "bumblebee/common/Mutex.h"
#include "bumblebee/common/types/ChunkCollection.h"
#include "bumblebee/parser/statement/Predicate.h"
#include "bumblebee/parser/statement/Atom.h"

namespace bumblebee{


class PredicateTables {
public:
    // public pointer to the predicate
    predicate_ptr_t predicate_;

    PredicateTables(const char* name, unsigned arity);
    PredicateTables(const PredicateTables &other) = delete;
    PredicateTables(PredicateTables &&other) noexcept = delete;
    ~PredicateTables() = default;

    // move the atom in the fact vector
    void addFact(Atom& atom);
    // Move the fact into the chunk if any
    // Call before sourcing the chunks
    void initializeChunks();
    // The amount of rows in the ChunkCollection
    idx_t getCount() const {
        return chunks_.getCount();
    }
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
    const std::vector<Atom>& getFacts() const {
        return facts_;
    }
    // Get a value given a row and column index
    Value getValue(idx_t column, idx_t index);

    // Append a data chunk
    void append(data_chunk_ptr_t chunk);
    void append(DataChunk& chunk);

    // Return the Constat types for each column
    std::vector<ConstantType> getTypes();


    PredicateTables & operator=(const PredicateTables &other) = delete;
    PredicateTables & operator=(PredicateTables &&other) noexcept = delete;

    friend bool operator==(const PredicateTables &lhs, const PredicateTables &rhs);
    friend bool operator!=(const PredicateTables &lhs, const PredicateTables &rhs);

protected:
    // Update current types based on new types
    void updateTypes(std::vector<ConstantType> &newTypes);
    void loadFacts();
    void loadRanges();

    // Types of the columns
    std::vector<ConstantType> types_;

    // Data Chunk collections of atoms (columnar version)
    ChunkCollection chunks_;
    // Fact cached during the parsing (not loaded)
    // after the initializeChunks() will be cleared
    // TODO posible optimization store in a row chunks
    std::vector<Atom> facts_;
    // facts that contains ranges
    // after the initializeChunks() will be cleared
    std::vector<Atom> ranges_;

    // mutex for sync functions
    mutex mutex_;

    // cache output
    // hash tables etc

};

using predicate_table_ptr_t = std::unique_ptr<PredicateTables>;

}
