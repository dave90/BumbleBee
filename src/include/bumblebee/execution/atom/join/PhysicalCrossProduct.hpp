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

namespace bumblebee{

// Cross product simple implementation
class PhysicalCrossProduct : public PhysicalAtom {
public:
    PhysicalCrossProduct(const vector<LogicalType> &types, vector<idx_t>& dcCols,vector<idx_t>& selectedCols, PredicateTables* pt);
    ~PhysicalCrossProduct() override = default;

    string getName() const override;
    string toString() const override;
    pstate_ptr_t getState() const override;
    AtomResultType execute(ThreadContext &context, DataChunk &input, DataChunk &chunk,
            PhysicalAtomState &state) const override;
private:
    // pointer to the pt to join
    PredicateTables* pt_;
};


}
