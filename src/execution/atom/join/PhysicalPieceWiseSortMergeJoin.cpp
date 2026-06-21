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
#include "bumblebee/execution/atom/join/PhysicalPieceWiseSortMergeJoin.hpp"

#include "bumblebee/ClientContext.hpp"
#include "bumblebee/common/Mutex.hpp"
#include "bumblebee/common/vector_operations/CreateSortKey.hpp"
#include "bumblebee/execution/SortMergeJoinIndex.hpp"

namespace bumblebee{

// ----- state classes (mirror RLJoinHTAtomState / GlobalRLJoinHTAtomState) -----

// COLLECT thread-local: owns a thread-local index that is finalized and merged on combine().
class SortMergeBuildState : public PhysicalAtomState {
public:
    SortMergeBuildState(BufferManager& mgr, const vector<LogicalType>& types, idx_t keyCol,
                        const vector<idx_t>& payloads)
        : local_(new SortMergeJoinIndex(mgr, types, keyCol, payloads)) {}

    sort_merge_index_ptr_t local_;
};

// COLLECT global: the pt-owned index plus a mutex guarding mergeFrom.
class GlobalSortMergeState : public GlobalPhysicalAtomState {
public:
    explicit GlobalSortMergeState(SortMergeJoinIndex* index) : index_(index) {}
    GlobalSortMergeState() : index_(nullptr) {}

    SortMergeJoinIndex* index_;
    mutex mutex_;
};

// PROBE: cursors into the index, plus the per-morsel probe sort keys.
class SortMergeProbeState : public PhysicalAtomState {
public:
    explicit SortMergeProbeState(SortMergeJoinIndex* index) : index_(index) {}

    void reset() {
        lpos_ = 0;
        pieceIdx_ = 0;
        cursor_ = 0;
        lo_ = 0;
        hi_ = 0;
        rangeComputed_ = false;
        keysBuilt_ = false;
    }

    SortMergeJoinIndex* index_;
    Vector probeKeys_{LogicalTypeId::STRING}; // byte-comparable key per probe row
    bool keysBuilt_{false};

    idx_t lpos_{0};          // current probe row
    idx_t pieceIdx_{0};      // current build piece
    idx_t lo_{0}, hi_{0};    // matched [lo,hi) within the current piece for lpos_
    idx_t cursor_{0};        // position within [lo,hi)
    bool rangeComputed_{false};
};

// ----- ctors -----

PhysicalPieceWiseSortMergeJoin::PhysicalPieceWiseSortMergeJoin(const ClientContext& context,
    const vector<LogicalType>& types, vector<idx_t>& dcCols, vector<idx_t>& selectedCols, PredicateTables* pt,
    idx_t buildKeyCol, idx_t probeKeyCol, Binop op, vector<idx_t> payloads)
    : PhysicalAtom(types, dcCols, selectedCols), context_(context), pt_(pt), buildKeyCol_(buildKeyCol),
      probeKeyCol_(probeKeyCol), op_(op), payloads_(std::move(payloads)), type_(PROBE) {
}

PhysicalPieceWiseSortMergeJoin::PhysicalPieceWiseSortMergeJoin(const ClientContext& context,
    const vector<LogicalType>& types, vector<idx_t>& dcCols, vector<idx_t>& selectedCols, PredicateTables* pt,
    idx_t buildKeyCol, vector<idx_t> payloads, PhysicalHashType type)
    : PhysicalAtom(types, dcCols, selectedCols), context_(context), pt_(pt), buildKeyCol_(buildKeyCol),
      probeKeyCol_(0), op_(NONE_OP), payloads_(std::move(payloads)), type_(type) {
}

PhysicalPieceWiseSortMergeJoin::~PhysicalPieceWiseSortMergeJoin() = default;

bool PhysicalPieceWiseSortMergeJoin::isSink() const {
    return type_ == COLLECT;
}

string PhysicalPieceWiseSortMergeJoin::getName() const {
    return "PHYSICAL_PIECE_WISE_SORT_MERGE_JOIN";
}

string PhysicalPieceWiseSortMergeJoin::toString() const {
    string result = getName() + " (" + pt_->predicate_->getName() + "; ";
    result += "buildKey=" + std::to_string(buildKeyCol_) + " " + getBinopStr(op_) +
              " probeKey=" + std::to_string(probeKeyCol_) + "; ";
    for (auto c : dcCols_) result += std::to_string(c) + ", ";
    result += "; ";
    for (auto c : selectCols_) result += std::to_string(c) + ", ";
    return result + ")";
}

pstate_ptr_t PhysicalPieceWiseSortMergeJoin::getState() const {
    if (type_ == COLLECT)
        return pstate_ptr_t(new SortMergeBuildState(*context_.bufferManager_, pt_->getTypes(), buildKeyCol_, payloads_));
    return pstate_ptr_t(new SortMergeProbeState(pt_->getSortMergeIndex(buildKeyCol_, payloads_).get()));
}

gpstate_ptr_t PhysicalPieceWiseSortMergeJoin::getGlobalState() const {
    if (type_ == COLLECT)
        return gpstate_ptr_t(new GlobalSortMergeState(pt_->getSortMergeIndex(buildKeyCol_, payloads_).get()));
    return gpstate_ptr_t(new GlobalSortMergeState());
}

// ----- COLLECT (build) -----

AtomResultType PhysicalPieceWiseSortMergeJoin::sink(ThreadContext& context, DataChunk& input,
    PhysicalAtomState& state, GlobalPhysicalAtomState& gstate) const {
    context.profiler_.startPhysicalAtom(this);
    if (input.getSize() == 0) {
        context.profiler_.endPhysicalAtom(input);
        return AtomResultType::NEED_MORE_INPUT;
    }
    auto& cstate = (SortMergeBuildState&)state;
    cstate.local_->addChunk(input); // full columns, thread-local, no lock
    context.profiler_.endPhysicalAtom(input);
    return AtomResultType::HAVE_MORE_OUTPUT;
}

void PhysicalPieceWiseSortMergeJoin::combine(ThreadContext& context, PhysicalAtomState& state,
    GlobalPhysicalAtomState& gstate) const {
    context.profiler_.startPhysicalAtom(this);
    auto& cstate = (SortMergeBuildState&)state;
    auto& cgstate = (GlobalSortMergeState&)gstate;
    // finalize the thread-local FIRST (seals its open_ piece) so mergeFrom only moves sealed pieces
    cstate.local_->finalize();
    {
        lock_guard lock(cgstate.mutex_);
        cgstate.index_->mergeFrom(*cstate.local_);
    }
    context.profiler_.endPhysicalAtomCombine();
}

void PhysicalPieceWiseSortMergeJoin::finalize(ThreadContext& context, GlobalPhysicalAtomState& gstate) const {
    if (type_ != COLLECT) return;
    auto& cgstate = (GlobalSortMergeState&)gstate;
    context.profiler_.startPhysicalAtom(this);
    cgstate.index_->finalize(); // seal the global trailing open_ piece, mark ready
    BB_ASSERT(cgstate.index_->isReady());
    context.profiler_.endPhysicalAtomFinalize();
}

// ----- PROBE -----

AtomResultType PhysicalPieceWiseSortMergeJoin::execute(ThreadContext& context, DataChunk& input,
    DataChunk& chunk, PhysicalAtomState& state) const {
    context.profiler_.startPhysicalAtom(this);
    auto& cstate = (SortMergeProbeState&)state;

    if (input.getSize() == 0) {
        cstate.reset();
        chunk.reset();
        context.profiler_.endPhysicalAtom(chunk);
        return AtomResultType::NEED_MORE_INPUT;
    }

    // build the probe sort keys once per morsel
    if (!cstate.keysBuilt_) {
        CreateSortKey::createSortKey(input.data_[probeKeyCol_], input.getSize(), {OrderType::ASCENDING}, cstate.probeKeys_);
        cstate.keysBuilt_ = true;
        cstate.lpos_ = 0;
        cstate.pieceIdx_ = 0;
        cstate.rangeComputed_ = false;
    }

    auto probeKeys = FlatVector::getData<string_t>(cstate.probeKeys_);
    auto& pieces = cstate.index_->pieces();

    idx_t produced = 0;
    while (produced == 0) {
        // advance cursors to the next non-empty candidate range
        if (cstate.lpos_ >= input.getSize()) break;                 // morsel done
        if (cstate.pieceIdx_ >= pieces.size()) {                    // pieces exhausted for this probe row
            cstate.lpos_++;
            cstate.pieceIdx_ = 0;
            cstate.rangeComputed_ = false;
            continue;
        }
        const auto& piece = pieces[cstate.pieceIdx_];
        if (!cstate.rangeComputed_) {
            SortMergeJoinIndex::rangeFor(piece, probeKeys[cstate.lpos_], op_, cstate.lo_, cstate.hi_);
            cstate.cursor_ = cstate.lo_;
            cstate.rangeComputed_ = true;
        }
        if (cstate.cursor_ >= cstate.hi_) {                         // this piece exhausted for lpos_
            cstate.pieceIdx_++;
            cstate.rangeComputed_ = false;
            continue;
        }

        // emit one output chunk [cursor_, cursor_ + n)
        idx_t n = minValue<idx_t>(cstate.hi_ - cstate.cursor_, STANDARD_VECTOR_SIZE);
        SelectionVector leftSel(n), buildSel(n);
        for (idx_t k = 0; k < n; ++k) {
            leftSel.setIndex(k, cstate.lpos_);
            buildSel.setIndex(k, piece.sortedRowIdx_[cstate.cursor_ + k]);
        }
        cstate.cursor_ += n;

        // left cols from input, build cols from the piece payload placed at dcCols_.
        // Project the (const) full payload down to selectCols_ without copying — reference accepts const.
        chunk.slice(input, leftSel, n);
        DataChunk rSelChunk;
        rSelChunk.initializeEmpty(dcColsType_);
        for (idx_t i = 0; i < selectCols_.size(); ++i)
            rSelChunk.data_[i].reference(piece.payload_.data_[selectCols_[i]]);
        rSelChunk.setCardinality(piece.payload_.getSize());
        chunk.slice(rSelChunk, buildSel, n, dcCols_);
        produced = n;
    }

    context.profiler_.endPhysicalAtom(chunk);

    if (produced == 0) {
        cstate.reset();
        chunk.reset();
        return AtomResultType::NEED_MORE_INPUT;
    }
    return AtomResultType::HAVE_MORE_OUTPUT;
}

}
