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
#include "bumblebee/execution/atom/scan/PhysicalChunkScan.hpp"

#include "bumblebee/catalog/Schema.hpp"
#include "bumblebee/common/Mutex.hpp"

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
        count_ = pt_->getCount();
    }

    // return the start and end index of the next chunks to read
    // return false if no chunks are available to read , otherwise true
    bool getNextChunksToRead(idx_t& start,idx_t& end) {
        // sync the function
        lock_guard lock(mutex_);
        if (!isPtInitialized_) {
            initPredicateTable();
        }
        if (!pt_->isDistinct())
            return getNextBucket(start, end, chunksRead_, chunksSize_);

        return getNextChunksToReadHT(start, end);
    }

    bool getNextChunksToReadHT(idx_t& start,idx_t& end) {
        if (currentOffset_ >= count_)
            return false;
        start = currentOffset_;
        end = minValue( count_, start + MORSEL_SIZE) - 1;
        currentOffset_ += MORSEL_SIZE;
        return true;
    }

    idx_t estimateMaxThreads() {
        idx_t size = pt_->getCount();
        return size / MORSEL_SIZE + 1;
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
    idx_t currentOffset_{0};
    // true if init function was called in pt
    bool isPtInitialized_{false};
    // size of pred table
    idx_t count_{0};
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

PhysicalChunkScan::PhysicalChunkScan(const vector<ConstantType> &types, vector<idx_t>& dcCols,vector<idx_t> &selectedCols, PredicateTables *pt) :
    PhysicalAtom(types,dcCols, selectedCols) {
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
    result += "; ";
    for (auto c : dcColsType_) {
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
    auto state = GlobalChunkScanState(pt_);
    return state.estimateMaxThreads();
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

    if (!pt_->isDistinct()) {
        auto& ptChunk = pt_->getChunk(cstate.currentIdx_++);
        chunk.reference(ptChunk, selectCols_);
    }else {
        idx_t size = minValue<idx_t>(STANDARD_VECTOR_SIZE, cstate.endIdx_ - cstate.currentIdx_ + 1);
        DataChunk result = pt_->getChunkFromDistinct(cstate.currentIdx_, size);
        cstate.currentIdx_ += result.getSize();
        chunk.reference(result, selectCols_);
    }

    BB_ASSERT(chunk.getSize());

    context.profiler_.endPhysicalAtom(chunk);
    return AtomResultType::HAVE_MORE_OUTPUT;
}

}
