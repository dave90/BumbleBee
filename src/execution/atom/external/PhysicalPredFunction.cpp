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
#include "bumblebee/execution/atom/external/PhysicalPredFunction.hpp"

namespace bumblebee{


class GlobalPredScanState : public  GlobalPhysicalAtomState {

};

class PredFuncState : public  PhysicalAtomState {
public:
    explicit PredFuncState(function_op_data_ptr_t& function_op_data)
        : functionOpData_(std::move(function_op_data)) {
    }
    bool isInitialized_{false};
    DataChunk cachedChunk_;
    function_op_data_ptr_t functionOpData_;
};

PhysicalPredFunction::PhysicalPredFunction(ClientContext &context, const vector<LogicalType> &types, vector<idx_t> &dcCols,
    vector<idx_t> &selectedCols, PredFunction *pred_function,
    function_data_ptr_t &bind_data): PhysicalAtom(types, dcCols, selectedCols),
                                           context_(context),
                                           predFunction_(pred_function),
                                           bindData_(std::move(bind_data)) {
}

PhysicalPredFunction::PhysicalPredFunction(ClientContext &context, const vector<LogicalType> &types,
vector<idx_t> &dcCols, PredFunction *pred_function, function_data_ptr_t &bind_data):
                                        PhysicalAtom(types),
                                       context_(context),
                                       predFunction_(pred_function),
                                       bindData_(std::move(bind_data)) {
    dcCols_ = std::move(dcCols);
    for (auto c : dcCols_)dcColsType_.push_back(types_[c]);
}

PhysicalPredFunction::~PhysicalPredFunction() {
}

idx_t PhysicalPredFunction::getMaxThreads() const {
    return predFunction_->maxThreadFunction_(context_, bindData_.get());
}

bool PhysicalPredFunction::isSource() const {
    return true;
}

bool PhysicalPredFunction::isSink() const {
    return true;
}

string PhysicalPredFunction::getName() const {
    return "PHYSICAL_PRED_FUNCTION";
}

string PhysicalPredFunction::toString() const {
    auto result = getName();
    result += " (" + predFunction_->name_+"; ";
    for (auto c : dcCols_) {
        result += std::to_string(c) + ", ";
    }
    result += "; ";
    for (auto c : selectCols_) {
        result += std::to_string(c) + ", ";
    }
    result += "; ";
    for (auto c : dcColsType_) {
        result += c.toString() + ", ";
    }
    return result + ")";
}

pstate_ptr_t PhysicalPredFunction::getState() const {
    auto funcOpData = predFunction_->initFunction_(context_, bindData_.get());
    return pstate_ptr_t(new PredFuncState(funcOpData));
}

gpstate_ptr_t PhysicalPredFunction::getGlobalState() const {
    return gpstate_ptr_t(new GlobalPredScanState());
}

AtomResultType PhysicalPredFunction::getData(ThreadContext &context, DataChunk &chunk, PhysicalAtomState &state,
    GlobalPhysicalAtomState &gstate) const {
    context.profiler_.startPhysicalAtom(this);
    auto& cstate = (PredFuncState&)state;

    if (!cstate.isInitialized_) {
        // calculate the types of the chunk
        vector<LogicalType> types;
        for (auto col:selectCols_)
            types.push_back(types_[col]);
        cstate.cachedChunk_.initialize(types);
        cstate.isInitialized_ = true;
    }
    cstate.cachedChunk_.setCardinality(0);
    predFunction_->function_(context_, bindData_.get(), cstate.functionOpData_.get(),nullptr, cstate.cachedChunk_);
    if (cstate.cachedChunk_.getSize()) {
        chunk.reference(cstate.cachedChunk_, selectCols_);
        context.profiler_.endPhysicalAtom(chunk);
        return AtomResultType::HAVE_MORE_OUTPUT;
    }
    chunk.setCardinality(0);
    context.profiler_.endPhysicalAtom(chunk);
    return AtomResultType::FINISHED;
}


AtomResultType PhysicalPredFunction::sink(ThreadContext &context, DataChunk &input, PhysicalAtomState &state,
    GlobalPhysicalAtomState &gstate) const {
    context.profiler_.startPhysicalAtom(this);
    auto& cstate = (PredFuncState&)state;
    if (input.getSize() == 0) {
        context.profiler_.endPhysicalAtom(input);
        return AtomResultType::FINISHED;
    }

    DataChunk pinput = projectColumns(input);

    predFunction_->function_(context_, bindData_.get(), cstate.functionOpData_.get(),nullptr, pinput);

    context.profiler_.endPhysicalAtom(input);
    return AtomResultType::NEED_MORE_INPUT;
}

AtomResultType PhysicalPredFunction::execute(ThreadContext &context, DataChunk &input, DataChunk &chunk,
    PhysicalAtomState &state) const {
    auto& cstate = (PredFuncState&)state;
    context.profiler_.startPhysicalAtom(this);

    // create output chunk with all column types
    DataChunk output;
    output.initializeEmpty(types_);

    predFunction_->function_(context_, bindData_.get(), cstate.functionOpData_.get(), &input, output);

    if (output.getSize() > 0) {
        chunk.reference(output);
    } else {
        chunk.setCardinality(0);
    }

    context.profiler_.endPhysicalAtom(chunk);
    return AtomResultType::NEED_MORE_INPUT;
}

void PhysicalPredFunction::finalize(ThreadContext &context, GlobalPhysicalAtomState &gstate) const {
    context.profiler_.startPhysicalAtom(this);

    if (predFunction_->finalize_function_)
        predFunction_->finalize_function_(context_, bindData_.get());

    context.profiler_.endPhysicalAtomFinalize();
}

void PhysicalPredFunction::combine(ThreadContext &context, PhysicalAtomState &state,
    GlobalPhysicalAtomState &gstate) const {

    auto& cstate = (PredFuncState&)state;
    predFunction_->combine_function_(context_, bindData_.get(), cstate.functionOpData_.get());

}
}
