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
#include "bumblebee/execution/PhysicalAtom.hpp"

namespace bumblebee{

// Piece-wise sort-merge join for inequality (theta) joins (a.x OP b.y, OP in {<,<=,>,>=}).
// Build side is indexed once into a SortMergeJoinIndex (columnar pieces, each independently
// sorted by a byte-comparable key). Probe morsels binary-search each piece and emit one output
// chunk per piece via zero-copy slicing. Used as:
// - COLLECT sink: thread-local build into a SortMergeJoinIndex, combined into the pt-owned index.
// - PROBE: resumable per-piece probe with optional residual conditions.
class PhysicalPieceWiseSortMergeJoin : public PhysicalAtom {
public:
    // PROBE: op_ is oriented as `probe OP build` (already normalized by the optimizer).
    // Only the single merge-key inequality is handled here; any other conditions are
    // produced by separate physical operators.
    PhysicalPieceWiseSortMergeJoin(const ClientContext& context, const vector<LogicalType>& types,
        vector<idx_t>& dcCols, vector<idx_t>& selectedCols, PredicateTables* pt,
        idx_t buildKeyCol, idx_t probeKeyCol, Binop op, vector<idx_t> payloads);
    // COLLECT (build sink).
    PhysicalPieceWiseSortMergeJoin(const ClientContext& context, const vector<LogicalType>& types,
        vector<idx_t>& dcCols, vector<idx_t>& selectedCols, PredicateTables* pt,
        idx_t buildKeyCol, vector<idx_t> payloads, PhysicalHashType type);
    ~PhysicalPieceWiseSortMergeJoin() override;

    bool isSink() const override;
    string getName() const override;
    string toString() const override;
    pstate_ptr_t getState() const override;
    gpstate_ptr_t getGlobalState() const override;

    AtomResultType execute(ThreadContext& context, DataChunk& input, DataChunk& chunk,
        PhysicalAtomState& state) const override;            // PROBE
    AtomResultType sink(ThreadContext& context, DataChunk& input, PhysicalAtomState& state,
        GlobalPhysicalAtomState& gstate) const override;     // COLLECT
    void combine(ThreadContext& context, PhysicalAtomState& state,
        GlobalPhysicalAtomState& gstate) const override;     // COLLECT
    void finalize(ThreadContext& context, GlobalPhysicalAtomState& gstate) const override; // COLLECT

private:
    const ClientContext& context_;
    PredicateTables* pt_;
    idx_t buildKeyCol_;            // key column index in the pt chunk
    idx_t probeKeyCol_;            // key column index in the input (probe) chunk
    Binop op_;                     // probe OP build
    vector<idx_t> payloads_;       // payload columns (cache key for the pt-owned index)
    PhysicalHashType type_;
};

}