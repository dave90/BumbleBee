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
#include "bumblebee/execution/PhysicalAtom.h"

namespace bumblebee{

enum PhysicalHashJoinType : uint8_t {
    PROBE = 0,
    STATS = 1,
    BUILD = 2
};

class PhysicalHashJoin : public PhysicalAtom {
public:
    PhysicalHashJoin(const std::vector<ConstantType> &types, std::vector<idx_t>& dcCols, std::vector<idx_t>& selectedCols,
        idx_t estimated_cardinality, PredicateTables* pt, std::vector<idx_t> keys,
        std::vector<idx_t> lkeys, std::vector<Expression>& conditions);
    PhysicalHashJoin(const std::vector<ConstantType> &types, std::vector<idx_t>& dcCols, std::vector<idx_t>& selectedCols,
        idx_t estimated_cardinality, PredicateTables* pt, std::vector<idx_t> keys, PhysicalHashJoinType type);

    ~PhysicalHashJoin() override;

    string getName() const override;
    string toString() const override;
    pstate_ptr_t getState() const override;
    idx_t getMaxThreads() const override;
    bool isSource() const override;
    bool isSink() const override;
    gpstate_ptr_t getGlobalState() const override;

    void finalize(ThreadContext &context, GlobalPhysicalAtomState &gstate) const override;
    AtomResultType execute(ThreadContext &context, DataChunk &input, DataChunk &chunk,
            PhysicalAtomState &state) const override;

    AtomResultType getData(ThreadContext &context, DataChunk &chunk, PhysicalAtomState &state,
        GlobalPhysicalAtomState &gstate) const override;

    AtomResultType sink(ThreadContext &context, DataChunk &input, PhysicalAtomState &state,
        GlobalPhysicalAtomState &gstate) const override;

private:
    DataChunk selectColumns(DataChunk &chunk) const;
    DataChunk projectColumns(DataChunk &input) const;

    PredicateTables* pt_;
    std::vector<idx_t> keys_; // keys of the current predicate
    std::vector<idx_t> lkeys_; // keys on the input dataset
    std::vector<Expression> conditions_;
    PhysicalHashJoinType type_;
};


}
