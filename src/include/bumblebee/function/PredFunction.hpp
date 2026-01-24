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
#include "Function.hpp"
#include "bumblebee/ClientContext.hpp"
#include "bumblebee/planner/filter/TableFilter.hpp"

namespace bumblebee{

struct FunctionOperatorData {
    virtual ~FunctionOperatorData() {
    }
};

using function_op_data_ptr_t =  std::unique_ptr<FunctionOperatorData> ;

typedef function_data_ptr_t (*pred_function_bind_t)(ClientContext &context,
                                                          vector<Value> &inputs,
                                                          vector<LogicalType> & inputTypes,
                                                          std::unordered_map<string, Value> &parameters,
                                                          vector<LogicalType> &returnTypes, vector<string> &names,
                                                          TableFilterSet& filters);

typedef function_op_data_ptr_t (*pred_function_init_t)(ClientContext &context, const FunctionData *bind_data);

typedef idx_t (*pred_function_max_threads_t)(ClientContext &context, const FunctionData *bind_data);

typedef void (*pred_function_combine_t)(ClientContext &context, const FunctionData *bind_data, FunctionOperatorData *operator_state);

typedef void (*pred_function_finalize_t)(ClientContext &context, const FunctionData *bind_data);

typedef void (*pred_function_t)(ClientContext &context, const FunctionData *bind_data,
                                 FunctionOperatorData *operator_state, DataChunk *input, DataChunk &output);


class PredFunction : public Function {
public:
    PredFunction(const string &name, const vector<LogicalType> &arguments,
        pred_function_t function, pred_function_bind_t bind_function, pred_function_init_t init_function,
        pred_function_max_threads_t max_thread_function,pred_function_finalize_t finalize_function, pred_function_combine_t combine_function )
        : Function(name, arguments),
          function_(function),
          bindFunction_(bind_function),
          initFunction_(init_function),
          maxThreadFunction_(max_thread_function),
          finalize_function_(finalize_function),
          combine_function_(combine_function){
    }


    pred_function_t function_;
    pred_function_bind_t bindFunction_;
    pred_function_init_t initFunction_;
    pred_function_max_threads_t maxThreadFunction_;
    pred_function_finalize_t finalize_function_;
    pred_function_combine_t combine_function_;

    // The named parameters of the function
    std::unordered_map<string, PhysicalType> namedParameters_;
};

using pred_function_ptr_t = std::shared_ptr<PredFunction>;


}
