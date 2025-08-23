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

#include "bumblebee/common/vector_operations/VectorOperations.h"

namespace bumblebee {


template <class T>
static void TemplatedCopy(const Vector &source, const SelectionVector &sel, Vector &target, const SelectionVector &targetSel, idx_t source_offset,
                          idx_t target_offset, idx_t copy_count) {
    auto ldata = FlatVector::getData<T>(source);
    auto tdata = FlatVector::getData<T>(target);
    for (idx_t i = 0; i < copy_count; i++) {
        auto source_idx = sel.getIndex(source_offset + i);
        auto target_idx = targetSel.getIndex(target_offset + i);
        tdata[target_idx] = ldata[source_idx];
    }
}


void VectorOperations::copy(const Vector &source, Vector &target, idx_t sourceCount, idx_t sourceOffset, idx_t targetOffset) {
	switch (source.getVectorType()) {
        case VectorType::DICTIONARY_VECTOR: {
            // dictionary: continue into child with selection vector
            auto &child = DictionaryVector::child(source);
            auto &dict_sel = DictionaryVector::selVector(source);
            VectorOperations::copy(child, target, dict_sel, sourceCount, sourceOffset, targetOffset);
            break;
        }
        case VectorType::CONSTANT_VECTOR: {
            SelectionVector ownedSel;
            auto sel = ConstantVector::zeroSelectionVector(sourceCount, ownedSel);
            VectorOperations::copy(source, target, *sel, sourceCount, sourceOffset, targetOffset);
            break;
        }
        case VectorType::FLAT_VECTOR: {
            VectorOperations::copy(source, target, FlatVector::INCREMENTAL_SELECTION_VECTOR, sourceCount, sourceOffset,
                                   targetOffset);
            break;
        }
        case VectorType::SEQUENCE_VECTOR: {
            int64_t start, increment;
            SequenceVector::getSequence(source, start, increment);
            Vector flattened(source.getType());
            VectorOperations::generateSequence(flattened, sourceCount, start, increment);
            VectorOperations::copy(flattened, target, FlatVector::INCREMENTAL_SELECTION_VECTOR, sourceCount, sourceOffset,
                                   targetOffset);
            break;
        }
    	case VectorType::SEQUENCE_CIRCULAR_VECTOR: {
        	int64_t start, offset, stride, end;
        	CircularSequenceVector::getSequence(source, start, offset, stride, end);
        	Vector flattened(source.getType());
        	VectorOperations::generateSequence(flattened, sourceCount, start, offset, stride, end);
        	VectorOperations::copy(flattened, target, FlatVector::INCREMENTAL_SELECTION_VECTOR, sourceCount, sourceOffset,
								   targetOffset);
        	break;
    	}
        default:
            ErrorHandler::errorNotImplemented("Error: unimplemented vector type for VectorOperations::Copy");
    }
}

void VectorOperations::copy(const Vector &source, Vector &target, const SelectionVector &sel, idx_t sourceCount, idx_t sourceOffset, idx_t targetOffset) {
	VectorOperations::copy(source, target, sel, FlatVector::INCREMENTAL_SELECTION_VECTOR, sourceCount, sourceOffset,targetOffset);
}

void VectorOperations::copy(const Vector &source, Vector &target, const SelectionVector &sel,const SelectionVector &targetSel, idx_t sourceCount, idx_t sourceOffset, idx_t targetOffset) {
	BB_ASSERT(sourceOffset <= sourceCount);
	BB_ASSERT(source.getType() == target.getType());
	idx_t copyCount = sourceCount - sourceOffset;

	SelectionVector ownedSel;
	switch (source.getVectorType()) {
	case VectorType::DICTIONARY_VECTOR: {
		// dictionary vector: merge selection vectors
		auto &child = DictionaryVector::child(source);
		auto &dict_sel = DictionaryVector::selVector(source);
		// merge the selection vectors and verify the child
		auto newBuffer = dict_sel.slice(sel, sourceCount);
		SelectionVector mergedSel(newBuffer);
		// recursively call until the child is flat
		VectorOperations::copy(child, target, mergedSel, sourceCount, sourceOffset, targetOffset);
		return;
	}
	case VectorType::SEQUENCE_VECTOR: {
		int64_t start, increment;
		Vector seq(source.getType());
		SequenceVector::getSequence(source, start, increment);
		VectorOperations::generateSequence(seq, sourceCount, sel, start, increment);
		VectorOperations::copy(seq, target, sel, sourceCount, sourceOffset, targetOffset);
		return;
	}
	case VectorType::SEQUENCE_CIRCULAR_VECTOR: {
		int64_t start, stride, end, offset;
		Vector seq(source.getType());
		CircularSequenceVector::getSequence(source, start,offset, stride, end);
		VectorOperations::generateSequence(seq, sourceCount, sel, start, offset, stride, end);
		VectorOperations::copy(seq, target, sel, sourceCount, sourceOffset, targetOffset);
		return;
		}
	case VectorType::CONSTANT_VECTOR:
		// sel SHOULD BE a ZERO SELECTION VECTOR
		break; // carry on with below code
	case VectorType::FLAT_VECTOR:
		break;
	default:
        ErrorHandler::errorNotImplemented("Error: unimplemented vector type for VectorOperations::Copy");
	}

	if (copyCount == 0) {
		return;
	}

	// Allow copying of a single value to constant vectors
	const auto tvType = target.getVectorType();
	if (copyCount == 1 && tvType == VectorType::CONSTANT_VECTOR) {
		targetOffset = 0;
		target.setVectorType(VectorType::FLAT_VECTOR);
	}
	BB_ASSERT(target.getVectorType() == VectorType::FLAT_VECTOR);


	// now copy over the data
	switch (source.getType()) {
		case ConstantType::TINYINT:
			TemplatedCopy<int8_t>(source, sel, target, targetSel, sourceOffset, targetOffset, copyCount);
			break;
		case ConstantType::SMALLINT:
			TemplatedCopy<int16_t>(source, sel, target, targetSel, sourceOffset, targetOffset, copyCount);
			break;
		case ConstantType::INTEGER:
			TemplatedCopy<int32_t>(source, sel, target, targetSel, sourceOffset, targetOffset, copyCount);
			break;
		case ConstantType::BIGINT:
			TemplatedCopy<int64_t>(source, sel, target, targetSel, sourceOffset, targetOffset, copyCount);
			break;
		case ConstantType::UTINYINT:
			TemplatedCopy<uint8_t>(source, sel, target, targetSel, sourceOffset, targetOffset, copyCount);
			break;
		case ConstantType::USMALLINT:
			TemplatedCopy<uint16_t>(source, sel, target, targetSel, sourceOffset, targetOffset, copyCount);
			break;
		case ConstantType::UINTEGER:
			TemplatedCopy<uint32_t>(source, sel, target, targetSel, sourceOffset, targetOffset, copyCount);
			break;
		case ConstantType::UBIGINT:
			TemplatedCopy<uint64_t>(source, sel, target, targetSel, sourceOffset, targetOffset, copyCount);
			break;
		case ConstantType::FLOAT:
			TemplatedCopy<float>(source, sel, target, targetSel, sourceOffset, targetOffset, copyCount);
			break;
		case ConstantType::DOUBLE:
			TemplatedCopy<double>(source, sel, target, targetSel, sourceOffset, targetOffset, copyCount);
			break;
		case ConstantType::STRING:	{
			auto ldata = FlatVector::getData<string_t>(source);
			auto tdata = FlatVector::getData<string_t>(target);
			for (idx_t i = 0; i < copyCount; i++) {
				auto source_idx = sel.getIndex(sourceOffset + i);
				auto target_idx = targetSel.getIndex(targetOffset + i);
				tdata[target_idx] = StringVector::addString(target, ldata[source_idx]);
			}
			break;
			}
		default:
			ErrorHandler::errorNotImplemented("Unimplemented type '%s' for copy!");
	}

	if (tvType != VectorType::FLAT_VECTOR) {
		target.setVectorType(tvType);
	}
}


}