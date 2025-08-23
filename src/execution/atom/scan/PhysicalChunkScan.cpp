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
#include "bumblebee/execution/atom/scan/PhysicalChunkScan.h"

#include "bumblebee/catalog/Schema.h"
#include "bumblebee/common/Mutex.h"

namespace bumblebee{
class GlobalChunkScanState : public  GlobalPhysicalAtomState {
public:
    GlobalChunkScanState(PredicateTables* pt): pt_(pt) {
    }

    void initPredicateTable() {
        pt_->initializeChunks();
        isPtInitialized_ = true;
        for (idx_t i = 0; i < pt_->chunkCount(); ++i) {
            chunksSize_.push_back(pt_->getChunk(i).getSize());
        }
    }

    // return the start and end index of the next chunks to read
    // return false if no chunks are available to read , otherwise true
    bool getNextChunksToRead(idx_t& start,idx_t& end) {
        // sync the function
        lock_guard lock(mutex_);
        if (!isPtInitialized_) {
            initPredicateTable();
        }
        if (chunksRead_ >= chunksSize_.size()) {
            return false;
        }

        auto size = chunksSize_[0];
        start = chunksRead_;
        end = start;
        while (end < chunksSize_.size() - 1) {
            size += chunksSize_[end];
            if (size > MORSEL_SIZE) break;
            ++end;
        }
        chunksRead_ = end + 1;
        return true;
    }

private:
    // Predicate table
    PredicateTables* pt_;
    // mutex to handle multiple threads calls
    mutex mutex_;
    // number of chunks read
    idx_t chunksRead_{0};
    // size of rows for each chunk
    vector<idx_t> chunksSize_;
    // true if init function was called in pt
    bool isPtInitialized_{false};

};

class ChunkScanState : public  PhysicalAtomState {
public:
    ChunkScanState() = default;

    // current chunk index to read
    idx_t currentIdx_{0};
    // final chunk index to read
    idx_t endIdx_{0};
    // if the state is initialized
    bool isInitialized_{false};
};

PhysicalChunkScan::PhysicalChunkScan(const vector<ConstantType> &types, vector<idx_t>& dcCols,vector<idx_t> &selectedCols, idx_t estimated_cardinality, PredicateTables *pt) :
    PhysicalAtom(types,dcCols, selectedCols, estimated_cardinality) {
    pt_ = pt;
    // expected dcCols increment array
    for (idx_t i = 0; i < dcCols.size(); ++i) {
        BB_ASSERT(i == dcCols[i]);
    }
}


PhysicalChunkScan::~PhysicalChunkScan() = default;

bool PhysicalChunkScan::isSource() const {
    return true;
}

string PhysicalChunkScan::getName() const {
    return "PHYSICAL_CHUNK_SCAN";
}

string PhysicalChunkScan::toString() const {
    auto result = getName();
    result += " (" + pt_->predicate_.get()->toString()+"; ";
    for (auto c : dcCols_) {
        result += std::to_string(c) + ", ";
    }
    result += "; ";
    for (auto c : selectCols_) {
        result += std::to_string(c) + ", ";
    }
    for (auto c : colsType_) {
        result += ctypeToString(c) + ", ";
    }
    return result + ")";
}

pstate_ptr_t PhysicalChunkScan::getState() const {
    return pstate_ptr_t(new ChunkScanState());
}

gpstate_ptr_t PhysicalChunkScan::getGlobalState() const {
    return gpstate_ptr_t(new GlobalChunkScanState(pt_));
}

idx_t PhysicalChunkScan::getMaxThreads() const {
    return estimatedCardinality_ / MORSEL_SIZE + 1; // for cardinality not multiple of morsel size
}

AtomResultType PhysicalChunkScan::getData(ThreadContext& context, DataChunk &chunk, PhysicalAtomState &state, GlobalPhysicalAtomState &gstate) const {
    context.profiler_.startPhysicalAtom(this);
    auto& cstate = (ChunkScanState&)state;
    auto& cgstate = (GlobalChunkScanState&)gstate;
    if (!cstate.isInitialized_) {
        // state is not initialized, so fetch the chunks from global state
        idx_t start, end;
        if (!cgstate.getNextChunksToRead(start, end)) {
            // no more data to read, return empty Chunk
            chunk.setCardinality(0);
            context.profiler_.endPhysicalAtom(chunk);
            return AtomResultType::FINISHED;
        }
        cstate.currentIdx_ = start;
        cstate.endIdx_ = end;
        cstate.isInitialized_ = true;
    }
    if (cstate.currentIdx_ > cstate.endIdx_) {
        // no more data to read return empty chunk
        chunk.setCardinality(0);
        context.profiler_.endPhysicalAtom(chunk);
        return AtomResultType::FINISHED;
    }
    auto& ptChunk = pt_->getChunk(cstate.currentIdx_);
    ++cstate.currentIdx_;
    // project the columns
    chunk.reference(ptChunk, selectCols_);
    context.profiler_.endPhysicalAtom(chunk);
    return AtomResultType::HAVE_MORE_OUTPUT;
}

}
