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

#include <unordered_set>

#include "Value.hpp"
#include "Vector.hpp"
#include "bumblebee/common/TypeDefs.hpp"

namespace bumblebee{
	struct VectorData;
	class SelectionVector;
	class Vector;

    // A Data Chunk represents a collection of equal-length vectors.
/*
    Used by execution engine, a DataChunk is an intermediate form
    representing a subset of a relation.

    Initialized via DataChunk::Initialize with a list of CType, it allocates
    memory for the vectors by default. These vectors reference the chunk’s own
    data but can also point to data from other chunks—for example, when an
    operator like Filter doesn't modify the data but adds a selection vector.

    The DataChunk also owns the selection vector that vectors may reference.
*/
class DataChunk {
public:
    // The vectors owned by the DataChunk (is public I know :) )
    vector_vector_t data_;

    // Creates an empty DataChunk
    DataChunk();
    DataChunk(DataChunk&& other) noexcept ;
	DataChunk(const DataChunk &) = delete;
	~DataChunk() = default;

    inline idx_t getSize() const {
		return count_;
	}
	inline idx_t columnCount() const {
		return data_.size();
	}
	inline void setCardinality(idx_t count) {
		BB_ASSERT(count <= capacity_);
		count_ = count;
	}
	inline void setCardinality(const DataChunk &other) {
		count_ = other.getSize();
	}
	inline void setCapacity(const DataChunk &other) {
		capacity_ = other.capacity_;
	}
	inline void setCapacity(idx_t capacity) {
    	capacity_ = capacity;
    }
	inline idx_t getCapacity() const {
    	return capacity_;
    }

	Value getValue(idx_t col, idx_t index) const;
	void setValue(idx_t col, idx_t index, const Value &val);

	// Set the DataChunk to reference another data chunk
	void reference(const DataChunk &chunk);
	// Set the DataChunk to reference another data chunk only on columns defined in cols
	void reference(DataChunk &chunk, const vector<idx_t>& cols);
	// call first the initialize and then reference the columns
	void initAndReference(DataChunk &chunk, const vector<idx_t>& cols);
	void initAndReference(DataChunk &chunk);

	// Create a new pointer data chunk that reference to this data chunk
	std::unique_ptr<DataChunk> clone();

	// Initializes the DataChunk with the specified types to an empty DataChunk
	// This will create one vector of the specified type for each type in the
	// types list. The vector will be referencing vector to the data owned by
	// the DataChunk.
	void initialize(const vector<PhysicalType> &types);
	void initialize(const vector<LogicalType> &types);
	void initialize(const vector<LogicalType> &types, const std::unordered_set<idx_t> & colsToInitialize);

	// Initializes an empty DataChunk with the given types. The vectors will *not* have any data allocated for them.
	void initializeEmpty(const vector<PhysicalType> &types);
	void initializeEmpty(const vector<LogicalType> &types);
	// Append the other DataChunk to this one. The column count and types of
	// the two DataChunks have to match exactly. Throws an exception if there
	// is not enough space in the chunk and resize is not allowed.
	// TODO move sel vector to shared ptr
	void append(const DataChunk &other, bool resize = false, SelectionVector *sel = nullptr,
	                       idx_t count = 0);
	// Resize all the vectors
	void resize(idx_t size);

	// Destroy all data and columns owned by this DataChunk
	void destroy();

	// Copies the data from this vector to another vector.
	void copy(DataChunk &other, idx_t offset = 0) const;
	void copy(DataChunk &other, const SelectionVector &sel, idx_t source_count, idx_t offset = 0) const;

	// Splits the DataChunk in two moving cols in other
	void split(DataChunk &other, idx_t splitIndex);

	// Turn all the vectors from the chunk into flat vectors
	void normalify();
	array_vector_data_t orrify();
	// slice the vectors
	void slice(const SelectionVector &sel_vector, idx_t count);
	// slice the vectors of other using a selection and set the results in data_
	void slice(DataChunk &other, const SelectionVector &sel, idx_t count, idx_t colOffset = 0);
	// slice the vectors of other using a selection and set the results in data_ setting the cols based on colsMap
	void slice(DataChunk &other, const SelectionVector &sel, idx_t count, const vector<idx_t>& colsMap);

	// Resets the DataChunk to its state right after the DataChunk::Initialize
	// function was called. This sets the count to 0, and resets each member
	// Vector to point back to the data owned by this DataChunk.
	void reset();
	void reset(const vector<idx_t>& columnsToReset);


	// Hashes the DataChunk to the target vector
	void hash(Vector &result);
	void hash(Vector &result, const vector<idx_t> &cols);


	// Returns a list of types of the vectors of this data chunk
	vector<LogicalType> getTypes() const;

	// Converts this DataChunk to a printable string representation
	std::string toString() const;

	// Verify that the DataChunk is in a consistent, not corrupt state. DEBUG
	// FUNCTION ONLY!
	void verify();

	// cast the data chunk based on the input types
	// this method COPY data (use it if necessary)
	void cast(const vector<LogicalType>& types);
	void cast(DataChunk& result);

private:
	// The amount of tuples stored in the data chunk
	idx_t count_;
	// The amount of tuples that can be stored in the data chunk
	idx_t capacity_;
};

using data_chunk_ptr_t = std::unique_ptr<DataChunk>;
using data_chunk_vector_t = vector<data_chunk_ptr_t>;

}
