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
#include "bumblebee/common/TypeDefs.hpp"
#include "bumblebee/common/types/DataChunk.hpp"
#include "bumblebee/parallel/ThreadContext.hpp"

namespace bumblebee{

// The operator can return one of three possible results:
// - NEED_MORE_INPUT: The operator has finished processing the current input
//   and is ready to accept more if available.
// - HAVE_MORE_OUTPUT: The operator is not yet done with the current input
//   and will be called again with the same input.
// - FINISHED: The operator has completed the entire pipeline.
//   No further processing is required, and no other operators in the pipeline will be invoked.
enum class AtomResultType : uint8_t {
	NEED_MORE_INPUT = 0,
	HAVE_MORE_OUTPUT = 1,
	FINISHED = 2
};


// Represents the state of the physical operator.
// Initialized and updated by the atom itself during execution,
// effectively storing the atom's memory.
class PhysicalAtomState {
public:
    PhysicalAtomState() = default;
    ~PhysicalAtomState() = default;
};

// Represents the Global state of the physical operator.
// Initialized and updated by the atom itself during execution,
// effectively store the data shared among ALL the thread operatos.
// NOTE: in case you need the partitions numbers create the ExecutorContext and store there
class GlobalPhysicalAtomState {
public:
	explicit GlobalPhysicalAtomState() = default;
	~GlobalPhysicalAtomState() = default;

	// return the start and end index of the next bucket to read
	// return false if no buckets are available to read , otherwise true
	static bool getNextBucket(idx_t& start,idx_t& end, idx_t& current, vector<idx_t>& size );

};

using pstate_ptr_t =std::unique_ptr<PhysicalAtomState>;
using pstate_ptr_vector_t = vector<pstate_ptr_t>;
using gpstate_ptr_t =std::unique_ptr<GlobalPhysicalAtomState>;

// PhysicalAtom (or patom) is the base class of the classical physical operators in mostly DB present in the
// execution plan
class PhysicalAtom {
public:

	// Information of the columns used by this patom. Why it is not in struct? because
	// not all the patoms needs all these information
	// types of all the columns of the data chunks
	vector<ConstantType> types_;
	// cols selected in the predicate tables
	vector<idx_t> selectCols_;
	// for each selected cols the cols index in the data chunk
	vector<idx_t> dcCols_;
	// because type_ contains the type of all the columns colsType_ contains the type of the interested columns
	vector<ConstantType> dcColsType_;

	PhysicalAtom(const vector<ConstantType> &types,vector<idx_t>& dcCols,vector<idx_t>& selectedCols);
	PhysicalAtom(const vector<ConstantType> &types);
	virtual ~PhysicalAtom() = default;

	// Execute is called during the execution and accept input chunks and produce output chunks.
	// Execute modify the internal state PhysicalAtomState but should NOT access to shared global states
	virtual AtomResultType execute(ThreadContext& context, DataChunk &input, DataChunk &chunk, PhysicalAtomState &state) const;
	// GetData source data and produce new chunks.
	// CAN be called in parallel, proper locking is needed when accessing global state
	virtual AtomResultType getData(ThreadContext& context, DataChunk &chunk, PhysicalAtomState &state, GlobalPhysicalAtomState& gstate) const;
	// Sink store data in a data structure or print the output.
	// CAN be called in parallel, proper locking is needed when accessing global state
	virtual AtomResultType sink(ThreadContext& context, DataChunk &input, PhysicalAtomState &state, GlobalPhysicalAtomState& gstate) const;
	// The finalize is called when ALL threads are finished execution. It is called only once per pipeline, and is
	// entirely single threaded.
	virtual void finalize(ThreadContext& context, GlobalPhysicalAtomState& gstate) const;

	// Return and set the maximum number of threads that can run this physical atom
	virtual idx_t getMaxThreads() const;
	virtual void setMaxThreads(idx_t threads) const;

	virtual bool isSource() const;
	virtual bool isSink() const;
	virtual string getName() const;
	virtual string toString() const;
	// Return a vector of the types that will be returned by this patom
	const vector<ConstantType> &getTypes()const;
	// Return the state of patom
	virtual pstate_ptr_t getState() const;
	virtual gpstate_ptr_t getGlobalState() const;

	friend bool operator==(const PhysicalAtom &lhs, const PhysicalAtom &rhs);
	friend bool operator!=(const PhysicalAtom &lhs, const PhysicalAtom &rhs);

protected:
	// Select columns from selectCols_
	DataChunk selectColumns(DataChunk &chunk) const;
	// Select columns from dcCols
	DataChunk projectColumns(DataChunk &input) const;

};

using patom_ptr_t = std::unique_ptr<PhysicalAtom>;
using patom_ptr_vector_t = vector<patom_ptr_t>;
}
