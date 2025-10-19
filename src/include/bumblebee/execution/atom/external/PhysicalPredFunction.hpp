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
#include "bumblebee/function/PredFunction.hpp"

namespace bumblebee{


class PhysicalPredFunction : public PhysicalAtom {
public:
    PhysicalPredFunction(ClientContext &context, const vector<ConstantType> &types, vector<idx_t> &dcCols, vector<idx_t> &selectedCols,
         PredFunction *pred_function, function_data_ptr_t &bind_data);

    PhysicalPredFunction(ClientContext &context, const vector<ConstantType> &types,vector<idx_t> &dcCols,
        PredFunction *pred_function, function_data_ptr_t &bind_data);

    ~PhysicalPredFunction() override;

    idx_t getMaxThreads() const override;
    bool isSource() const override;
    bool isSink() const override;


    string getName() const override;
    string toString() const override;
    pstate_ptr_t getState() const override;
    gpstate_ptr_t getGlobalState() const override;

    AtomResultType getData(ThreadContext &context, DataChunk &chunk, PhysicalAtomState &state,
                            GlobalPhysicalAtomState &gstate) const override;

    AtomResultType sink(ThreadContext &context, DataChunk &input, PhysicalAtomState &state,
    GlobalPhysicalAtomState &gstate) const override;

    void finalize(ThreadContext &context, GlobalPhysicalAtomState &gstate) const override;

    void combine(ThreadContext &context, PhysicalAtomState &state, GlobalPhysicalAtomState &gstate) const override;


private:
    ClientContext& context_;
    PredFunction* predFunction_;
    function_data_ptr_t bindData_;
};


}
