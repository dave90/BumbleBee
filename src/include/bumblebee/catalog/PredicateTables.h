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
    void initializeChunks();
    // The amount of rows in the ChunkCollection
    idx_t getCount() const {
        return atoms_.getCount();
    }
    // The amount of columns
    idx_t columnCount() const {
        return predicate_->getArity();
    }
    // The amount of chunks
    idx_t chunkCount() const {
        return atoms_.chunkCount();
    }
    // Gets a reference to the chunk at the given index
    DataChunk & getChunk(idx_t index) {
        return atoms_.getChunk(index);
    }
    // Append a data chunk
    void append(DataChunk &chunk);
    // Return the Constat types for each column
    std::vector<ConstantType> getTypes();


    PredicateTables & operator=(const PredicateTables &other) = delete;
    PredicateTables & operator=(PredicateTables &&other) noexcept = delete;

    friend bool operator==(const PredicateTables &lhs, const PredicateTables &rhs);
    friend bool operator!=(const PredicateTables &lhs, const PredicateTables &rhs);

protected:
    // Update current types based on new types
    void updateTypes(std::vector<ConstantType> &newTypes);

    // Types of the columns
    std::vector<ConstantType> types_;

    // Data Chunk collections of atoms (columnar version)
    ChunkCollection atoms_;
    // Fact cached during the parsing not loaded
    std::vector<Atom> facts_;

    // cache output
    // hash tables etc

};

using predicate_table_ptr_t = std::unique_ptr<PredicateTables>;

}
