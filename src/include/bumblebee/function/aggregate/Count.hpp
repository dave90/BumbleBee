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
struct CountState {
    uint64_t count;

    void initialize() {
        count = 0;
    }

    void combine(CountState<T>* other) {
        count += other->count;
    }
};

template <class INPUT_TYPE, class RESULT_TYPE>
struct CountOperation {

    static void initialize(CountState<RESULT_TYPE> *state) {
        state->initialize();
    }

    static void combine(CountState<RESULT_TYPE>* source, CountState<RESULT_TYPE> *target) {
        target->combine(source);
    }

    static void operation(INPUT_TYPE *input, CountState<RESULT_TYPE> *state) {
        ++state->count;
    }

    static void finalize(CountState<RESULT_TYPE> *state, RESULT_TYPE *result) {
        *result = (RESULT_TYPE)state->count;
    }
};

class CountFunc {
public:
    // get the function from the type
    static function_ptr_t getFunction(LogicalType type) {
        string name = "#count";
        switch (type.type()) {
            case LogicalTypeId::TINYINT: {
                auto func = AggregateFunction::unaryAggregate<CountState,int8_t,uint64_t,CountOperation>(name, {type}, LogicalTypeId::UBIGINT);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::SMALLINT:{
                auto func = AggregateFunction::unaryAggregate<CountState,int16_t,uint64_t,CountOperation>(name, {type}, LogicalTypeId::UBIGINT);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::INTEGER:{
                auto func = AggregateFunction::unaryAggregate<CountState,int32_t,uint64_t,CountOperation>(name, {type}, LogicalTypeId::UBIGINT);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::BIGINT:{
                auto func = AggregateFunction::unaryAggregate<CountState,int64_t,uint64_t,CountOperation>(name, {type}, LogicalTypeId::UBIGINT);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::UTINYINT:{
                auto func = AggregateFunction::unaryAggregate<CountState,uint8_t,uint64_t,CountOperation>(name, {type}, LogicalTypeId::UBIGINT);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::USMALLINT:{
                auto func = AggregateFunction::unaryAggregate<CountState,uint16_t,uint64_t,CountOperation>(name, {type}, LogicalTypeId::UBIGINT);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::UINTEGER:{
                auto func = AggregateFunction::unaryAggregate<CountState,uint32_t,uint64_t,CountOperation>(name, {type}, LogicalTypeId::UBIGINT);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::HASH:
            case LogicalTypeId::UBIGINT:{
                auto func = AggregateFunction::unaryAggregate<CountState,uint64_t,uint64_t,CountOperation>(name, {type}, LogicalTypeId::UBIGINT);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::FLOAT:{
                auto func = AggregateFunction::unaryAggregate<CountState,float,uint64_t,CountOperation>(name, {type}, LogicalTypeId::UBIGINT);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::DOUBLE:{
                auto func = AggregateFunction::unaryAggregate<CountState,double,uint64_t,CountOperation>(name, {type}, LogicalTypeId::UBIGINT);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::STRING:	{
                auto func = AggregateFunction::unaryAggregate<CountState,string_t,uint64_t,CountOperation>(name, {type}, LogicalTypeId::UBIGINT);
                return function_ptr_t(new AggregateFunction(func));
            }

            default:
                ErrorHandler::errorNotImplemented("Unimplemented type for min operation!");
        }
        return nullptr;
    }

    // register the functions
    static void registerFunction(FunctionRegister& funcRegister) {
        vector<LogicalType> supportedTypes = {LogicalTypeId::TINYINT, LogicalTypeId::SMALLINT,
            LogicalTypeId::INTEGER, LogicalTypeId::BIGINT, LogicalTypeId::UTINYINT, LogicalTypeId::USMALLINT, LogicalTypeId::UINTEGER,
            LogicalTypeId::UBIGINT, LogicalTypeId::HASH, LogicalTypeId::DOUBLE, LogicalTypeId::FLOAT, LogicalTypeId::STRING};
        for (auto& c: supportedTypes) {
            funcRegister.registerFunction(getFunction(c));
        }
    }
};

}
