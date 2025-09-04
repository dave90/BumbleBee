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
#include "bumblebee/execution/Expression.h"
#include "bumblebee/execution/PhysicalAtom.h"

namespace bumblebee{


class PhysicalNestedLoop : public PhysicalAtom{
public:
    // conditions must be in the following format:
    // - only 2 columns (one for left and one for right)
    // - left col ref to a col in input data chunk
    // - right col ref to a col in predicate table
    PhysicalNestedLoop(const vector<ConstantType> &types, vector<idx_t> &dcCols,
        vector<idx_t> &selectedCols, PredicateTables* pt, vector<Expression> &conditions);

    ~PhysicalNestedLoop() override;
    string getName() const override;
    string toString() const override;
    pstate_ptr_t getState() const override;
    AtomResultType execute(ThreadContext &context, DataChunk &input, DataChunk &chunk,
        PhysicalAtomState &state) const override;

private:

    PredicateTables* pt_;
    vector<Expression> conditions_;
};


}
