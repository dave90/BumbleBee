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

#include "bumblebee/function/AggregateFunction.hpp"
#include "bumblebee/function/Function.hpp"
#include "bumblebee/function/FunctionRegister.hpp"

namespace bumblebee{


template <class T>
struct MaxState {
    T value;
    bool init;

    void initialize() {
        init = false;
    }

    void combine(MaxState<T>* other) {
        value = (other->init && ( !init || value < other->value))?other->value: value;
        init = true;
    }
};

template <class INPUT_TYPE, class RESULT_TYPE>
struct MaxOperation {

    static void initialize(MaxState<RESULT_TYPE> *state) {
        state->initialize();
    }

    static void combine(MaxState<RESULT_TYPE>* source, MaxState<RESULT_TYPE> *target) {
        target->combine(source);
    }

    static void operation(INPUT_TYPE *input, MaxState<RESULT_TYPE> *state) {
        state->value = (!state->init || state->value < (RESULT_TYPE)(*input))? ((RESULT_TYPE)(*input)): state->value;
        state->init = true;
    }

    static void finalize(MaxState<RESULT_TYPE> *state, RESULT_TYPE *result) {
        *result = state->value;
    }
};

class MaxFunc {
public:
    // get the function from the type
    static function_ptr_t getFunction(ConstantType type) {
        string name = "#max";
        switch (type) {
            case ConstantType::TINYINT: {
                auto func = AggregateFunction::unaryAggregate<MaxState,int8_t,int64_t,MaxOperation>(name, {type}, BIGINT);
                return function_ptr_t(new AggregateFunction(func));
            }
            case ConstantType::SMALLINT:{
                auto func = AggregateFunction::unaryAggregate<MaxState,int16_t,int64_t,MaxOperation>(name, {type}, BIGINT);
                return function_ptr_t(new AggregateFunction(func));
            }
            case ConstantType::INTEGER:{
                auto func = AggregateFunction::unaryAggregate<MaxState,int32_t,int64_t,MaxOperation>(name, {type}, BIGINT);
                return function_ptr_t(new AggregateFunction(func));
            }
            case ConstantType::BIGINT:{
                auto func = AggregateFunction::unaryAggregate<MaxState,int64_t,int64_t,MaxOperation>(name, {type}, BIGINT);
                return function_ptr_t(new AggregateFunction(func));
            }
            case ConstantType::UTINYINT:{
                auto func = AggregateFunction::unaryAggregate<MaxState,uint8_t,uint64_t,MaxOperation>(name, {type}, UBIGINT);
                return function_ptr_t(new AggregateFunction(func));
            }
            case ConstantType::USMALLINT:{
                auto func = AggregateFunction::unaryAggregate<MaxState,uint16_t,uint64_t,MaxOperation>(name, {type}, UBIGINT);
                return function_ptr_t(new AggregateFunction(func));
            }
            case ConstantType::UINTEGER:{
                auto func = AggregateFunction::unaryAggregate<MaxState,uint32_t,uint64_t,MaxOperation>(name, {type}, UBIGINT);
                return function_ptr_t(new AggregateFunction(func));
            }
            case ConstantType::UBIGINT:{
                auto func = AggregateFunction::unaryAggregate<MaxState,uint64_t,uint64_t,MaxOperation>(name, {type}, UBIGINT);
                return function_ptr_t(new AggregateFunction(func));
            }
            case ConstantType::FLOAT:{
                auto func = AggregateFunction::unaryAggregate<MaxState,float,float,MaxOperation>(name, {type}, FLOAT);
                return function_ptr_t(new AggregateFunction(func));
            }
            case ConstantType::DOUBLE:{
                auto func = AggregateFunction::unaryAggregate<MaxState,double,double,MaxOperation>(name, {type}, DOUBLE);
                return function_ptr_t(new AggregateFunction(func));
            }
            case ConstantType::STRING:	{
                auto func = AggregateFunction::unaryAggregate<MaxState,string_t,string_t,MaxOperation>(name, {type}, STRING);
                return function_ptr_t(new AggregateFunction(func));
            }

            default:
                ErrorHandler::errorNotImplemented("Unimplemented type for min operation!");
        }
        return nullptr;
    }

    // register the functions
    static void registerFunction(FunctionRegister& funcRegister) {
        vector<ConstantType> supportedTypes = {TINYINT, SMALLINT, INTEGER, BIGINT, UTINYINT, USMALLINT, UINTEGER, UBIGINT, DOUBLE, FLOAT, STRING};
        for (auto& c: supportedTypes) {
            funcRegister.registerFunction(getFunction(c));
        }
    }
};

}
