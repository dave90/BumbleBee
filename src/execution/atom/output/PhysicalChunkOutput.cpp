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
#include "bumblebee/execution/atom/output/PhysicalChunkOutput.hpp"
#include "bumblebee/common/Mutex.hpp"

#include "CLI11.hpp"

namespace bumblebee{

class ChunksOutputCollector {
public:
    ChunksOutputCollector() = default;
    virtual ~ChunksOutputCollector() = default;

    ChunkCollection chunks_;
    // Copy the chunk (call if is not full)
    virtual void sinkChunk(DataChunk &chunk) {
        chunks_.append(chunk);
    }

    // Append the pointer in the chunk collection (without copying)
    virtual void sinkChunk(data_chunk_ptr_t &chunk) {
        chunks_.append(std::move(chunk));
        // check if the previous chunk was not full
        idx_t size = chunks_.chunkCount();
        if (size > 1 && chunks_.getChunk(size - 2).getSize() != chunks_.getChunk(size - 2).getCapacity()) {
            // the second last is not full so swap with the last
            chunks_.swapChunks(size -1 , size - 2);
        }
    }
};

class GlobalChunkOutputState : public  GlobalPhysicalAtomState, public ChunksOutputCollector {
public:

    explicit GlobalChunkOutputState(PredicateTables *pt): pt_(pt) {}

    void initPredicateTable() {
        pt_->initializeChunks();
        isPtInitialized_ = true;
    }

    // Copy the chunk (call if is not full)
    void sinkChunk(DataChunk &chunk) override{
        lock_guard lock(chunksMutex_);
        if (!isPtInitialized_) {
            initPredicateTable();
        }
        ChunksOutputCollector::sinkChunk(chunk);
    }

    // Append the pointer in the chunk collection (without copying)
    void sinkChunk(data_chunk_ptr_t &chunk) override {
        lock_guard lock(chunksMutex_);
        if (!isPtInitialized_) {
            initPredicateTable();
        }
        ChunksOutputCollector::sinkChunk(chunk);
    }

private:
    // mutex for pushing data in predicate table
    mutex chunksMutex_;
    bool isPtInitialized_{false};
    // Predicate table
    PredicateTables* pt_;
};

class ChunkOutputState : public  PhysicalAtomState, public ChunksOutputCollector  {
public:
    ChunkOutputState() = default;
};


PhysicalChunkOutput::PhysicalChunkOutput(const vector<ConstantType> &types, vector<idx_t> &dcCols,
PredicateTables *pt): PhysicalAtom(types), pt_(pt){
    dcCols_ = std::move(dcCols);
    for (auto c : dcCols_)dcColsType_.push_back(types_[c]);
}

PhysicalChunkOutput::~PhysicalChunkOutput() {}


bool PhysicalChunkOutput::isSink() const {
    return true;
}

string PhysicalChunkOutput::getName() const {
    return "PHYSICAL_CHUNK_OUTPUT";
}

string PhysicalChunkOutput::toString() const {
    auto result = getName();
    result += " (" + pt_->predicate_.get()->toString()+"; ";
    for (auto c : dcCols_) {
        result += std::to_string(c) + ", ";
    }
    result += ";";
    for (auto c : dcColsType_) {
        result += ctypeToString(c) + ", ";
    }
    return result + ")";

}

pstate_ptr_t PhysicalChunkOutput::getState() const {
    return pstate_ptr_t(new ChunkOutputState());
}

gpstate_ptr_t PhysicalChunkOutput::getGlobalState() const {
    return gpstate_ptr_t(new GlobalChunkOutputState(pt_));
}


AtomResultType PhysicalChunkOutput::sink(ThreadContext& context, DataChunk &input, PhysicalAtomState &state, GlobalPhysicalAtomState &gstate) const {
    /**
     * Handles input data chunk processing and forwarding in a chunked execution pipeline.
     *
     * This function processes an incoming DataChunk (`input`) within the context of the current
     * operator state (`state`) and the global state (`gstate`). It performs the following:
     *
     * - If the input is empty and the cache is empty, signals that more input is needed.
     * - If the input is empty but the cache holds data, flushes the cached chunk to the global state.
     * - If the input chunk is full, it is immediately forwarded to the global state.
     * - If the input is partial, it is normalized and either cached or merged with the existing cache.
     * - If the cache becomes full during merging, it is flushed to the global state.
     * - If the input is too large to fully fit in the cache, the cache is filled and flushed,
     *   and the remaining input is cached.
     *
     * return AtomResultType indicating whether more input is needed or more output is available (cache is not empty).
     */

    context.profiler_.startPhysicalAtom(this);

    auto& cstate = (ChunkOutputState&)state;
    if (input.getSize() == 0) {
        context.profiler_.endPhysicalAtom(input);
        return AtomResultType::NEED_MORE_INPUT;
    }

    BB_ASSERT(input.getSize() > 0);
    DataChunk pinput = projectColumns(input);
    BB_ASSERT(pinput.columnCount() == dcColsType_.size());

    if ( pinput.getSize() == pinput.getCapacity()) {
        auto cloned = pinput.clone();
        cstate.sinkChunk(cloned);
    }else {
        cstate.sinkChunk(pinput);
    }

    context.profiler_.endPhysicalAtom(input);
    return AtomResultType::NEED_MORE_INPUT;
}

void PhysicalChunkOutput::combine(ThreadContext &context, PhysicalAtomState &state,
    GlobalPhysicalAtomState &gstate) const {
    context.profiler_.startPhysicalAtom(this);

    auto& cstate = (ChunkOutputState&)state;
    auto& gcstate = (GlobalChunkOutputState&)gstate;

    for (auto& c : cstate.chunks_.chunks()) {
        if (c->getCapacity() == c->getSize())
            gcstate.sinkChunk(c);
        else
            gcstate.sinkChunk(*c);
    }

    cstate.chunks_.reset();
    DataChunk chunk;
    context.profiler_.endPhysicalAtom(chunk);
}


void PhysicalChunkOutput::finalize(ThreadContext& context, GlobalPhysicalAtomState& gstate) const {
    context.profiler_.startPhysicalAtom(this);
    // send the chunks from the global state to predicate table
    auto& gcstate = (GlobalChunkOutputState&)gstate;
    for (auto& chunk: gcstate.chunks_.chunks()) {
        pt_->append(*chunk);
    }
    context.profiler_.endPhysicalAtomFinalize();

}
}
