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
        : result(result_p), error_message(error_message_p) {
    }

    Vector &result;
    string *error_message;
    bool all_converted = true;
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
        return HandleVectorCastError::operation<RESULT_TYPE>("Error during conversion!", data->error_message, data->all_converted);
    }
};

template <class OP>
struct VectorStringCastOperator {
    template <class INPUT_TYPE, class RESULT_TYPE>
    static RESULT_TYPE operation(INPUT_TYPE input, void *dataptr) {
        auto result = (VectorTryCastData *)dataptr;
        return OP::template operation<INPUT_TYPE>(input, result->result);
    }
};


template <class SRC, class DST, class OP>
bool vectorCastLoop(Vector &source, Vector &result, idx_t count, string *errorMessage) {
    VectorTryCastData input(result, errorMessage);
    UnaryExecution::genericExecute<SRC, DST, VectorTryCastOperator<OP>>(source, result, count, &input);
    return input.all_converted;
}

template <class SRC, class DST, class OP>
bool vectorStringCastLoop(Vector &source, Vector &result, idx_t count, string *errorMessage) {
    VectorTryCastData input(result, errorMessage);
    UnaryExecution::genericExecute<SRC, DST, VectorStringCastOperator<OP>>(source, result, count, &input);
    return input.all_converted;
}


template <class SRC>
bool numericCastSwitch(Vector &source, Vector &result, idx_t count, string *errorMessage) {

    switch (result.getType()) {
        case PhysicalType::TINYINT:
            return vectorCastLoop<SRC, int8_t, NumericTryCast>(source, result, count, errorMessage);

        case PhysicalType::SMALLINT:
            return vectorCastLoop<SRC, int16_t, NumericTryCast>(source, result, count, errorMessage);

        case PhysicalType::INTEGER:
            return vectorCastLoop<SRC, int32_t, NumericTryCast>(source, result, count, errorMessage);

        case PhysicalType::UTINYINT:
            return vectorCastLoop<SRC, uint8_t, NumericTryCast>(source, result, count, errorMessage);

        case PhysicalType::USMALLINT:
            return vectorCastLoop<SRC, uint16_t, NumericTryCast>(source, result, count, errorMessage);

        case PhysicalType::UINTEGER:
            return vectorCastLoop<SRC, uint32_t, NumericTryCast>(source, result, count, errorMessage);

        case PhysicalType::UBIGINT:
            return vectorCastLoop<SRC, uint64_t, NumericTryCast>(source, result, count, errorMessage);

        case PhysicalType::BIGINT:
            return vectorCastLoop<SRC, int64_t, NumericTryCast>(source, result, count, errorMessage);

        case PhysicalType::FLOAT:
            return vectorCastLoop<SRC, float, NumericTryCast>(source, result, count, errorMessage);

        case PhysicalType::DOUBLE:
            return vectorCastLoop<SRC, double, NumericTryCast>(source, result, count, errorMessage);

        case PhysicalType::STRING:
            return vectorStringCastLoop<SRC, string_t, StringTryCast>(source, result, count, errorMessage);


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

struct DecimalCastInput {
    DecimalCastInput(Vector &result_p, uint8_t width_p, uint8_t scale_p)
        : result(result_p), width(width_p), scale(scale_p) {
    }

    Vector &result;
    uint8_t width;
    uint8_t scale;
};


struct StringCastFromDecimalOperator {
    template <class INPUT_TYPE, class RESULT_TYPE>
    static RESULT_TYPE operation(INPUT_TYPE input, void *dataptr) {
        auto data = (DecimalCastInput *)dataptr;
        return StringTryCastFromDecimal::operation<INPUT_TYPE>(input, data->width, data->scale, data->result);
    }
};

static bool decimalCastSwitch(Vector &source, Vector &result, idx_t count, string *errorMessage) {
	// now switch on the result type
	switch (result.getLogicalTypeId()) {
	    case LogicalTypeId::STRING: {
		    auto source_type = source.getLogicalType();
		    int width, scale;
	        source_type.getDecimalWidhtAndScale(width, scale);
		    DecimalCastInput input(result, width, scale);
		    switch (source_type.getPhysicalType()) {
		    case PhysicalType::SMALLINT:
			    UnaryExecution::genericExecute<int16_t, string_t, StringCastFromDecimalOperator>(source, result, count,
			                                                                                    (void *)&input);
			    break;
		    case PhysicalType::INTEGER:
			    UnaryExecution::genericExecute<int32_t, string_t, StringCastFromDecimalOperator>(source, result, count,
			                                                                                    (void *)&input);
			    break;
		    case PhysicalType::BIGINT:
			    UnaryExecution::genericExecute<int64_t, string_t, StringCastFromDecimalOperator>(source, result, count,
			                                                                                    (void *)&input);
			    break;
		    default:
                ErrorHandler::errorNotImplemented("Unimplemented type for execute operation!");
		    }
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
        return StringCastFromDate::operation<INPUT_TYPE>(input, data->result);
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
        return StringCastFromTimestamp::operation<INPUT_TYPE>(input, data->result);
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
    if (source.getType() != result.getType())
        tryCast(source, result, count, nullptr);
    else
        copy(source, result, count, 0, 0);
}

bool VectorOperations::tryCast(Vector &source, Vector &result, idx_t count, string *errorMessage) {
    BB_ASSERT(source.getType() != result.getType());

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
        case LogicalTypeId::DECIMAL:
            return decimalCastSwitch(source, result, count, errorMessage);
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
