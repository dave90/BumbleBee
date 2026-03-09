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
#include "bumblebee/execution/atom/output/PhysicalTopNHOutput.hpp"

#include "CLI11.hpp"
#include "bumblebee/common/Mutex.hpp"
#include "bumblebee/execution/TopNHeap.hpp"


namespace bumblebee{

class GlobalChunkTopNHOutputState : public  GlobalPhysicalAtomState {
public:
    TopNHeap globalHeap_;

    GlobalChunkTopNHOutputState(PredicateTables* pt, const vector<LogicalType> &payloadTypes,const vector<ColModifier> &modifiers, idx_t limit):
        globalHeap_(payloadTypes, modifiers, limit), pt_(pt) {}

    void initPredicateTable() {
        pt_->initializeChunks();
        isPtInitialized_ = true;
    }

    // Combine heap with the global
    void combineHeap(TopNHeap& heap) {
        lock_guard lock(mutex_);
        if (!isPtInitialized_) {
            initPredicateTable();
        }
        globalHeap_.combine(heap);
    }

private:
    mutex mutex_;

    bool isPtInitialized_{false};
    // Predicate table
    PredicateTables* pt_;
};

class ChunkTopNHOutputState : public  PhysicalAtomState {
public:
    ChunkTopNHOutputState( const vector<LogicalType> &payloadTypes,const vector<ColModifier> &modifiers, idx_t limit):
        heap_(payloadTypes, modifiers, limit) {}

    TopNHeap heap_;
};


PhysicalTopNHOutput::PhysicalTopNHOutput(const vector<LogicalType> &types, vector<idx_t> &dcCols, PredicateTables *pt, const vector<ColModifier>& modifiers, idx_t limit)
    : PhysicalAtom(types), pt_(pt), modifiers_(modifiers), limit_(limit) {
    dcCols_ = std::move(dcCols);
    for (auto c : dcCols_)dcColsType_.push_back(types_[c]);
}

PhysicalTopNHOutput::~PhysicalTopNHOutput() {}


bool PhysicalTopNHOutput::isSink() const {
    return true;
}

string PhysicalTopNHOutput::getName() const {
    return "PHYSICAL_TOPN_HEAP_OUTPUT";
}

string PhysicalTopNHOutput::toString() const {
    auto result = getName();
    result += " (" + pt_->predicate_.get()->toString()+"; ";
    for (auto c : dcCols_) {
        result += std::to_string(c) + ", ";
    }
    result += ";";
    for (auto c : dcColsType_) {
        result += c.toString() + ", ";
    }
    result += "; LIMIT: " + std::to_string(limit_) + "; ";
    for (auto c : modifiers_) {
        result +=std::to_string(c.col_) + " " + c.modifier_.toString()+", ";
    }
    return result + ")";

}

pstate_ptr_t PhysicalTopNHOutput::getState() const {
    return pstate_ptr_t(new ChunkTopNHOutputState(dcColsType_, modifiers_, limit_));
}

gpstate_ptr_t PhysicalTopNHOutput::getGlobalState() const {
    return gpstate_ptr_t(new GlobalChunkTopNHOutputState(pt_, dcColsType_, modifiers_, limit_));
}


AtomResultType PhysicalTopNHOutput::sink(ThreadContext& context, DataChunk &input, PhysicalAtomState &state, GlobalPhysicalAtomState &gstate) const {


    context.profiler_.startPhysicalAtom(this);

    auto& cstate = (ChunkTopNHOutputState&)state;
    auto& gcstate = (GlobalChunkTopNHOutputState&)gstate;
    if (input.getSize() == 0) {
        context.profiler_.endPhysicalAtom(input);
        return AtomResultType::NEED_MORE_INPUT;
    }

    BB_ASSERT(input.getSize() > 0);
    DataChunk pinput = projectColumns(input);
    BB_ASSERT(pinput.columnCount() == dcColsType_.size());

    // add to the local top n heap
    cstate.heap_.sink(pinput);

    context.profiler_.endPhysicalAtom(pinput);
    return AtomResultType::HAVE_MORE_OUTPUT;
}

void PhysicalTopNHOutput::combine(ThreadContext &context, PhysicalAtomState &state,
    GlobalPhysicalAtomState &gstate) const {
    context.profiler_.startPhysicalAtom(this);

    auto& cstate = (ChunkTopNHOutputState&)state;
    auto& gcstate = (GlobalChunkTopNHOutputState&)gstate;
    DataChunk result;
    result.initializeEmpty(cstate.heap_.getPayloadTypes());
    if ( cstate.heap_.getSize() == 0) {

        context.profiler_.endPhysicalAtom(result);
        return;
    }

    cstate.heap_.finalize();
    gcstate.combineHeap(cstate.heap_);

    context.profiler_.endPhysicalAtom(result);
}


void PhysicalTopNHOutput::finalize(ThreadContext& context, GlobalPhysicalAtomState& gstate) const {
    context.profiler_.startPhysicalAtom(this);
    // send the chunks from the global state to predicate table
    auto& gcstate = (GlobalChunkTopNHOutputState&)gstate;

    gcstate.globalHeap_.finalize();

    DataChunk result;
    result.initializeEmpty(gcstate.globalHeap_.getPayloadTypes());

    idx_t pos = 0;
    while (true) {
        result.setCardinality(0);
        gcstate.globalHeap_.getData(result, pos);
        if (result.getSize() == 0) break;
        pt_->append(result);
        pos += result.getSize();
    }
    context.profiler_.endPhysicalAtomFinalize();
}
}
