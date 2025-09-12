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

// Physical atom for aggregates
class PhysicalPartitionedAggHT : public PhysicalAtom {

public:
    // constructor for sink and source
    PhysicalPartitionedAggHT(const ClientContext& context, const vector<ConstantType> &types, vector<idx_t> &dcCols,vector<idx_t> &selectedCols, PredicateTables *pt, const vector<idx_t> &group_cols,
        const vector<idx_t> &payload_cols, const vector<AggregateFunction *> &aggregate_functions,
        PhysicalHashType type);
    // constructor for probe
    PhysicalPartitionedAggHT(const ClientContext& context, const vector<ConstantType> &types, vector<idx_t> &dcCols,vector<idx_t> &selectedCols, const vector<idx_t> &group_cols,
        const vector<idx_t> &payload_cols, AggregatePRLHashTable* aht);

    ~PhysicalPartitionedAggHT() override = default;

    idx_t getMaxThreads() const override;
    bool isSource() const override;
    bool isSink() const override;
    string toString() const override;
    pstate_ptr_t getState() const override;
    gpstate_ptr_t getGlobalState() const override;
    string getName() const override;

    void finalize(ThreadContext &context, GlobalPhysicalAtomState &gstate) const override;
    AtomResultType execute(ThreadContext &context, DataChunk &input, DataChunk &chunk,
        PhysicalAtomState &state) const override;
    AtomResultType getData(ThreadContext &context, DataChunk &chunk, PhysicalAtomState &state,
        GlobalPhysicalAtomState &gstate) const override;
    AtomResultType sink(ThreadContext &context, DataChunk &input, PhysicalAtomState &state,
        GlobalPhysicalAtomState &gstate) const override;
    void combine(ThreadContext &context, PhysicalAtomState &state, GlobalPhysicalAtomState &gstate) const override;

private:
    const ClientContext& context_;

    PredicateTables* pt_;
    // used during probe phase
    AggregatePRLHashTable* aht_;
    // during build phase point to the internal predicate table columns
    // on probe point to the group column in input data chunk
    vector<idx_t> groupCols_;
    vector<ConstantType> groupColsTypes_;
    // during build phase point the payloads columns to aggregate
    // during probe phase the payload columns to fetch
    vector<idx_t> payloadCols_;
    vector<ConstantType> payloadColsTypes_;
    vector<AggregateFunction*> aggregateFunctions_;
    PhysicalHashType type_;
};


}
