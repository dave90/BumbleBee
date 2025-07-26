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
#include "bumblebee/execution/atom/output/PhysicalChunkOutput.h"
#include "bumblebee/common/Mutex.h"

#include "CLI11.hpp"

namespace bumblebee{

class GlobalChunkOutputState : public  GlobalPhysicalAtomState {
public:
    // Chunks to push in pt during finalize
    ChunkCollection chunks_;

    explicit GlobalChunkOutputState(PredicateTables *pt) {}

    // Copy the chunk (call if is not full)
    void sinkChunk(DataChunk &chunk) {
        lock_guard lock(chunksMutex_);
        chunks_.append(chunk);
    }

    // Append the pointer in the chunk collection (without copying)
    void sinkChunk(data_chunk_ptr_t &chunk) {
        lock_guard lock(chunksMutex_);
        chunks_.append(std::move(chunk));
    }

private:
    // mutex for pushing data in predicate table
    mutex chunksMutex_;
};

class ChunkOutputState : public  PhysicalAtomState {
public:
    ChunkOutputState() = default;

    // cache chunk if the output chunk of an iteration is not full
    DataChunk cachedChunk_;
};


PhysicalChunkOutput::PhysicalChunkOutput(const std::vector<ConstantType> &types, idx_t estimated_cardinality,
PredicateTables *pt, std::vector<idx_t> &cols): PhysicalAtom(types, cols, estimated_cardinality),
                                       pt_(pt){
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
    for (auto c : cols_) {
        result += std::to_string(c) + ", ";
    }
    for (auto c : colsType_) {
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

DataChunk PhysicalChunkOutput::projectColumns(DataChunk &input) const{

    DataChunk newChunk;
    newChunk.initializeEmpty(colsType_);
    newChunk.reference(input, cols_);
    return newChunk;

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
    auto& gcstate = (GlobalChunkOutputState&)gstate;
    if (input.getSize() == 0 && cstate.cachedChunk_.getSize() == 0) {
        context.profiler_.endPhysicalAtom(input);
        return AtomResultType::NEED_MORE_INPUT;
    }

    if ( input.getSize() == 0 && cstate.cachedChunk_.getSize() > 0) {
        // input is empty and cache not, send the cache to global state
        gcstate.sinkChunk(cstate.cachedChunk_);
        cstate.cachedChunk_.destroy();
        context.profiler_.endPhysicalAtom(input);
        return AtomResultType::NEED_MORE_INPUT;
    }
    BB_ASSERT(input.getSize() > 0);
    DataChunk pinput = projectColumns(input);
    BB_ASSERT(pinput.columnCount() == colsType_.size());

    if ( pinput.getSize() == pinput.getCapacity()) {
        // chunk is full push in the global state
        data_chunk_ptr_t cptr = pinput.clone();
        gcstate.sinkChunk(cptr);
        auto resultState = AtomResultType::HAVE_MORE_OUTPUT;
        if (cstate.cachedChunk_.getSize() == 0)
            resultState =  AtomResultType::NEED_MORE_INPUT;
        context.profiler_.endPhysicalAtom(input);
        return resultState;
    }

    // pinput chunk is not full so we need to flat it
    pinput.normalify();
    // if cache is empty set to cache
    if (cstate.cachedChunk_.getSize() == 0) {
        cstate.cachedChunk_.initializeEmpty(colsType_);
        cstate.cachedChunk_.reference(pinput);
        context.profiler_.endPhysicalAtom(input);
        return AtomResultType::HAVE_MORE_OUTPUT;
    }

    auto spaceAvailable = cstate.cachedChunk_.getCapacity() - cstate.cachedChunk_.getSize();
    // check if pinput fit into the cache
    if (spaceAvailable >= pinput.getSize()) {
        cstate.cachedChunk_.append(pinput);
        // if cache is full send to global state and reset the cache
        if (cstate.cachedChunk_.getSize() == cstate.cachedChunk_.getCapacity()) {
            data_chunk_ptr_t cptr = cstate.cachedChunk_.clone();
            gcstate.sinkChunk(cptr);
            cstate.cachedChunk_.destroy();
        }
        context.profiler_.endPhysicalAtom(input);
        return AtomResultType::NEED_MORE_INPUT;
    }

    // pinput does not fit into the cache, so append a portion of pinput
    // send the cache to global state and set the remaining portion of pinput into the cache
    auto oldSize = pinput.getSize();
    pinput.setCapacity(spaceAvailable);
    cstate.cachedChunk_.append(pinput);
    data_chunk_ptr_t cptr = cstate.cachedChunk_.clone();
    gcstate.sinkChunk(cptr);
    cstate.cachedChunk_.reset();

    input.setCardinality(oldSize);
    auto inputOffset = spaceAvailable;
    cstate.cachedChunk_.copy(input, inputOffset);
    context.profiler_.endPhysicalAtom(input);
    return AtomResultType::HAVE_MORE_OUTPUT;
}


void PhysicalChunkOutput::finalize(ThreadContext& context, GlobalPhysicalAtomState& gstate) const {
    context.profiler_.startPhysicalAtom(this);
    // send the chunks from the global state to predicate table
    auto& gcstate = (GlobalChunkOutputState&)gstate;
    for (auto& chunk: gcstate.chunks_.chunks()) {

        if (chunk->getSize() == chunk->getCapacity()) {
            data_chunk_ptr_t cptr = chunk->clone();
            pt_->append(std::move(cptr));
            continue;
        }
        pt_->append(*chunk);
    }
    context.profiler_.endPhysicalAtomFinalize();

}
}
