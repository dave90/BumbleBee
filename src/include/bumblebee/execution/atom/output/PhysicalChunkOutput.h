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
#include "bumblebee/catalog/PredicateTables.h"
#include "bumblebee/execution/PhysicalAtom.h"
#include "bumblebee/output/OutputBuilder.h"

namespace bumblebee{

// Sink Physical Atom (patom) that receive a data chunk and push in
// predicate table and if is not internal output it
class PhysicalChunkOutput : PhysicalAtom {
public:
    PhysicalChunkOutput(const std::vector<ConstantType> &types, idx_t estimated_cardinality, PredicateTables *pt,
        OutputBuilder& obuilder);
    PhysicalChunkOutput(const std::vector<ConstantType> &types, idx_t estimated_cardinality, PredicateTables *pt,
        std::vector<idx_t> &cols, OutputBuilder& obuilder);

    ~PhysicalChunkOutput() override;
    AtomResultType sink(DataChunk &input, PhysicalAtomState &state, GlobalPhysicalAtomState &gstate) const override;
    void finalize(GlobalPhysicalAtomState& gstate) const override;
    DataChunk projectColumns(DataChunk &input) const;
    bool isSink() const override;
    string getName() const override;
    string toString() const override;
    pstate_ptr_t getState() const override;
    gpstate_ptr_t getGlobalState() const override;

private:
    // output the chunk
    void outputChunk(data_chunk_ptr_t& chunk) const;

    // pointer to the predicate table to store the data
    PredicateTables* pt_;
    // cols to project
    std::vector<idx_t> cols_;
    // Reference of the Output builder
    OutputBuilder& obuilder_;
};


}
