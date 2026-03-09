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

#include "bumblebee/common/types/Decimal.hpp"
#include "bumblebee/function/AggregateFunction.hpp"
#include "bumblebee/function/Function.hpp"
#include "bumblebee/function/FunctionRegister.hpp"

namespace bumblebee{


template <class T>
struct MinState {
    T value;
    bool init;

    void initialize() {
        init = false;
    }

    void combine(MinState<T>* other) {
        value = (other->init && ( !init || value > other->value))?other->value: value;
        init = true;
    }
};

template <class INPUT_TYPE, class RESULT_TYPE>
struct MinOperation {

    static void initialize(MinState<RESULT_TYPE> *state) {
        state->initialize();
    }

    static void combine(MinState<RESULT_TYPE>* source, MinState<RESULT_TYPE> *target) {
        target->combine(source);
    }

    static void operation(INPUT_TYPE *input, MinState<RESULT_TYPE> *state) {
        state->value = (!state->init || state->value > (RESULT_TYPE)(*input))? ((RESULT_TYPE)(*input)): state->value;
        state->init = true;
    }

    static void finalize(MinState<RESULT_TYPE> *state, RESULT_TYPE *result) {
        *result = state->value;
    }
};

class MinFunc : public FunctionGenerator {
public:
    string getName() override {
        return "#min";
    }

    // get the function from the type
    function_ptr_t createFunction(const vector<LogicalType>& type) override {
        BB_ASSERT(type.size() == 1);
        string name = getName();
        switch (type[0].type()) {
            case LogicalTypeId::TINYINT: {
                auto func = AggregateFunction::unaryAggregate<MinState,int8_t,int64_t,MinOperation>(name, type,LogicalTypeId:: BIGINT);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::SMALLINT:{
                auto func = AggregateFunction::unaryAggregate<MinState,int16_t,int64_t,MinOperation>(name, type,LogicalTypeId:: BIGINT);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::INTEGER:{
                auto func = AggregateFunction::unaryAggregate<MinState,int32_t,int64_t,MinOperation>(name, type,LogicalTypeId:: BIGINT);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::DECIMAL:{
                auto func = AggregateFunction::unaryAggregate<MinState,int64_t,int64_t,MinOperation>(name, type, LogicalType::createDecimal(Decimal::MAX_WIDTH_INT64, type[0].getDecimalData().scale_));
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::BIGINT:{
                auto func = AggregateFunction::unaryAggregate<MinState,int64_t,int64_t,MinOperation>(name, type,LogicalTypeId:: BIGINT);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::UTINYINT:{
                auto func = AggregateFunction::unaryAggregate<MinState,uint8_t,uint64_t,MinOperation>(name, type, LogicalTypeId::UBIGINT);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::USMALLINT:{
                auto func = AggregateFunction::unaryAggregate<MinState,uint16_t,uint64_t,MinOperation>(name, type, LogicalTypeId::UBIGINT);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::UINTEGER:{
                auto func = AggregateFunction::unaryAggregate<MinState,uint32_t,uint64_t,MinOperation>(name, type, LogicalTypeId::UBIGINT);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::HASH:
            case LogicalTypeId::UBIGINT:{
                auto func = AggregateFunction::unaryAggregate<MinState,uint64_t,uint64_t,MinOperation>(name, type, LogicalTypeId::UBIGINT);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::FLOAT:{
                auto func = AggregateFunction::unaryAggregate<MinState,float,float,MinOperation>(name, type, LogicalTypeId::FLOAT);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::DOUBLE:{
                auto func = AggregateFunction::unaryAggregate<MinState,double,double,MinOperation>(name, type, LogicalTypeId::DOUBLE);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::STRING:	{
                auto func = AggregateFunction::unaryAggregate<MinState,string_t,string_t,MinOperation>(name, type, LogicalTypeId::STRING);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::DATE:	{
                auto func = AggregateFunction::unaryAggregate<MinState,date_t,date_t,MinOperation>(name, type, LogicalTypeId::DATE);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::TIMESTAMP:	{
                auto func = AggregateFunction::unaryAggregate<MinState,timestamp_t,timestamp_t,MinOperation>(name, type, LogicalTypeId::TIMESTAMP);
                return function_ptr_t(new AggregateFunction(func));
            }

            default:
                ErrorHandler::errorNotImplemented("Unimplemented type for min operation!");
        }
        return nullptr;
    }

    // register the functions
    static void registerFunction(FunctionRegister& funcRegister) {
        std::unique_ptr<FunctionGenerator> fg = std::make_unique<MinFunc>();
        funcRegister.registerFunctionGen(fg);
    }
};

}
