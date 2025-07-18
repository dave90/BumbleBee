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
#include "bumblebee/common/TypeDefs.h"
#include "bumblebee/common/types/DataChunk.h"

namespace bumblebee{

// The operator can return one of three possible results:
// - NEED_MORE_INPUT: The operator has finished processing the current input
//   and is ready to accept more if available.
// - HAVE_MORE_OUTPUT: The operator is not yet done with the current input
//   and will be called again with the same input.
// - FINISHED: The operator has completed the entire pipeline.
//   No further processing is required, and no other operators in the pipeline will be invoked.
enum class AtomResultType : uint8_t { NEED_MORE_INPUT, HAVE_MORE_OUTPUT, FINISHED };


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
class GlobalPhysicalAtomState {
public:
	GlobalPhysicalAtomState() = default;
	~GlobalPhysicalAtomState() = default;

	idx_t partitions;
};

using pstate_ptr_t =std::unique_ptr<PhysicalAtomState>;
using pstate_ptr_vector_t = std::vector<pstate_ptr_t>;
using gpstate_ptr_t =std::unique_ptr<GlobalPhysicalAtomState>;

// PhysicalAtom (or patom) is the base class of the classical physical operators in mostly DB present in the
// execution plan
class PhysicalAtom {
public:

	std::vector<ConstantType> types_;
	// TODO populate this value
	idx_t estimatedCardinality_;

	PhysicalAtom(const std::vector<ConstantType> &types, idx_t estimated_cardinality);
	virtual ~PhysicalAtom() = default;

	// Execute is called during the execution and accept input chunks and produce output chunks.
	// Execute modify the internal state PhysicalAtomState but should NOT access to shared global states
	virtual AtomResultType execute(DataChunk &input, DataChunk &chunk, PhysicalAtomState &state) const;
	// GetData source data and produce new chunks.
	// CAN be called in parallel, proper locking is needed when accessing global state
	virtual void getData(DataChunk &chunk, PhysicalAtomState &state, GlobalPhysicalAtomState& gstate) const;
	// Sink store data in a data structure or print the output.
	// CAN be called in parallel, proper locking is needed when accessing global state
	virtual AtomResultType sink(DataChunk &input, PhysicalAtomState &state, GlobalPhysicalAtomState& gstate) const;
	// The finalize and initialize is called when ALL threads are finished execution. It is called only once per pipeline, and is
	// entirely single threaded.
	virtual void finalize() const;

	virtual bool isSource() const;
	virtual bool isSink() const;
	virtual string getName() const;
	virtual string toString() const;
	// Return a vector of the types that will be returned by this patom
	const std::vector<ConstantType> &getTypes()const;
	// Return the state of patom
	virtual pstate_ptr_t getState() const;
	virtual gpstate_ptr_t getGlobalState() const;

	friend bool operator==(const PhysicalAtom &lhs, const PhysicalAtom &rhs);
	friend bool operator!=(const PhysicalAtom &lhs, const PhysicalAtom &rhs);
};

using patom_ptr_t = std::unique_ptr<PhysicalAtom>;
using patom_ptr_vector_t = std::vector<patom_ptr_t>;
}
