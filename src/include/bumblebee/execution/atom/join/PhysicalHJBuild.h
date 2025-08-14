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
#include "bumblebee/execution/JoinHashTable.h"
#include "bumblebee/execution/PhysicalAtom.h"

namespace bumblebee{


class PhysicalHJBuild : public PhysicalAtom{
public:
    PhysicalHJBuild(const std::vector<ConstantType> &types, std::vector<idx_t>& dcCols, std::vector<idx_t>& selectedCols,
idx_t estimated_cardinality, PredicateTables* pt, std::vector<idx_t> keys, bool isSink);
    ~PhysicalHJBuild() override;

    void finalize(ThreadContext &context, GlobalPhysicalAtomState &gstate) const override;
    bool isSource() const override;
    bool isSink() const override;
    string getName() const override;
    string toString() const override;
    gpstate_ptr_t getGlobalState() const override;
    idx_t getMaxThreads() const override;

    AtomResultType getData(ThreadContext &context, DataChunk &chunk, PhysicalAtomState &state,
    GlobalPhysicalAtomState &gstate) const override;

    AtomResultType sink(ThreadContext &context, DataChunk &input, PhysicalAtomState &state,
        GlobalPhysicalAtomState &gstate) const override;

private:
    DataChunk projectColumns(DataChunk &input) const;
    bool sink_;
    PredicateTables* pt_;
    std::vector<idx_t> keys_;
};


}
