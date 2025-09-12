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

#include "bumblebee/common/Hash.hpp"
#include "bumblebee/common/vector_operations/VectorOperations.hpp"

namespace bumblebee {


template <bool HAS_RSEL, class T>
static inline void TightLoopHash(T *__restrict ldata, hash_t *__restrict rdata, const SelectionVector *rsel,
                                 idx_t count, const SelectionVector *__restrict lsel) {

    for (idx_t i = 0; i < count; i++) {
        auto ridx = HAS_RSEL ? rsel->getIndex(i) : i;
        auto lidx = lsel->getIndex(ridx);
        rdata[ridx] = Hash<T>(ldata[lidx]);
    }
}

template <bool HAS_RSEL, class T>
static inline void TemplatedLoopHash(Vector &input, Vector &result, const SelectionVector *rsel, idx_t count) {
    if (input.getVectorType() == VectorType::CONSTANT_VECTOR) {
        result.setVectorType(VectorType::CONSTANT_VECTOR);

        auto ldata = ConstantVector::getData<T>(input);
        auto result_data = ConstantVector::getData<hash_t>(result);
        *result_data =  Hash<T>(*ldata);
    } else {
        result.setVectorType(VectorType::FLAT_VECTOR);

        VectorData idata;
        input.orrify(count, idata);

        TightLoopHash<HAS_RSEL, T>((T *)idata.data_, FlatVector::getData<hash_t>(result), rsel, count, idata.sel_);
    }
}


template <bool HAS_RSEL>
static inline void HashTypeSwitch(Vector &input, Vector &result, const SelectionVector *rsel, idx_t count) {
    BB_ASSERT(result.getType() == ConstantType::UBIGINT);

    switch (input.getType()) {
        case ConstantType::TINYINT:
		    TemplatedLoopHash<HAS_RSEL, int8_t>(input, result, rsel, count);
            break;
        case ConstantType::SMALLINT:
		    TemplatedLoopHash<HAS_RSEL, int16_t>(input, result, rsel, count);
            break;
        case ConstantType::INTEGER:
		    TemplatedLoopHash<HAS_RSEL, int32_t>(input, result, rsel, count);
            break;
        case ConstantType::BIGINT:
		    TemplatedLoopHash<HAS_RSEL, int64_t>(input, result, rsel, count);
            break;
        case ConstantType::UTINYINT:
		    TemplatedLoopHash<HAS_RSEL, uint8_t>(input, result, rsel, count);
            break;
        case ConstantType::USMALLINT:
		    TemplatedLoopHash<HAS_RSEL, uint16_t>(input, result, rsel, count);
            break;
        case ConstantType::UINTEGER:
		    TemplatedLoopHash<HAS_RSEL, uint32_t>(input, result, rsel, count);
            break;
        case ConstantType::UBIGINT:
		    TemplatedLoopHash<HAS_RSEL, uint64_t>(input, result, rsel, count);
            break;
        case ConstantType::FLOAT:
		    TemplatedLoopHash<HAS_RSEL, float>(input, result, rsel, count);
            break;
        case ConstantType::DOUBLE:
		    TemplatedLoopHash<HAS_RSEL, double>(input, result, rsel, count);
            break;
        case ConstantType::STRING:
            TemplatedLoopHash<HAS_RSEL, string_t>(input, result, rsel, count);
            break;
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type hash!");
    }
}

void VectorOperations::hash(Vector &input, Vector &result, idx_t count) {
    HashTypeSwitch<false>(input, result, nullptr, count);
}

void VectorOperations::hash(Vector &input, Vector &result, const SelectionVector &sel, idx_t count) {
    HashTypeSwitch<true>(input, result, &sel, count);
}

static inline hash_t combineHashScalar(hash_t a, hash_t b) {
    return (a * UINT64_C(0xbf58476d1ce4e5b9)) ^ b;
}

template <bool HAS_RSEL, class T>
static inline void TightLoopCombineHashConstant(T *__restrict ldata, hash_t constant_hash, hash_t *__restrict hashData,
                                                const SelectionVector *rsel, idx_t count,
                                                const SelectionVector *__restrict lsel) {

	for (idx_t i = 0; i < count; i++) {
		auto ridx = HAS_RSEL ? rsel->getIndex(i) : i;
		auto idx = lsel->getIndex(ridx);
		auto other_hash = Hash<T>(ldata[idx]);
		hashData[ridx] = combineHashScalar(constant_hash, other_hash);
	}

}

template <bool HAS_RSEL, class T>
static inline void TightLoopCombineHash(T *__restrict ldata, hash_t *__restrict hashData, const SelectionVector *rsel,
                                        idx_t count, const SelectionVector *__restrict lsel) {

	for (idx_t i = 0; i < count; i++) {
		auto ridx = HAS_RSEL ? rsel->getIndex(i) : i;
		auto idx = lsel->getIndex(ridx);
		auto otherHash = Hash<T>(ldata[idx]);
		hashData[ridx] = combineHashScalar(hashData[ridx], otherHash);
	}

}

template <bool HAS_RSEL, class T>
void TemplatedLoopCombineHash(Vector &input, Vector &hashes, const SelectionVector *rsel, idx_t count) {
	if (input.getVectorType() == VectorType::CONSTANT_VECTOR && hashes.getVectorType() == VectorType::CONSTANT_VECTOR) {
		// both vector are constant vector
		auto ldata = ConstantVector::getData<T>(input);
		auto hash = ConstantVector::getData<hash_t>(hashes);
		auto other_hash = Hash<T>(*ldata);
		*hash = combineHashScalar(*hash, other_hash);
		return;
	}
	// check if the hash vector is constant
	VectorData idata;
	input.orrify(count, idata);
	if (hashes.getVectorType() == VectorType::CONSTANT_VECTOR) {
		// input vector non constant hash constant
		auto hash = *ConstantVector::getData<hash_t>(hashes);
		// now re-initialize the hashes vector to an empty flat vector
		// as we need to combine with the input vector
		hashes.setVectorType(VectorType::FLAT_VECTOR);
		TightLoopCombineHashConstant<HAS_RSEL, T>((T *)idata.data_, hash, FlatVector::getData<hash_t>(hashes), rsel, count, idata.sel_);
		return;
	}
	BB_ASSERT(hashes.getVectorType() == VectorType::FLAT_VECTOR);
	TightLoopCombineHash<HAS_RSEL, T>((T *)idata.data_, FlatVector::getData<hash_t>(hashes), rsel, count, idata.sel_);

}


template <bool HAS_RSEL>
static inline void CombineHashTypeSwitch(Vector &hashes, Vector &input, const SelectionVector *rsel, idx_t count) {
	BB_ASSERT(hashes.getType() == ConstantType::UBIGINT);

	switch (input.getType()) {
		case ConstantType::TINYINT:
			TemplatedLoopCombineHash<HAS_RSEL, int8_t>(input, hashes, rsel, count);
			break;
		case ConstantType::SMALLINT:
			TemplatedLoopCombineHash<HAS_RSEL, int16_t>(input, hashes, rsel, count);
			break;
		case ConstantType::INTEGER:
			TemplatedLoopCombineHash<HAS_RSEL, int32_t>(input, hashes, rsel, count);
			break;
		case ConstantType::BIGINT:
			TemplatedLoopCombineHash<HAS_RSEL, int64_t>(input, hashes, rsel, count);
			break;
		case ConstantType::UTINYINT:
			TemplatedLoopCombineHash<HAS_RSEL, uint8_t>(input, hashes, rsel, count);
			break;
		case ConstantType::USMALLINT:
			TemplatedLoopCombineHash<HAS_RSEL, uint16_t>(input, hashes, rsel, count);
			break;
		case ConstantType::UINTEGER:
			TemplatedLoopCombineHash<HAS_RSEL, uint32_t>(input, hashes, rsel, count);
			break;
		case ConstantType::UBIGINT:
			TemplatedLoopCombineHash<HAS_RSEL, uint64_t>(input, hashes, rsel, count);
			break;
		case ConstantType::FLOAT:
			TemplatedLoopCombineHash<HAS_RSEL, float>(input, hashes, rsel, count);
			break;
		case ConstantType::DOUBLE:
			TemplatedLoopCombineHash<HAS_RSEL, double>(input, hashes, rsel, count);
			break;
		case ConstantType::STRING:
			TemplatedLoopCombineHash<HAS_RSEL, string_t>(input, hashes, rsel, count);
			break;
		default:
			ErrorHandler::errorNotImplemented("Unimplemented combine hash type!");
	}
}

void VectorOperations::combineHash(Vector &hashes, Vector &input, idx_t count) {
	CombineHashTypeSwitch<false>(hashes, input, nullptr, count);
}

void VectorOperations::combineHash(Vector &hashes, Vector &input, const SelectionVector &rsel, idx_t count) {
	CombineHashTypeSwitch<true>(hashes, input, &rsel, count);
}
}