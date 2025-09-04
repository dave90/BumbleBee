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
#include "bumblebee/catalog/Schema.hpp"
#include "bumblebee/execution/PhysicalAtom.hpp"
#include "bumblebee/parser/statement/Predicate.hpp"

namespace bumblebee{


// Scan the chunks and return the input to be processed
// The operator can also do a projection is cols to fetch are passed in the constructor
// The global state return the chunks to be processed of the operator (sync with mutex)
class PhysicalChunkScan : public PhysicalAtom {
public:
    PhysicalChunkScan(const vector<ConstantType> &types, vector<idx_t>& dcCols,vector<idx_t>& selectedCols, PredicateTables* pt);
    ~PhysicalChunkScan() override;

    AtomResultType getData(ThreadContext& context, DataChunk &chunk, PhysicalAtomState &state, GlobalPhysicalAtomState &gstate) const override;

    bool isSource() const override;
    string getName() const override;
    string toString() const override;
    pstate_ptr_t getState() const override;
    gpstate_ptr_t getGlobalState() const override;
    idx_t getMaxThreads() const override;


private:
    // pointer to the predicate table to source the data
    PredicateTables* pt_;
};


}
