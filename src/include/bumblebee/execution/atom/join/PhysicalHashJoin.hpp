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


// Physical Atom for Hash Join. Can be used as:
// - sink and source for build phase
// - executor for probe phase
class PhysicalHashJoin : public PhysicalAtom {
public:
    PhysicalHashJoin(const vector<LogicalType> &types, vector<idx_t>& dcCols, vector<idx_t>& selectedCols, PredicateTables* pt, vector<idx_t> keys, vector<idx_t> payloads,
        vector<idx_t> lkeys, vector<Expression>& conditions);
    PhysicalHashJoin(const vector<LogicalType> &types, vector<idx_t>& dcCols, vector<idx_t>& selectedCols, PredicateTables* pt, vector<idx_t> keys, vector<idx_t> payloads, PhysicalHashType type);

    ~PhysicalHashJoin() override;

    string getName() const override;
    string toString() const override;
    pstate_ptr_t getState() const override;
    idx_t getMaxThreads() const override;
    bool isSource() const override;
    bool isSink() const override;
    gpstate_ptr_t getGlobalState() const override;

    // Colelct the stats to build the HT
    AtomResultType sink(ThreadContext &context, DataChunk &input, PhysicalAtomState &state,
    GlobalPhysicalAtomState &gstate) const override;
    // Build the HT bucket by bucket
    AtomResultType getData(ThreadContext &context, DataChunk &chunk, PhysicalAtomState &state,
        GlobalPhysicalAtomState &gstate) const override;
    // Finalize the build
    void finalize(ThreadContext &context, GlobalPhysicalAtomState &gstate) const override;
    // Combine
    void combine(ThreadContext &context, PhysicalAtomState &state, GlobalPhysicalAtomState &gstate) const override;

    // Execute probe
    AtomResultType execute(ThreadContext &context, DataChunk &input, DataChunk &chunk,
            PhysicalAtomState &state) const override;

private:

    PredicateTables* pt_;
    vector<idx_t> keys_; // keys of the current predicate
    vector<idx_t> payloads_; // payloads of the current predicate
    vector<idx_t> lkeys_; // keys on the input dataset
    vector<Expression> conditions_;
    PhysicalHashType type_;
};


}
