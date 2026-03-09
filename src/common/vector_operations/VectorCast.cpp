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

#include "bumblebee/common/operator/CastOperators.hpp"
#include "bumblebee/common/vector_operations/UnaryExecution.hpp"
#include "bumblebee/common/vector_operations/VectorOperations.hpp"
#include "bumblebee/common/types/NullValue.hpp"

namespace bumblebee {


struct VectorTryCastData {
    VectorTryCastData(Vector &result_p, string *error_message_p)
        : result_(result_p), errorMessage_(error_message_p) {
    }

    Vector &result_;
    string *errorMessage_;
    bool allConverted_ = true;
};



struct DecimalCastInput {
    DecimalCastInput(Vector &result_p, int width_p, int scale_p)
        : result_(result_p), width_(width_p), scale_(scale_p) {
    }

    Vector &result_;
    int width_;
    int scale_;
    bool allConverted_ = true;
};

struct HandleVectorCastError {
    template <class RESULT_TYPE>
    static RESULT_TYPE operation(string error_message, string *error_message_ptr,
                                 bool &all_converted) {
        if (error_message_ptr) {
            *error_message_ptr = error_message;
        }
        all_converted = false;
        return NullValue<RESULT_TYPE>();
    }
};

template <class OP>
struct VectorTryCastOperator {
    template <class INPUT_TYPE, class RESULT_TYPE>
    static RESULT_TYPE operation(INPUT_TYPE& input, void *dataptr) {
        RESULT_TYPE output;
        if ( OP::template operation<INPUT_TYPE, RESULT_TYPE>(input, output) ){
            return output;
        }
        auto data = (VectorTryCastData *)dataptr;
        // TODO better error messsage
        return HandleVectorCastError::operation<RESULT_TYPE>("Error during conversion!", data->errorMessage_, data->allConverted_);
    }
};

template <class OP>
struct VectorStringCastOperator {
    template <class INPUT_TYPE, class RESULT_TYPE>
    static RESULT_TYPE operation(INPUT_TYPE input, void *dataptr) {
        auto result = (VectorTryCastData *)dataptr;
        return OP::template operation<INPUT_TYPE>(input, result->result_);
    }
};


struct StringCastFromDecimalOperator {
    template <class INPUT_TYPE, class RESULT_TYPE>
    static RESULT_TYPE operation(INPUT_TYPE input, void *dataptr) {
        auto data = (DecimalCastInput *)dataptr;
        return StringTryCastFromDecimal::operation<INPUT_TYPE>(input, data->width_, data->scale_, data->result_);
    }
};

struct DecimalCastOperator {
    template <class INPUT_TYPE, class RESULT_TYPE>
    static RESULT_TYPE operation(INPUT_TYPE input, void *dataptr) {
        auto data = (DecimalCastInput *)dataptr;
        if (data->scale_ < 0) {
            return (RESULT_TYPE)std::llround((long double)input / (long double)NumericHelper::POWERS_OF_TEN[abs(data->scale_)]);
        } if (data->scale_ > 0) {
            return (RESULT_TYPE)std::llround((long double)input * (long double)NumericHelper::POWERS_OF_TEN[data->scale_]);
        }
        return (RESULT_TYPE)input;
    }
};

// Casts DECIMAL (stored as scaled integer) to a floating-point type by dividing by the scale factor
struct DecimalToFloatCastOp {
    template <class INPUT_TYPE, class RESULT_TYPE>
    static RESULT_TYPE operation(INPUT_TYPE input, void *dataptr) {
        auto data = (DecimalCastInput *)dataptr;
        if (data->scale_ == 0) return static_cast<RESULT_TYPE>(input);
        if (data->scale_ > 0)
            return static_cast<RESULT_TYPE>((long double)input / (long double)NumericHelper::POWERS_OF_TEN[data->scale_]);
        return static_cast<RESULT_TYPE>((long double)input * (long double)NumericHelper::POWERS_OF_TEN[-data->scale_]);
    }
};


template <class SRC, class DST, class OP>
bool vectorCastLoop(Vector &source, Vector &result, idx_t count, string *errorMessage) {
    VectorTryCastData input(result, errorMessage);
    UnaryExecution::genericExecute<SRC, DST, VectorTryCastOperator<OP>>(source, result, count, &input);
    return input.allConverted_;
}

template <class SRC, class DST, class OP>
bool vectorStringCastLoop(Vector &source, Vector &result, idx_t count, string *errorMessage) {
    VectorTryCastData input(result, errorMessage);
    UnaryExecution::genericExecute<SRC, DST, VectorStringCastOperator<OP>>(source, result, count, &input);
    return input.allConverted_;
}

template <class SRC, class DST>
bool vectorDecimalCastLoop(Vector &source, Vector &result, idx_t count, string *errorMessage) {
    int sourceScale = (source.getLogicalTypeId() == LogicalTypeId::DECIMAL)
                                ?source.getLogicalType().getDecimalData().scale_
                                :0;
    int scale = result.getLogicalType().getDecimalData().scale_ - sourceScale;
    DecimalCastInput input(result, result.getLogicalType().getDecimalData().width_, scale);
    UnaryExecution::genericExecute<SRC, DST, DecimalCastOperator>(source, result, count, &input);
    return input.allConverted_;
}


template <class SRC>
bool numericCastSwitch(Vector &source, Vector &result, idx_t count, string *errorMessage) {

    switch (result.getLogicalTypeId()) {
        case LogicalTypeId::TINYINT:
            return vectorCastLoop<SRC, int8_t, NumericTryCast>(source, result, count, errorMessage);
        case LogicalTypeId::SMALLINT:
            return vectorCastLoop<SRC, int16_t, NumericTryCast>(source, result, count, errorMessage);
        case LogicalTypeId::INTEGER:
            return vectorCastLoop<SRC, int32_t, NumericTryCast>(source, result, count, errorMessage);
        case LogicalTypeId::UTINYINT:
            return vectorCastLoop<SRC, uint8_t, NumericTryCast>(source, result, count, errorMessage);
        case LogicalTypeId::USMALLINT:
            return vectorCastLoop<SRC, uint16_t, NumericTryCast>(source, result, count, errorMessage);
        case LogicalTypeId::UINTEGER:
            return vectorCastLoop<SRC, uint32_t, NumericTryCast>(source, result, count, errorMessage);
        case LogicalTypeId::UBIGINT:
            return vectorCastLoop<SRC, uint64_t, NumericTryCast>(source, result, count, errorMessage);
        case LogicalTypeId::BIGINT:
            return vectorCastLoop<SRC, int64_t, NumericTryCast>(source, result, count, errorMessage);
        case LogicalTypeId::FLOAT:
            return vectorCastLoop<SRC, float, NumericTryCast>(source, result, count, errorMessage);
        case LogicalTypeId::DOUBLE:
            return vectorCastLoop<SRC, double, NumericTryCast>(source, result, count, errorMessage);
        case LogicalTypeId::STRING:
            return vectorStringCastLoop<SRC, string_t, StringTryCast>(source, result, count, errorMessage);
        case LogicalTypeId::DECIMAL: {
            switch (result.getType()) {
                case PhysicalType::SMALLINT:
                    return vectorDecimalCastLoop<SRC, int16_t>(source, result, count, errorMessage);
                case PhysicalType::INTEGER:
                    return vectorDecimalCastLoop<SRC, int32_t>(source, result, count, errorMessage);
                case PhysicalType::BIGINT:
                    return vectorDecimalCastLoop<SRC, int64_t>(source, result, count, errorMessage);
                default:
                    ErrorHandler::errorNotImplemented("Unimplemented type for execute operation!");
            }
        }
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for execute operation!");
        return false;
    }
}



bool stringCastSwitch(Vector &source, Vector &result, idx_t count, string *errorMessage) {

    switch (result.getType()) {
        case PhysicalType::TINYINT:
            return vectorCastLoop<string_t, int8_t, TryIntegerCast>(source, result, count, errorMessage);

        case PhysicalType::SMALLINT:
            return vectorCastLoop<string_t, int16_t, TryIntegerCast>(source, result, count, errorMessage);

        case PhysicalType::INTEGER:
            return vectorCastLoop<string_t, int32_t, TryIntegerCast>(source, result, count, errorMessage);

        case PhysicalType::UTINYINT:
            return vectorCastLoop<string_t, uint8_t, TryIntegerCast>(source, result, count, errorMessage);

        case PhysicalType::USMALLINT:
            return vectorCastLoop<string_t, uint16_t, TryIntegerCast>(source, result, count, errorMessage);

        case PhysicalType::UINTEGER:
            return vectorCastLoop<string_t, uint32_t, TryIntegerCast>(source, result, count, errorMessage);

        case PhysicalType::UBIGINT:
            return vectorCastLoop<string_t, uint64_t, TryIntegerCast>(source, result, count, errorMessage);

        case PhysicalType::BIGINT:
            return vectorCastLoop<string_t, int64_t, TryIntegerCast>(source, result, count, errorMessage);

        case PhysicalType::FLOAT:
            return vectorCastLoop<string_t, float, TryDoubleCast>(source, result, count, errorMessage);

        case PhysicalType::DOUBLE:
            return vectorCastLoop<string_t, double, TryDoubleCast>(source, result, count, errorMessage);

        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for execute operation!");
    }
    return false;
}


template<class INPUT_TYPE>
static bool decimalCastSwitch(Vector &source, Vector &result, idx_t count, string *errorMessage) {
    BB_ASSERT(source.getLogicalTypeId() == LogicalTypeId::DECIMAL);

	// now switch on the result type
	switch (result.getLogicalTypeId()) {
	    case LogicalTypeId::STRING: {
		    auto source_type = source.getLogicalType();
	        auto& decimalData = source.getLogicalType().getDecimalData();
		    DecimalCastInput input(result, decimalData.width_, decimalData.scale_);
	        UnaryExecution::genericExecute<INPUT_TYPE, string_t, StringCastFromDecimalOperator>(source, result, count, (void *)&input);
		    return true;
	    }
	    case LogicalTypeId::DECIMAL: {
	        switch (result.getType()) {
	            case PhysicalType::SMALLINT:
                    return vectorDecimalCastLoop<INPUT_TYPE, int16_t>(source, result, count, errorMessage);
	            case PhysicalType::INTEGER:
                    return vectorDecimalCastLoop<INPUT_TYPE, int32_t>(source, result, count, errorMessage);
	            case PhysicalType::BIGINT:
                    return vectorDecimalCastLoop<INPUT_TYPE, int64_t>(source, result, count, errorMessage);
	            default:
	                ErrorHandler::errorNotImplemented("Unimplemented type for execute operation!");
	        }
	        return false;
	    }
	    case LogicalTypeId::FLOAT: {
	        auto& decimalData = source.getLogicalType().getDecimalData();
	        DecimalCastInput input(result, decimalData.width_, decimalData.scale_);
	        UnaryExecution::genericExecute<INPUT_TYPE, float, DecimalToFloatCastOp>(source, result, count, (void*)&input);
	        return true;
	    }
	    case LogicalTypeId::DOUBLE: {
	        auto& decimalData = source.getLogicalType().getDecimalData();
	        DecimalCastInput input(result, decimalData.width_, decimalData.scale_);
	        UnaryExecution::genericExecute<INPUT_TYPE, double, DecimalToFloatCastOp>(source, result, count, (void*)&input);
	        return true;
	    }
	default:
        ErrorHandler::errorNotImplemented("Unimplemented type for execute operation!");
	}
    return false;
}


struct StringCastFromDateOperator {
    template <class INPUT_TYPE, class RESULT_TYPE>
    static RESULT_TYPE operation(INPUT_TYPE input, void *dataptr) {
        auto data = (VectorTryCastData *)dataptr;
        return StringCastFromDate::operation<INPUT_TYPE>(input, data->result_);
    }
};


static bool dateCastSwitch(Vector &source, Vector &result, idx_t count, string *errorMessage) {
    VectorTryCastData input(result, errorMessage);
    switch (result.getLogicalTypeId()) {
        case LogicalTypeId::STRING:
            // date to varchar
            UnaryExecution::genericExecute<int32_t, string_t, StringCastFromDateOperator>(source, result, count,
                                                                                (void *)&input);
            return true;
       default:
        ErrorHandler::errorNotImplemented("Unimplemented type for execute operation!");
    }
    return false;
}

struct StringCastFromTimestampOperator {
    template <class INPUT_TYPE, class RESULT_TYPE>
    static RESULT_TYPE operation(INPUT_TYPE input, void *dataptr) {
        auto data = (VectorTryCastData *)dataptr;
        return StringCastFromTimestamp::operation<INPUT_TYPE>(input, data->result_);
    }
};

static bool timestampCastSwitch(Vector &source, Vector &result, idx_t count, string *errorMessage) {
    VectorTryCastData input(result, errorMessage);
    switch (result.getLogicalTypeId()) {
        case LogicalTypeId::STRING:
            // date to varchar
            UnaryExecution::genericExecute<int64_t, string_t, StringCastFromTimestampOperator>(source, result, count,
                                                                                (void *)&input);
            return true;
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for execute operation!");
    }
    return false;
}


void VectorOperations::cast(Vector &source, Vector &result, idx_t count) {
    if (source.getLogicalType() != result.getLogicalType())
        tryCast(source, result, count, nullptr);
    else
        copy(source, result, count, 0, 0);
}

bool VectorOperations::tryCast(Vector &source, Vector &result, idx_t count, string *errorMessage) {
    BB_ASSERT(source.getLogicalType() != result.getLogicalType());

    switch (source.getLogicalTypeId()) {
        case LogicalTypeId::TINYINT:
            return numericCastSwitch<int8_t>(source, result, count, errorMessage);
        case LogicalTypeId::SMALLINT:
            return numericCastSwitch<int16_t>(source, result, count, errorMessage);
        case LogicalTypeId::INTEGER:
            return numericCastSwitch<int32_t>(source, result, count, errorMessage);
        case LogicalTypeId::BOOLEAN:
        case LogicalTypeId::UTINYINT:
            return numericCastSwitch<uint8_t>(source, result, count, errorMessage);
        case LogicalTypeId::USMALLINT:
            return numericCastSwitch<uint16_t>(source, result, count, errorMessage);
        case LogicalTypeId::UINTEGER:
            return numericCastSwitch<uint32_t>(source, result, count, errorMessage);
        case LogicalTypeId::HASH:
        case LogicalTypeId::ADDRESS:
        case LogicalTypeId::UBIGINT:
            return numericCastSwitch<uint64_t>(source, result, count, errorMessage);
        case LogicalTypeId::BIGINT:
            return numericCastSwitch<int64_t>(source, result, count, errorMessage);
        case LogicalTypeId::FLOAT:
            return numericCastSwitch<float>(source, result, count, errorMessage);
        case LogicalTypeId::DOUBLE:
            return numericCastSwitch<double>(source, result, count, errorMessage);
        case LogicalTypeId::STRING:
            return stringCastSwitch(source, result, count, errorMessage);
        case LogicalTypeId::DECIMAL: {
            switch (source.getType()) {
                case PhysicalType::SMALLINT:
                    return decimalCastSwitch<int16_t>(source, result, count, errorMessage);
                    break;
                case PhysicalType::INTEGER:
                    return decimalCastSwitch<int32_t>(source, result, count, errorMessage);
                    break;
                case PhysicalType::BIGINT:
                    return decimalCastSwitch<int64_t>(source, result, count, errorMessage);
                    break;
                default:
                    ErrorHandler::errorNotImplemented("Unimplemented type for execute operation!");
            }
            break;
        }
        case LogicalTypeId::DATE:
            return dateCastSwitch(source, result, count, errorMessage);
        case LogicalTypeId::TIMESTAMP:
            return timestampCastSwitch(source, result, count, errorMessage);

        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for cast operation!");
    }
    return false;
}


}
