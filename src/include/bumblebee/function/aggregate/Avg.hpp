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

#include "bumblebee/common/NumericUtils.hpp"
#include "bumblebee/function/AggregateFunction.hpp"
#include "bumblebee/function/Function.hpp"
#include "bumblebee/function/FunctionRegister.hpp"

namespace bumblebee{


template <class T>
struct AvgState {
    T value_;
    uint64_t count_;

    void initialize() {
        value_ = 0;
        count_ = 0;
    }

    void combine(AvgState<T>* other) {
        value_ += other->value_;
        count_ += other->count_;
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
        state->value_ += (RESULT_TYPE)(*input);
        ++state->count_;
    }

    static void finalize(AvgState<RESULT_TYPE> *state, RESULT_TYPE *result) {
        *result = (RESULT_TYPE)((RESULT_TYPE)state->value_ / (RESULT_TYPE)(state->count_));
    }
};


template <class INPUT_TYPE, class RESULT_TYPE, int SCALE>
struct AvgDecimalOperation : AvgOperation<INPUT_TYPE, RESULT_TYPE> {

    static void finalize(AvgState<RESULT_TYPE> *state, RESULT_TYPE *result) {
        *result = (RESULT_TYPE)((RESULT_TYPE)state->value_ / (RESULT_TYPE)(state->count_));
        *result /= NumericHelper::POWERS_OF_TEN[SCALE];
    }
};

template <int SCALE>
struct AvgDecimalOpBinder {
    template <class I, class R>
    using type = AvgDecimalOperation<I, R, SCALE>;
};

template <class INPUT_TYPE, int SCALE>
function_ptr_t templateDecimalAvgScaleFunction(string& name, const vector<LogicalType>& type) {

    auto func = AggregateFunction::unaryAggregate<AvgState,INPUT_TYPE,double, AvgDecimalOpBinder<SCALE>::template type>(name, type, LogicalTypeId::DOUBLE);
    return function_ptr_t(new AggregateFunction(func));
}

template <class INPUT_TYPE>
function_ptr_t templateDecimalAvgScaleSwitch(string& name, const vector<LogicalType>& type) {
    BB_ASSERT(type.size() == 1 && type[0].type() == LogicalTypeId::DECIMAL);
    auto scale = type[0].getDecimalData().scale_;
    switch (scale) {
        case 0: return templateDecimalAvgScaleFunction<INPUT_TYPE, 0>(name, type);
        case 1: return templateDecimalAvgScaleFunction<INPUT_TYPE, 1>(name, type);
        case 2: return templateDecimalAvgScaleFunction<INPUT_TYPE, 2>(name, type);
        case 3: return templateDecimalAvgScaleFunction<INPUT_TYPE, 3>(name, type);
        case 4: return templateDecimalAvgScaleFunction<INPUT_TYPE, 4>(name, type);
        case 5: return templateDecimalAvgScaleFunction<INPUT_TYPE, 5>(name, type);
        case 6: return templateDecimalAvgScaleFunction<INPUT_TYPE, 6>(name, type);
        case 7: return templateDecimalAvgScaleFunction<INPUT_TYPE, 7>(name, type);
        case 8: return templateDecimalAvgScaleFunction<INPUT_TYPE, 8>(name, type);
        case 9: return templateDecimalAvgScaleFunction<INPUT_TYPE, 9>(name, type);
        case 10: return templateDecimalAvgScaleFunction<INPUT_TYPE, 10>(name, type);
        case 11: return templateDecimalAvgScaleFunction<INPUT_TYPE, 11>(name, type);
        case 12: return templateDecimalAvgScaleFunction<INPUT_TYPE, 12>(name, type);
        case 13: return templateDecimalAvgScaleFunction<INPUT_TYPE, 13>(name, type);
        case 14: return templateDecimalAvgScaleFunction<INPUT_TYPE, 14>(name, type);
        case 15: return templateDecimalAvgScaleFunction<INPUT_TYPE, 15>(name, type);
        case 16: return templateDecimalAvgScaleFunction<INPUT_TYPE, 16>(name, type);
        case 17: return templateDecimalAvgScaleFunction<INPUT_TYPE, 17>(name, type);
        case 18: return templateDecimalAvgScaleFunction<INPUT_TYPE, 18>(name, type);
        default:
            ErrorHandler::errorNotImplemented("Unimplemented DECIMAL with scale > 18 for avg operation!");
    }
    return nullptr;
}

class AvgFunc : public FunctionGenerator {
public:
    string getName() override {
        return "#avg";
    }

    // get the function from the type
    function_ptr_t createFunction(const vector<LogicalType>& type) override {
        BB_ASSERT(type.size() == 1);
        string name = getName();
        switch (type[0].type()) {
            case LogicalTypeId::TINYINT: {
                auto func = AggregateFunction::unaryAggregate<AvgState,int8_t,double,AvgOperation>(name, type, LogicalTypeId::DOUBLE);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::SMALLINT:{
                auto func = AggregateFunction::unaryAggregate<AvgState,int16_t,double,AvgOperation>(name, type, LogicalTypeId::DOUBLE);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::INTEGER:{
                auto func = AggregateFunction::unaryAggregate<AvgState,int32_t,double,AvgOperation>(name, type, LogicalTypeId::DOUBLE);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::DECIMAL: {
                switch (type[0].getPhysicalType()) {
                    case PhysicalType::SMALLINT:
                        return templateDecimalAvgScaleSwitch<int16_t>(name, type);
                    case PhysicalType::INTEGER:
                        return templateDecimalAvgScaleSwitch<int32_t>(name, type);
                    case PhysicalType::BIGINT:
                        return templateDecimalAvgScaleSwitch<int64_t>(name, type);
                    default:
                        ErrorHandler::errorNotImplemented("Unimplemented type for avg operation!");
                }
                break;
            }
            case LogicalTypeId::BIGINT:{
                auto func = AggregateFunction::unaryAggregate<AvgState,int64_t,double,AvgOperation>(name, type, LogicalTypeId::DOUBLE);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::UTINYINT:{
                auto func = AggregateFunction::unaryAggregate<AvgState,uint8_t,double,AvgOperation>(name, type, LogicalTypeId::DOUBLE);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::USMALLINT:{
                auto func = AggregateFunction::unaryAggregate<AvgState,uint16_t,double,AvgOperation>(name, type, LogicalTypeId::DOUBLE);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::UINTEGER:{
                auto func = AggregateFunction::unaryAggregate<AvgState,uint32_t,double,AvgOperation>(name, type, LogicalTypeId::DOUBLE);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::HASH:
            case LogicalTypeId::UBIGINT:{
                auto func = AggregateFunction::unaryAggregate<AvgState,uint64_t,double,AvgOperation>(name, type, LogicalTypeId::DOUBLE);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::FLOAT:{
                auto func = AggregateFunction::unaryAggregate<AvgState,float,double,AvgOperation>(name, type, LogicalTypeId::DOUBLE);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::DOUBLE:{
                auto func = AggregateFunction::unaryAggregate<AvgState,double,double,AvgOperation>(name, type, LogicalTypeId::DOUBLE);
                return function_ptr_t(new AggregateFunction(func));
            }
            case LogicalTypeId::STRING:	{
                ErrorHandler::errorNotImplemented("Aggregate string avg not supported");
            }

            default:
                ErrorHandler::errorNotImplemented("Unimplemented type for avg operation!");
        }
        return nullptr;
    }

    static void registerFunction(FunctionRegister& funcRegister) {
        std::unique_ptr<FunctionGenerator> fg = std::make_unique<AvgFunc>();
        funcRegister.registerFunctionGen(fg);
    }
};

}
