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

#include "DataChunk.hpp"
#include "bumblebee/common/TypeDefs.hpp"

namespace bumblebee{

class ChunkCollection {
public:
    ChunkCollection() : count_(0) {
	}

	// Returns a list of types of the vectors of this data chunk
	vector<ConstantType> getTypes() {
	    return types_;
    }

	// The amount of rows in the ChunkCollection
	const idx_t &getCount() const {
		return count_;
	}

	// The amount of columns in the ChunkCollection
	idx_t columnCount() const {
		return types_.size();
	}

	// Append a new DataChunk to this ChunkCollection handling type checks and partial fills (NOTE: this append copy the chunk)
	void append(DataChunk &chunk);
	// Append a new DataChunk directly to this ChunkCollection (NOTE: this append do not copy the chunk)
	void append(data_chunk_ptr_t chunk);

	// Append another ChunkCollection directly to this ChunkCollection
	void append(ChunkCollection &other);

	// Merge is like Append but messes up the order and destroys the other collection
	void merge(ChunkCollection &other);

	// Fuse adds new columns to the right of the collection
	void fuse(ChunkCollection &other);

	// Swap the chunks
	void swapChunks(idx_t index1, idx_t index2);

	void verify();

	// Gets the value of the column at the specified index
	Value getValue(idx_t column, idx_t index);
	// Sets the value of the column at the specified index
	void setValue(idx_t column, idx_t index, const Value &value);

	// Copy a single cell to a target vector
	void copyCell(idx_t column, idx_t index, Vector &target, idx_t targetOffset);

	string toString() const;

    // Gets a reference to the chunk at the given index
	DataChunk & getChunkForRow(idx_t rowIndex) {
		return *chunks_[locateChunk(rowIndex)];
	}

	// Gets a reference to the chunk at the given index
	DataChunk & getChunk(idx_t chunk_index) {
		BB_ASSERT(chunk_index < chunks_.size());
		return *chunks_[chunk_index];
	}
	const DataChunk &getChunk(idx_t chunk_index) const {
		BB_ASSERT(chunk_index < chunks_.size());
		return *chunks_[chunk_index];
	}

	vector<data_chunk_ptr_t> &chunks() {
		return chunks_;
	}

	idx_t chunkCount() const {
		return chunks_.size();
	}

	void reset() {
		count_ = 0;
		chunks_.clear();
		types_.clear();
	}

	 data_chunk_ptr_t fetch() {
		if (chunkCount() == 0) {
			return nullptr;
		}
		auto res = std::move(chunks_[0]);
		count_ -= res->getSize();
		chunks_.erase(chunks_.begin() + 0);
		return res;
	}

	// Returns true if the ChunkCollections are equivalent
	// Not vectorized!!! Do not use during execution
	bool equals(ChunkCollection &other);

	// Locates the chunk that belongs to the specific index
	idx_t locateChunk(idx_t index) {
		idx_t result = index / STANDARD_VECTOR_SIZE;
		BB_ASSERT(result < chunks_.size());
		return result;
	}

	// cast data chunks
	void cast(const vector<ConstantType> &newTypes) {
		types_ = newTypes;
		if (count_ == 0 )return;
		for (auto& chunk : chunks_)
			chunk->cast(newTypes);
	}

private:
	// The total amount of elements in the collection
	idx_t count_;
	// The set of data chunks in the collection
	vector<data_chunk_ptr_t> chunks_;
	// The types of the ChunkCollection
	vector<ConstantType> types_;
};


}
