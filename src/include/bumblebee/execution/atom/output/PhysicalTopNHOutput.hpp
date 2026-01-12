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
#include "bumblebee/catalog/PredicateTables.hpp"
#include "bumblebee/execution/PhysicalAtom.hpp"
#include "bumblebee/execution/TopNHeap.hpp"
#include "bumblebee/output/OutputBuilder.hpp"

namespace bumblebee{

// Sink Physical Atom (patom) that receive a data chunk and push  TOP N results in
// predicate table
class PhysicalTopNHOutput : public PhysicalAtom {
public:
    PhysicalTopNHOutput(const vector<LogicalType> &types, vector<idx_t> &dcCols, PredicateTables *pt, const vector<ColModifier>& modifiers, idx_t limit);

    ~PhysicalTopNHOutput() override;
    AtomResultType sink(ThreadContext& context, DataChunk &input, PhysicalAtomState &state, GlobalPhysicalAtomState &gstate) const override;
    void finalize(ThreadContext& context, GlobalPhysicalAtomState& gstate) const override;
    void combine(ThreadContext &context, PhysicalAtomState &state, GlobalPhysicalAtomState &gstate) const override;

    bool isSink() const override;
    string getName() const override;
    string toString() const override;
    pstate_ptr_t getState() const override;
    gpstate_ptr_t getGlobalState() const override;

private:

    // pointer to the predicate table to store the data
    PredicateTables* pt_;

    const vector<ColModifier>& modifiers_;
    idx_t limit_;
};


}
