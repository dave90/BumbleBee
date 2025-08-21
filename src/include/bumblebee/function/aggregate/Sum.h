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
#include <__filesystem/perms.h>

#include "bumblebee/function/Function.h"
#include "bumblebee/function/FunctionRegister.h"

namespace bumblebee{


template <class T>
struct SumState {
    T value;

    void initialize() {
        value = 0;
    }

    void combine(SumState<T>* other) {
        this->value += other->value;
    }
};

template <class INPUT_TYPE, class RESULT_TYPE>
struct SumOperation {

    static void initialize(SumState<RESULT_TYPE> *state) {
        state->initialize();
    }

    static void combine(SumState<RESULT_TYPE>* source, SumState<RESULT_TYPE> *target) {
        target->combine(source);
    }

    static void operation(INPUT_TYPE *input, SumState<RESULT_TYPE> *state) {
        state->value += ((RESULT_TYPE)(*input));
    }

    static void finalize(SumState<RESULT_TYPE> *state, RESULT_TYPE *result) {
        *result = state->value;
    }
};

class SumFunc {
public:
    // get the function from the type
    static function_ptr getFunction(ConstantType type) {
        string name = "#sum";
        switch (type) {
            case ConstantType::TINYINT: {
                auto func = AggregateFunction::unaryAggregate<SumState,int8_t,int64_t,SumOperation>(name, {type}, BIGINT);
                return function_ptr(new AggregateFunction(func));
            }
            case ConstantType::SMALLINT:{
                auto func = AggregateFunction::unaryAggregate<SumState,int16_t,int64_t,SumOperation>(name, {type}, BIGINT);
                return function_ptr(new AggregateFunction(func));
            }
            case ConstantType::INTEGER:{
                auto func = AggregateFunction::unaryAggregate<SumState,int32_t,int64_t,SumOperation>(name, {type}, BIGINT);
                return function_ptr(new AggregateFunction(func));
            }
            case ConstantType::BIGINT:{
                auto func = AggregateFunction::unaryAggregate<SumState,int64_t,int64_t,SumOperation>(name, {type}, BIGINT);
                return function_ptr(new AggregateFunction(func));
            }
            case ConstantType::UTINYINT:{
                auto func = AggregateFunction::unaryAggregate<SumState,uint8_t,uint64_t,SumOperation>(name, {type}, UBIGINT);
                return function_ptr(new AggregateFunction(func));
            }
            case ConstantType::USMALLINT:{
                auto func = AggregateFunction::unaryAggregate<SumState,uint16_t,uint64_t,SumOperation>(name, {type}, UBIGINT);
                return function_ptr(new AggregateFunction(func));
            }
            case ConstantType::UINTEGER:{
                auto func = AggregateFunction::unaryAggregate<SumState,uint32_t,uint64_t,SumOperation>(name, {type}, UBIGINT);
                return function_ptr(new AggregateFunction(func));
            }
            case ConstantType::UBIGINT:{
                auto func = AggregateFunction::unaryAggregate<SumState,uint64_t,uint64_t,SumOperation>(name, {type}, UBIGINT);
                return function_ptr(new AggregateFunction(func));
            }
            case ConstantType::FLOAT:{
                auto func = AggregateFunction::unaryAggregate<SumState,float,float,SumOperation>(name, {type}, FLOAT);
                return function_ptr(new AggregateFunction(func));
            }
            case ConstantType::DOUBLE:{
                auto func = AggregateFunction::unaryAggregate<SumState,double,double,SumOperation>(name, {type}, DOUBLE);
                return function_ptr(new AggregateFunction(func));
            }
            case ConstantType::STRING:	{
                ErrorHandler::errorNotImplemented("Aggregate string sum not supported");
            }

            default:
                ErrorHandler::errorNotImplemented("Unimplemented type for select operation!");
        }
        return nullptr;
    }

    // register the functions
    static void registerFunction(FunctionRegister& funcRegister) {
        std::vector<ConstantType> supportedTypes = {TINYINT, SMALLINT, INTEGER, BIGINT, UTINYINT, USMALLINT, UINTEGER, UBIGINT, DOUBLE, FLOAT};
        for (auto& c: supportedTypes) {
            funcRegister.registerFunction(getFunction(c));
        }
    }
};

}
