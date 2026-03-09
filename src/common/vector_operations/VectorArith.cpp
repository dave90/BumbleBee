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

#include "CLI11.hpp"
#include "bumblebee/common/Helper.hpp"
#include "bumblebee/common/Limits.hpp"
#include "bumblebee/common/NumericUtils.hpp"
#include "bumblebee/common/operator/ArithOperators.hpp"
#include "bumblebee/common/vector_operations/BinaryExecution.hpp"
#include "bumblebee/common/vector_operations/UnaryExecution.hpp"
#include "bumblebee/common/vector_operations/VectorOperations.hpp"

namespace bumblebee {

struct ArithNegate {
    template<class INPUT_TYPE, class RESULT_TYPE>
    static inline RESULT_TYPE operation(INPUT_TYPE value) {
        return (RESULT_TYPE)value * -1;
    }
};

template<class INPUT_TYPE>
void templatedExecuteNegateSwitchResult(Vector &input, Vector &result, idx_t count) {
    switch (result.getType()) {
        case PhysicalType::TINYINT:
            UnaryExecution::execute<INPUT_TYPE, int8_t, ArithNegate>(input, result, count);
            break;
        case PhysicalType::SMALLINT:
            UnaryExecution::execute<INPUT_TYPE, int16_t, ArithNegate>(input, result, count);
            break;
        case PhysicalType::INTEGER:
            UnaryExecution::execute<INPUT_TYPE, int32_t, ArithNegate>(input, result, count);
            break;
        case PhysicalType::BIGINT:
            UnaryExecution::execute<INPUT_TYPE, int64_t, ArithNegate>(input, result, count);
            break;
        case PhysicalType::FLOAT:
            UnaryExecution::execute<INPUT_TYPE, float, ArithNegate>(input, result, count);
            break;
        case PhysicalType::DOUBLE:
            UnaryExecution::execute<INPUT_TYPE, double, ArithNegate>(input, result, count);
            break;
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for execute operation!");
    }
}

template <class OP>
void templatedExecuteOperationSwitchEqualType(Vector &left, Vector &right,Vector &result, idx_t count) {
    BB_ASSERT(left.getLogicalType() == right.getLogicalType() && left.getLogicalType() == result.getLogicalType());
    switch (left.getLogicalTypeId()) {
        case LogicalTypeId::TINYINT:
            BinaryExecution::execute<int8_t,int8_t,int8_t,OP>(left, right, result, count);
            break;
        case LogicalTypeId::SMALLINT:
            BinaryExecution::execute<int16_t,int16_t,int16_t,OP>(left, right, result, count);
            break;
        case LogicalTypeId::DATE:
        case LogicalTypeId::INTEGER:
            BinaryExecution::execute<int32_t,int32_t,int32_t,OP>(left, right, result, count);
            break;
        case LogicalTypeId::TIMESTAMP:
        case LogicalTypeId::DECIMAL:
        case LogicalTypeId::BIGINT:
            BinaryExecution::execute<int64_t,int64_t,int64_t,OP>(left, right, result, count);
            break;
        case LogicalTypeId::UTINYINT:
            BinaryExecution::execute<uint8_t,uint8_t,uint8_t,OP>(left, right, result, count);
            break;
        case LogicalTypeId::USMALLINT:
            BinaryExecution::execute<uint16_t,uint16_t,uint16_t,OP>(left, right, result, count);
            break;
        case LogicalTypeId::UINTEGER:
            BinaryExecution::execute<uint32_t,uint32_t,uint32_t,OP>(left, right, result, count);
            break;
        case LogicalTypeId::HASH:
        case LogicalTypeId::ADDRESS:
        case LogicalTypeId::UBIGINT:
            BinaryExecution::execute<uint64_t,uint64_t,uint64_t,OP>(left, right, result, count);
            break;
        case LogicalTypeId::FLOAT:
            BinaryExecution::execute<float,float,float,OP>(left, right, result, count);
            break;
        case LogicalTypeId::DOUBLE:
            BinaryExecution::execute<double,double,double,OP>(left, right, result, count);
            break;
        case LogicalTypeId::STRING:	{
            BinaryExecution::execute<string_t,string_t,string_t,OP>(left, right, result, count);
            break;
        }
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for execute operation!");
    }
}



template<class LEFT_TYPE, class RIGHT_TYPE, class RESULT_TYPE, class OP>
struct ArithCommonCast {
    static inline RESULT_TYPE operation(LEFT_TYPE left, RIGHT_TYPE right) {
        return OP::operation(static_cast<RESULT_TYPE>(left), static_cast<RESULT_TYPE>(right));
    }
};

struct DecimalCommonCastInput {
    DecimalCommonCastInput(int leftScale, int rightScale, int resultScale)
        : leftScale_(leftScale),
          rightScale_(rightScale),
          resultScale_(resultScale) {
    }

    int leftScale_{0};
    int rightScale_{0};
    int resultScale_{0};
};

struct MixedDecimalFloatInput {
    explicit MixedDecimalFloatInput(int decimalScale) : decimalScale_(decimalScale) {}
    int decimalScale_{0};
};

template<class LEFT_DECIMAL_INT, class RIGHT_FLOAT, class RESULT_FLOAT, class OP>
struct DecimalFloatCast {
    static inline RESULT_FLOAT operation(LEFT_DECIMAL_INT left, RIGHT_FLOAT right, idx_t, void* dataptr) {
        auto input = (MixedDecimalFloatInput*)dataptr;
        auto leftAsFloat = static_cast<RESULT_FLOAT>(left) /
                           static_cast<RESULT_FLOAT>(NumericHelper::POWERS_OF_TEN[input->decimalScale_]);
        return OP::operation(leftAsFloat, static_cast<RESULT_FLOAT>(right));
    }
};

template<class LEFT_FLOAT, class RIGHT_DECIMAL_INT, class RESULT_FLOAT, class OP>
struct FloatDecimalCast {
    static inline RESULT_FLOAT operation(LEFT_FLOAT left, RIGHT_DECIMAL_INT right, idx_t, void* dataptr) {
        auto input = (MixedDecimalFloatInput*)dataptr;
        auto rightAsFloat = static_cast<RESULT_FLOAT>(right) /
                            static_cast<RESULT_FLOAT>(NumericHelper::POWERS_OF_TEN[input->decimalScale_]);
        return OP::operation(static_cast<RESULT_FLOAT>(left), rightAsFloat);
    }
};

template<class LEFT_TYPE, class RIGHT_TYPE, class RESULT_TYPE, class OP>
struct DecimalCommonCast {
    static inline RESULT_TYPE operation(LEFT_TYPE left, RIGHT_TYPE right, idx_t i, void* dataptr) {
        auto input = (DecimalCommonCastInput*)dataptr;
        auto leftResultType = static_cast<RESULT_TYPE>(left);
        auto rightResultType = static_cast<RESULT_TYPE>(right);

        // If leftScale < resultScale, multiply by 10^(resultScale-leftScale), otherwise divide it
        if (input->resultScale_ >= input->leftScale_)
            leftResultType *= NumericHelper::POWERS_OF_TEN[input->resultScale_ - input->leftScale_];
        else
            leftResultType /= NumericHelper::POWERS_OF_TEN[input->leftScale_ - input->resultScale_];

        if (input->resultScale_ >= input->rightScale_)
            rightResultType *= NumericHelper::POWERS_OF_TEN[input->resultScale_ - input->rightScale_];
        else
            rightResultType /= NumericHelper::POWERS_OF_TEN[input->rightScale_ - input->resultScale_];

        return OP::operation(leftResultType, rightResultType);
    }
};

// Decimal cast for the multiplication (avoids scaling cast)
template<class LEFT_TYPE, class RIGHT_TYPE, class RESULT_TYPE>
struct DecimalCommonCast<LEFT_TYPE, RIGHT_TYPE, RESULT_TYPE, Dot> {
    static inline RESULT_TYPE operation(LEFT_TYPE left, RIGHT_TYPE right, idx_t idx, void* dataptr) {
        auto in = (DecimalCommonCastInput*)dataptr;

        // raw integer multiply in wider type to avoid overflow
        // TODO in widnows could be overflow

#if defined(_MSC_VER) && !defined(__clang__)
        // MSVC: no __int128 in standard C++; just do 64-bit multiply (may overflow, per your choice)
        int64_t raw = static_cast<int64_t>(left) * static_cast<int64_t>(right);
        int rawScale = in->leftScale_ + in->rightScale_;
        int target = in->resultScale_;

        if (target > rawScale) {
            raw *= NumericHelper::POWERS_OF_TEN[target - rawScale];
        } else if (target < rawScale) {
            // truncation; optionally implement rounding here
            raw /= NumericHelper::POWERS_OF_TEN[rawScale - target];
        }
        return (RESULT_TYPE)raw;
#else
        // GCC/Clang: use 128-bit intermediate to reduce overflow risk
        __int128 raw = static_cast<__int128>(left) * static_cast<__int128>(right);

        int rawScale = in->leftScale_ + in->rightScale_;
        int target = in->resultScale_;

        if (target > rawScale) {
            raw *= NumericHelper::POWERS_OF_TEN[target - rawScale];
        } else if (target < rawScale) {
            // truncation; optionally implement rounding here
            raw /= NumericHelper::POWERS_OF_TEN[rawScale - target];
        }
        return (RESULT_TYPE)raw;
#endif
    }
};



// Decimal cast for the division (fixed-point aware)
template<class LEFT_TYPE, class RIGHT_TYPE, class RESULT_TYPE>
struct DecimalCommonCast<LEFT_TYPE, RIGHT_TYPE, RESULT_TYPE, Division> {
    static inline RESULT_TYPE operation(LEFT_TYPE left, RIGHT_TYPE right, idx_t idx, void* dataptr) {
        auto in = (DecimalCommonCastInput*)dataptr;

        if (right == 0) {
            // division by zero set result as max value
            return NumericLimits<RESULT_TYPE>::maximum();
        }

        // We want: result_scaled = (left * 10^(resultScale + rightScale - leftScale)) / right
        const int exp = in->resultScale_ + in->rightScale_ - in->leftScale_;

#if defined(_MSC_VER) && !defined(__clang__)
        // MSVC: no __int128 (overflow possible, per your choice)
        int64_t num = static_cast<int64_t>(left);
        int64_t den = static_cast<int64_t>(right);

        if (exp >= 0) {
            num *= NumericHelper::POWERS_OF_TEN[exp];
        } else {
            // If exp < 0, we would need to divide numerator before division to avoid negative power.
            // This loses precision, but is consistent with truncating integer arithmetic.
            num /= NumericHelper::POWERS_OF_TEN[-exp];
        }

        int64_t raw = num / den; // truncation toward zero
        return static_cast<RESULT_TYPE>(raw);
#else
        // GCC/Clang: use 128-bit intermediate
        __int128 num = static_cast<__int128>(left);
        __int128 den = static_cast<__int128>(right);

        if (exp >= 0) {
            num *= NumericHelper::POWERS_OF_TEN[exp];
        } else {
            num /= NumericHelper::POWERS_OF_TEN[-exp];
        }

        __int128 raw = num / den; // truncation toward zero
        return (RESULT_TYPE)(raw);
#endif
    }
};




template <class LEFT_TYPE, class RIGHT_TYPE,  class RESULT_TYPE, class OP>
void templatedExecuteOperationSwitchDecimalMaxScale(Vector &left, Vector &right, Vector &result, idx_t count) {
    BB_ASSERT(left.getLogicalTypeId() == LogicalTypeId::DECIMAL);
    BB_ASSERT(right.getLogicalTypeId() == LogicalTypeId::DECIMAL);
    int sl = left.getLogicalType().getDecimalData().scale_;
    int sr = right.getLogicalType().getDecimalData().scale_;
    int sres = result.getLogicalType().getDecimalData().scale_;
    DecimalCommonCastInput input(sl, sr, sres);

    BinaryExecution::genericExecute<LEFT_TYPE, RIGHT_TYPE, RESULT_TYPE, DecimalCommonCast<LEFT_TYPE, RIGHT_TYPE, RESULT_TYPE, OP>>(left, right, result, count, &input);
}


template<class LEFT_DECIMAL_INT, class OP>
void templatedExecuteOperationDecimalFloat(Vector& left, Vector& right, Vector& result, idx_t count) {
    BB_ASSERT(left.getLogicalTypeId() == LogicalTypeId::DECIMAL);
    BB_ASSERT(result.getLogicalTypeId() == LogicalTypeId::DOUBLE);
    int scale = left.getLogicalType().getDecimalData().scale_;
    MixedDecimalFloatInput input(scale);
    switch (right.getLogicalTypeId()) {
        case LogicalTypeId::FLOAT:
            BinaryExecution::genericExecute<LEFT_DECIMAL_INT, float, double,
                DecimalFloatCast<LEFT_DECIMAL_INT, float, double, OP>>(left, right, result, count, &input);
            break;
        case LogicalTypeId::DOUBLE:
            BinaryExecution::genericExecute<LEFT_DECIMAL_INT, double, double,
                DecimalFloatCast<LEFT_DECIMAL_INT, double, double, OP>>(left, right, result, count, &input);
            break;
        default:
            ErrorHandler::errorNotImplemented("Unimplemented right type for decimal-float operation!");
    }
}

template<class RIGHT_DECIMAL_INT, class OP>
void templatedExecuteOperationFloatDecimal(Vector& left, Vector& right, Vector& result, idx_t count) {
    BB_ASSERT(right.getLogicalTypeId() == LogicalTypeId::DECIMAL);
    BB_ASSERT(result.getLogicalTypeId() == LogicalTypeId::DOUBLE);
    int scale = right.getLogicalType().getDecimalData().scale_;
    MixedDecimalFloatInput input(scale);
    switch (left.getLogicalTypeId()) {
        case LogicalTypeId::FLOAT:
            BinaryExecution::genericExecute<float, RIGHT_DECIMAL_INT, double,
                FloatDecimalCast<float, RIGHT_DECIMAL_INT, double, OP>>(left, right, result, count, &input);
            break;
        case LogicalTypeId::DOUBLE:
            BinaryExecution::genericExecute<double, RIGHT_DECIMAL_INT, double,
                FloatDecimalCast<double, RIGHT_DECIMAL_INT, double, OP>>(left, right, result, count, &input);
            break;
        default:
            ErrorHandler::errorNotImplemented("Unimplemented left type for float-decimal operation!");
    }
}

template <class LEFT_TYPE, class RIGHT_TYPE,  class OP>
void templatedExecuteOperationSwitchResult(Vector &left, Vector &right, Vector &result, idx_t count) {
    switch (result.getLogicalTypeId()) {
        case LogicalTypeId::TINYINT:
            BinaryExecution::execute<LEFT_TYPE,RIGHT_TYPE, int8_t, ArithCommonCast<LEFT_TYPE, RIGHT_TYPE, int8_t, OP>>(left, right, result, count);
            break;
        case LogicalTypeId::SMALLINT:
            BinaryExecution::execute<LEFT_TYPE,RIGHT_TYPE, int16_t, ArithCommonCast<LEFT_TYPE, RIGHT_TYPE, int16_t, OP>>(left, right, result, count);
            break;
        case LogicalTypeId::DATE:
        case LogicalTypeId::INTEGER:
            BinaryExecution::execute<LEFT_TYPE,RIGHT_TYPE, int32_t, ArithCommonCast<LEFT_TYPE, RIGHT_TYPE, int32_t, OP>>(left, right, result, count);
            break;
        case LogicalTypeId::UTINYINT:
            BinaryExecution::execute<LEFT_TYPE,RIGHT_TYPE, uint8_t, ArithCommonCast<LEFT_TYPE, RIGHT_TYPE, uint8_t, OP>>(left, right, result, count);
            break;
        case LogicalTypeId::USMALLINT:
            BinaryExecution::execute<LEFT_TYPE,RIGHT_TYPE, uint16_t, ArithCommonCast<LEFT_TYPE, RIGHT_TYPE, uint16_t, OP>>(left, right, result, count);
            break;
        case LogicalTypeId::UINTEGER:
            BinaryExecution::execute<LEFT_TYPE,RIGHT_TYPE, uint32_t, ArithCommonCast<LEFT_TYPE, RIGHT_TYPE, uint32_t, OP>>(left, right, result, count);
            break;
        case LogicalTypeId::ADDRESS:
        case LogicalTypeId::HASH:
        case LogicalTypeId::UBIGINT:
            BinaryExecution::execute<LEFT_TYPE,RIGHT_TYPE, uint64_t, ArithCommonCast<LEFT_TYPE, RIGHT_TYPE, uint64_t, OP>>(left, right, result, count);
            break;
        case LogicalTypeId::TIMESTAMP:
        case LogicalTypeId::BIGINT:
            BinaryExecution::execute<LEFT_TYPE,RIGHT_TYPE, int64_t, ArithCommonCast<LEFT_TYPE, RIGHT_TYPE, int64_t, OP>>(left, right, result, count);
            break;
        case LogicalTypeId::FLOAT:
            BinaryExecution::execute<LEFT_TYPE,RIGHT_TYPE, float, ArithCommonCast<LEFT_TYPE, RIGHT_TYPE, float, OP>>(left, right, result, count);
            break;
        case LogicalTypeId::DOUBLE:
            BinaryExecution::execute<LEFT_TYPE,RIGHT_TYPE, double, ArithCommonCast<LEFT_TYPE, RIGHT_TYPE, double, OP>>(left, right, result, count);
            break;
        case LogicalTypeId::DECIMAL: {
            switch (result.getType()) {
                case PhysicalType::SMALLINT:
                    templatedExecuteOperationSwitchDecimalMaxScale<LEFT_TYPE, RIGHT_TYPE, int16_t, OP>(left, right, result, count);
                    break;
                case PhysicalType::INTEGER:
                    templatedExecuteOperationSwitchDecimalMaxScale<LEFT_TYPE, RIGHT_TYPE, int32_t, OP>(left, right, result, count);
                    break;
                case PhysicalType::BIGINT:
                    templatedExecuteOperationSwitchDecimalMaxScale<LEFT_TYPE, RIGHT_TYPE, int64_t, OP>(left, right, result, count);
                    break;
                default:
                    ErrorHandler::errorNotImplemented("Unimplemented type for execute operation!");
            }
            break;
        }
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for execute operation!");
    }
}

template <class LEFT_TYPE,  class OP>
void templatedExecuteOperationSwitchRight(Vector &left, Vector &right, Vector &result, idx_t count) {
    switch (right.getLogicalTypeId()) {
        case LogicalTypeId::TINYINT:
            templatedExecuteOperationSwitchResult<LEFT_TYPE,int8_t, OP>(left, right, result, count);
            break;
        case LogicalTypeId::SMALLINT:
            templatedExecuteOperationSwitchResult<LEFT_TYPE,int16_t, OP>(left, right, result, count);
            break;
        case LogicalTypeId::INTEGER:
            templatedExecuteOperationSwitchResult<LEFT_TYPE,int32_t, OP>(left, right, result, count);
            break;
        case LogicalTypeId::UTINYINT:
            templatedExecuteOperationSwitchResult<LEFT_TYPE,uint8_t, OP>(left, right, result, count);
            break;
        case LogicalTypeId::USMALLINT:
            templatedExecuteOperationSwitchResult<LEFT_TYPE,uint16_t, OP>(left, right, result, count);
            break;
        case LogicalTypeId::UINTEGER:
            templatedExecuteOperationSwitchResult<LEFT_TYPE,uint32_t, OP>(left, right, result, count);
            break;
        case LogicalTypeId::HASH:
        case LogicalTypeId::ADDRESS:
        case LogicalTypeId::UBIGINT:
            templatedExecuteOperationSwitchResult<LEFT_TYPE,uint64_t, OP>(left, right, result, count);
            break;
        case LogicalTypeId::BIGINT:
            templatedExecuteOperationSwitchResult<LEFT_TYPE,int64_t, OP>(left, right, result, count);
            break;
        case LogicalTypeId::FLOAT:
            templatedExecuteOperationSwitchResult<LEFT_TYPE,float, OP>(left, right, result, count);
            break;
        case LogicalTypeId::DOUBLE:
            templatedExecuteOperationSwitchResult<LEFT_TYPE,double, OP>(left, right, result, count);
            break;
        case LogicalTypeId::DECIMAL: {
            const bool leftIsFloat = left.getLogicalTypeId() == LogicalTypeId::FLOAT
                                  || left.getLogicalTypeId() == LogicalTypeId::DOUBLE;
            switch (right.getType()) {
                case PhysicalType::SMALLINT:
                    leftIsFloat ? templatedExecuteOperationFloatDecimal<int16_t, OP>(left, right, result, count)
                                : templatedExecuteOperationSwitchResult<LEFT_TYPE, int16_t, OP>(left, right, result, count);
                    break;
                case PhysicalType::INTEGER:
                    leftIsFloat ? templatedExecuteOperationFloatDecimal<int32_t, OP>(left, right, result, count)
                                : templatedExecuteOperationSwitchResult<LEFT_TYPE, int32_t, OP>(left, right, result, count);
                    break;
                case PhysicalType::BIGINT:
                    leftIsFloat ? templatedExecuteOperationFloatDecimal<int64_t, OP>(left, right, result, count)
                                : templatedExecuteOperationSwitchResult<LEFT_TYPE, int64_t, OP>(left, right, result, count);
                    break;
                default:
                    ErrorHandler::errorNotImplemented("Unimplemented type for execute operation!");
            }
            break;
        }
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for execute operation!");
    }
}

template <class OP>
void templatedExecuteOperationSwitchLeft(Vector &left, Vector &right, Vector &result, idx_t count) {
    switch (left.getLogicalTypeId()) {
        case LogicalTypeId::TINYINT:
            templatedExecuteOperationSwitchRight<int8_t, OP>(left, right, result, count);
            break;
        case LogicalTypeId::SMALLINT:
            templatedExecuteOperationSwitchRight<int16_t, OP>(left, right, result, count);
            break;
        case LogicalTypeId::INTEGER:
            templatedExecuteOperationSwitchRight<int32_t, OP>(left, right, result, count);
            break;
        case LogicalTypeId::UTINYINT:
            templatedExecuteOperationSwitchRight<uint8_t, OP>(left, right, result, count);
            break;
        case LogicalTypeId::USMALLINT:
            templatedExecuteOperationSwitchRight<uint16_t, OP>(left, right, result, count);
            break;
        case LogicalTypeId::UINTEGER:
            templatedExecuteOperationSwitchRight<uint32_t, OP>(left, right, result, count);
            break;
        case LogicalTypeId::HASH:
        case LogicalTypeId::ADDRESS:
        case LogicalTypeId::UBIGINT:
            templatedExecuteOperationSwitchRight<uint64_t, OP>(left, right, result, count);
            break;
        case LogicalTypeId::BIGINT:
            templatedExecuteOperationSwitchRight<int64_t, OP>(left, right, result, count);
            break;
        case LogicalTypeId::FLOAT:
            templatedExecuteOperationSwitchRight<float, OP>(left, right, result, count);
            break;
        case LogicalTypeId::DOUBLE:
            templatedExecuteOperationSwitchRight<double, OP>(left, right, result, count);
            break;
        case LogicalTypeId::DECIMAL: {
            const bool rightIsFloat = right.getLogicalTypeId() == LogicalTypeId::FLOAT
                                   || right.getLogicalTypeId() == LogicalTypeId::DOUBLE;
            switch (left.getType()) {
                case PhysicalType::SMALLINT:
                    rightIsFloat ? templatedExecuteOperationDecimalFloat<int16_t, OP>(left, right, result, count)
                                 : templatedExecuteOperationSwitchRight<int16_t, OP>(left, right, result, count);
                    break;
                case PhysicalType::INTEGER:
                    rightIsFloat ? templatedExecuteOperationDecimalFloat<int32_t, OP>(left, right, result, count)
                                 : templatedExecuteOperationSwitchRight<int32_t, OP>(left, right, result, count);
                    break;
                case PhysicalType::BIGINT:
                    rightIsFloat ? templatedExecuteOperationDecimalFloat<int64_t, OP>(left, right, result, count)
                                 : templatedExecuteOperationSwitchRight<int64_t, OP>(left, right, result, count);
                    break;
                default:
                    ErrorHandler::errorNotImplemented("Unimplemented type for execute operation!");
            }
            break;
        }
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for execute operation!");
    }
}

void VectorOperations::sum(Vector &left, Vector &right, Vector &result, idx_t count) {
    if (left.getLogicalType() == right.getLogicalType() && left.getLogicalType() == result.getLogicalType())
        templatedExecuteOperationSwitchEqualType<Sum>(left, right, result, count);
    else
        templatedExecuteOperationSwitchLeft<Sum>(left, right, result, count);
}

void VectorOperations::dot(Vector &left, Vector &right, Vector &result, idx_t count) {
    if (left.getLogicalType() == right.getLogicalType() && left.getLogicalType() == result.getLogicalType())
        templatedExecuteOperationSwitchEqualType<Dot>(left, right, result, count);
    else
        templatedExecuteOperationSwitchLeft<Dot>(left, right, result, count);

}

void VectorOperations::division(Vector &left, Vector &right, Vector &result, idx_t count) {
    if (left.getLogicalType() == right.getLogicalType() && left.getLogicalType() == result.getLogicalType())
        templatedExecuteOperationSwitchEqualType<Division>(left, right, result, count);
    else
        templatedExecuteOperationSwitchLeft<Division>(left, right, result, count);

}

void VectorOperations::difference(Vector &left, Vector &right, Vector &result, idx_t count) {
    if (left.getLogicalType() == right.getLogicalType() && left.getLogicalType() == result.getLogicalType())
        templatedExecuteOperationSwitchEqualType<Difference>(left, right, result, count);
    else
        templatedExecuteOperationSwitchLeft<Difference>(left, right, result, count);
}

void VectorOperations::negate(Vector &input, Vector &result, idx_t count) {
    switch (input.getType()) {
        case PhysicalType::TINYINT:
            templatedExecuteNegateSwitchResult<int8_t>(input, result, count);
            break;
        case PhysicalType::SMALLINT:
            templatedExecuteNegateSwitchResult<int16_t>(input, result, count);
            break;
        case PhysicalType::INTEGER:
            templatedExecuteNegateSwitchResult<int32_t>(input, result, count);
            break;
        case PhysicalType::UTINYINT:
            templatedExecuteNegateSwitchResult<uint8_t>(input, result, count);
            break;
        case PhysicalType::USMALLINT:
            templatedExecuteNegateSwitchResult<uint16_t>(input, result, count);
            break;
        case PhysicalType::UINTEGER:
            templatedExecuteNegateSwitchResult<uint32_t>(input, result, count);
            break;
        case PhysicalType::UBIGINT:
            templatedExecuteNegateSwitchResult<uint64_t>(input, result, count);
            break;
        case PhysicalType::BIGINT:
            templatedExecuteNegateSwitchResult<int64_t>(input, result, count);
            break;
        case PhysicalType::FLOAT:
            templatedExecuteNegateSwitchResult<float>(input, result, count);
            break;
        case PhysicalType::DOUBLE:
            templatedExecuteNegateSwitchResult<double>(input, result, count);
            break;
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for execute operation!");
    }
}

void VectorOperations::modulo(Vector &left, Vector &right, Vector &result, idx_t count) {
    if (left.getLogicalType() == right.getLogicalType() && left.getLogicalType() == result.getLogicalType())
        templatedExecuteOperationSwitchEqualType<Modulo>(left, right, result, count);
    else
        templatedExecuteOperationSwitchLeft<Modulo>(left, right, result, count);
}

void VectorOperations::lAnd(Vector &left, Vector &right, Vector &result, idx_t count) {
    if (left.getLogicalType() == right.getLogicalType() && left.getLogicalType() == result.getLogicalType())
        templatedExecuteOperationSwitchEqualType<And>(left, right, result, count);
    else
        templatedExecuteOperationSwitchLeft<And>(left, right, result, count);
}


}