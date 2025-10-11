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
        case ConstantType::TINYINT:
            return vectorCastLoop<SRC, int8_t, NumericTryCast>(source, result, count, errorMessage);

        case ConstantType::SMALLINT:
            return vectorCastLoop<SRC, int16_t, NumericTryCast>(source, result, count, errorMessage);

        case ConstantType::INTEGER:
            return vectorCastLoop<SRC, int32_t, NumericTryCast>(source, result, count, errorMessage);

        case ConstantType::UTINYINT:
            return vectorCastLoop<SRC, uint8_t, NumericTryCast>(source, result, count, errorMessage);

        case ConstantType::USMALLINT:
            return vectorCastLoop<SRC, uint16_t, NumericTryCast>(source, result, count, errorMessage);

        case ConstantType::UINTEGER:
            return vectorCastLoop<SRC, uint32_t, NumericTryCast>(source, result, count, errorMessage);

        case ConstantType::UBIGINT:
            return vectorCastLoop<SRC, uint64_t, NumericTryCast>(source, result, count, errorMessage);

        case ConstantType::BIGINT:
            return vectorCastLoop<SRC, int64_t, NumericTryCast>(source, result, count, errorMessage);

        case ConstantType::FLOAT:
            return vectorCastLoop<SRC, float, NumericTryCast>(source, result, count, errorMessage);

        case ConstantType::DOUBLE:
            return vectorCastLoop<SRC, double, NumericTryCast>(source, result, count, errorMessage);

        case ConstantType::STRING:
            return vectorStringCastLoop<SRC, string_t, StringCast>(source, result, count, errorMessage);


        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for execute operation!");

        return false;
    }
}



bool stringCastSwitch(Vector &source, Vector &result, idx_t count, string *errorMessage) {

    switch (result.getType()) {
        case ConstantType::TINYINT:
            return vectorCastLoop<string_t, int8_t, TryIntegerCast>(source, result, count, errorMessage);

        case ConstantType::SMALLINT:
            return vectorCastLoop<string_t, int16_t, TryIntegerCast>(source, result, count, errorMessage);

        case ConstantType::INTEGER:
            return vectorCastLoop<string_t, int32_t, TryIntegerCast>(source, result, count, errorMessage);

        case ConstantType::UTINYINT:
            return vectorCastLoop<string_t, uint8_t, TryIntegerCast>(source, result, count, errorMessage);

        case ConstantType::USMALLINT:
            return vectorCastLoop<string_t, uint16_t, TryIntegerCast>(source, result, count, errorMessage);

        case ConstantType::UINTEGER:
            return vectorCastLoop<string_t, uint32_t, TryIntegerCast>(source, result, count, errorMessage);

        case ConstantType::UBIGINT:
            return vectorCastLoop<string_t, uint64_t, TryIntegerCast>(source, result, count, errorMessage);

        case ConstantType::BIGINT:
            return vectorCastLoop<string_t, int64_t, TryIntegerCast>(source, result, count, errorMessage);

        case ConstantType::FLOAT:
            return vectorCastLoop<string_t, float, TryDoubleCast>(source, result, count, errorMessage);

        case ConstantType::DOUBLE:
            return vectorCastLoop<string_t, double, TryDoubleCast>(source, result, count, errorMessage);

        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for execute operation!");
    }
    return false;
}


void VectorOperations::cast(Vector &source, Vector &result, idx_t count) {
    tryCast(source, result, count, nullptr);
}

bool VectorOperations::tryCast(Vector &source, Vector &result, idx_t count, string *errorMessage) {
    BB_ASSERT(source.getType() != result.getType());

    switch (source.getType()) {
        case ConstantType::TINYINT:
            return numericCastSwitch<int8_t>(source, result, count, errorMessage);
        case ConstantType::SMALLINT:
            return numericCastSwitch<int16_t>(source, result, count, errorMessage);
        case ConstantType::INTEGER:
            return numericCastSwitch<int32_t>(source, result, count, errorMessage);
        case ConstantType::UTINYINT:
            return numericCastSwitch<uint8_t>(source, result, count, errorMessage);
        case ConstantType::USMALLINT:
            return numericCastSwitch<uint16_t>(source, result, count, errorMessage);
        case ConstantType::UINTEGER:
            return numericCastSwitch<uint32_t>(source, result, count, errorMessage);
        case ConstantType::UBIGINT:
            return numericCastSwitch<uint64_t>(source, result, count, errorMessage);
        case ConstantType::BIGINT:
            return numericCastSwitch<int64_t>(source, result, count, errorMessage);
        case ConstantType::FLOAT:
            return numericCastSwitch<float>(source, result, count, errorMessage);
        case ConstantType::DOUBLE:
            return numericCastSwitch<double>(source, result, count, errorMessage);
        case ConstantType::STRING:
            return stringCastSwitch(source, result, count, errorMessage);

        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for cast operation!");
    }
    return false;
}


}
