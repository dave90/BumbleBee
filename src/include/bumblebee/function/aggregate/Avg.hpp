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
struct AvgState {
    T value;
    uint64_t count;

    void initialize() {
        value = 0;
        count = 0;
    }

    void combine(AvgState<T>* other) {
        value += other->value;
        count += other->count;
    }
};

template <class INPUT_TYPE, class RESULT_TYPE>
struct AvgOperation {

    static void initialize(AvgState<RESULT_TYPE> *state) {
        state->initialize();
    }

    static void combine(AvgState<RESULT_TYPE>* source, AvgState<RESULT_TYPE> *target) {
        target->combine(source);
    }

    static void operation(INPUT_TYPE *input, AvgState<RESULT_TYPE> *state) {
        state->value += (RESULT_TYPE)(*input);
        ++state->count;
    }

    static void finalize(AvgState<RESULT_TYPE> *state, RESULT_TYPE *result) {
        *result = (RESULT_TYPE)((RESULT_TYPE)state->value / (RESULT_TYPE)(state->count));
    }
};

class AvgFunc {
public:
    // get the function from the type
    static function_ptr_t getFunction(ConstantType type) {
        string name = "#avg";
        switch (type) {
            case ConstantType::TINYINT: {
                auto func = AggregateFunction::unaryAggregate<AvgState,int8_t,double,AvgOperation>(name, {type}, DOUBLE);
                return function_ptr_t(new AggregateFunction(func));
            }
            case ConstantType::SMALLINT:{
                auto func = AggregateFunction::unaryAggregate<AvgState,int16_t,double,AvgOperation>(name, {type}, DOUBLE);
                return function_ptr_t(new AggregateFunction(func));
            }
            case ConstantType::INTEGER:{
                auto func = AggregateFunction::unaryAggregate<AvgState,int32_t,double,AvgOperation>(name, {type}, DOUBLE);
                return function_ptr_t(new AggregateFunction(func));
            }
            case ConstantType::BIGINT:{
                auto func = AggregateFunction::unaryAggregate<AvgState,int64_t,double,AvgOperation>(name, {type}, DOUBLE);
                return function_ptr_t(new AggregateFunction(func));
            }
            case ConstantType::UTINYINT:{
                auto func = AggregateFunction::unaryAggregate<AvgState,uint8_t,double,AvgOperation>(name, {type}, DOUBLE);
                return function_ptr_t(new AggregateFunction(func));
            }
            case ConstantType::USMALLINT:{
                auto func = AggregateFunction::unaryAggregate<AvgState,uint16_t,double,AvgOperation>(name, {type}, DOUBLE);
                return function_ptr_t(new AggregateFunction(func));
            }
            case ConstantType::UINTEGER:{
                auto func = AggregateFunction::unaryAggregate<AvgState,uint32_t,double,AvgOperation>(name, {type}, DOUBLE);
                return function_ptr_t(new AggregateFunction(func));
            }
            case ConstantType::UBIGINT:{
                auto func = AggregateFunction::unaryAggregate<AvgState,uint64_t,double,AvgOperation>(name, {type}, DOUBLE);
                return function_ptr_t(new AggregateFunction(func));
            }
            case ConstantType::FLOAT:{
                auto func = AggregateFunction::unaryAggregate<AvgState,float,double,AvgOperation>(name, {type}, DOUBLE);
                return function_ptr_t(new AggregateFunction(func));
            }
            case ConstantType::DOUBLE:{
                auto func = AggregateFunction::unaryAggregate<AvgState,double,double,AvgOperation>(name, {type}, DOUBLE);
                return function_ptr_t(new AggregateFunction(func));
            }
            case ConstantType::STRING:	{
                ErrorHandler::errorNotImplemented("Aggregate string sum not supported");
            }

            default:
                ErrorHandler::errorNotImplemented("Unimplemented type for min operation!");
        }
        return nullptr;
    }

    // register the functions
    static void registerFunction(FunctionRegister& funcRegister) {
        vector<ConstantType> supportedTypes = {TINYINT, SMALLINT, INTEGER, BIGINT, UTINYINT, USMALLINT, UINTEGER, UBIGINT, DOUBLE, FLOAT};
        for (auto& c: supportedTypes) {
            funcRegister.registerFunction(getFunction(c));
        }
    }
};

}
