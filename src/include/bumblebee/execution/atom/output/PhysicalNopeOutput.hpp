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

// Physical output atom that do nothing
class PhysicalNopeOutput : public  PhysicalAtom{
public:
    PhysicalNopeOutput(const vector<LogicalType> &types, vector<idx_t> &dcCols,
        vector<idx_t> &selectedCols)
        : PhysicalAtom(types, dcCols, selectedCols) {
    }

    ~PhysicalNopeOutput() override{};

    AtomResultType sink(ThreadContext &context, DataChunk &input, PhysicalAtomState &state,
        GlobalPhysicalAtomState &gstate) const override {
        context.profiler_.startPhysicalAtom(this);
        context.profiler_.endPhysicalAtom(input);
        return  AtomResultType::NEED_MORE_INPUT ;
    }

    void finalize(ThreadContext &context, GlobalPhysicalAtomState &gstate) const override {
        context.profiler_.startPhysicalAtom(this);
        context.profiler_.endPhysicalAtomFinalize();
    }

    string getName() const override {
        return "PHYSICAL_NOPE";
    }
    string toString() const override {
        return getName() + "( )";
    }

    bool isSink() const override {
        return true;
    }
};


}
