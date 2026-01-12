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

#include "bumblebee/common/Helper.hpp"
#include "bumblebee/common/row_operations/RowOperations.hpp"

namespace bumblebee {


template <class T>
static void templatedGatherLoop(Vector &rows, const SelectionVector &row_sel, Vector &col,
                                const SelectionVector &col_sel, idx_t count, idx_t col_offset) {

	auto ptrs = FlatVector::getData<data_ptr_t>(rows);
	auto data = FlatVector::getData<T>(col);

	for (idx_t i = 0; i < count; i++) {
		auto row_idx = row_sel.getIndex(i);
		auto row = ptrs[row_idx];
		auto col_idx = col_sel.getIndex(i);
		data[col_idx] = load<T>(row + col_offset);
	}
}


void RowOperations::gather(Vector &rows, const SelectionVector &row_sel, Vector &col, const SelectionVector &col_sel,
                           const idx_t count, const idx_t col_offset) {
	BB_ASSERT(rows.getVectorType() == VectorType::FLAT_VECTOR);
	BB_ASSERT(rows.getType() == PhysicalType::UBIGINT); // "Cannot gather from non-pointer type!"

	col.setVectorType(VectorType::FLAT_VECTOR);

	switch (col.getType()) {
		case PhysicalType::TINYINT:
			templatedGatherLoop<int8_t>(rows, row_sel, col, col_sel, count, col_offset);
			break;
		case PhysicalType::SMALLINT:
			templatedGatherLoop<int16_t>(rows, row_sel, col, col_sel, count, col_offset);
			break;
		case PhysicalType::INTEGER:
			templatedGatherLoop<int32_t>(rows, row_sel, col, col_sel, count, col_offset);
			break;
		case PhysicalType::BIGINT:
			templatedGatherLoop<int64_t>(rows, row_sel, col, col_sel, count, col_offset);
			break;
		case PhysicalType::UTINYINT:
			templatedGatherLoop<uint8_t>(rows, row_sel, col, col_sel, count, col_offset);
			break;
		case PhysicalType::USMALLINT:
			templatedGatherLoop<uint16_t>(rows, row_sel, col, col_sel, count, col_offset);
			break;
		case PhysicalType::UINTEGER:
			templatedGatherLoop<uint32_t>(rows, row_sel, col, col_sel, count, col_offset);
			break;
		case PhysicalType::UBIGINT:
			templatedGatherLoop<uint64_t>(rows, row_sel, col, col_sel, count, col_offset);
			break;
		case PhysicalType::FLOAT:
			templatedGatherLoop<float>(rows, row_sel, col, col_sel, count, col_offset);
			break;
		case PhysicalType::DOUBLE:
			templatedGatherLoop<double>(rows, row_sel, col, col_sel, count, col_offset);
			break;
		case PhysicalType::STRING:	{
			templatedGatherLoop<string_t>(rows, row_sel, col, col_sel, count, col_offset);
			break;
		}
		default:
			ErrorHandler::errorNotImplemented("Unimplemented type for RowOperations::gather");
	}


}

template <class T>
static void templatedFullScanLoop(Vector &rows, Vector &col, idx_t count, idx_t col_offset, idx_t col_no) {
	// Precompute mask indexes

	auto ptrs = FlatVector::getData<data_ptr_t>(rows);
	auto data = FlatVector::getData<T>(col);
	//	auto &col_mask = FlatVector::Validity(col);

	for (idx_t i = 0; i < count; i++) {
		auto row = ptrs[i];
		data[i] = load<T>(row + col_offset);
	}
}

void RowOperations::fullScanColumn(const RowLayout &layout, Vector &rows, Vector &col, idx_t count, idx_t col_no) {
	const auto col_offset = layout.getOffsets()[col_no];
	col.setVectorType(VectorType::FLAT_VECTOR);
	switch (col.getType()) {

		case PhysicalType::TINYINT:
			templatedFullScanLoop<int8_t>(rows, col, count, col_offset, col_no);
			break;
		case PhysicalType::SMALLINT:
			templatedFullScanLoop<int16_t>(rows, col, count, col_offset, col_no);
			break;
		case PhysicalType::INTEGER:
			templatedFullScanLoop<int32_t>(rows, col, count, col_offset, col_no);
			break;
		case PhysicalType::BIGINT:
			templatedFullScanLoop<int64_t>(rows, col, count, col_offset, col_no);
			break;
		case PhysicalType::UTINYINT:
			templatedFullScanLoop<uint8_t>(rows, col, count, col_offset, col_no);
			break;
		case PhysicalType::USMALLINT:
			templatedFullScanLoop<uint16_t>(rows, col, count, col_offset, col_no);
			break;
		case PhysicalType::UINTEGER:
			templatedFullScanLoop<uint32_t>(rows, col, count, col_offset, col_no);
			break;
		case PhysicalType::UBIGINT:
			templatedFullScanLoop<uint64_t>(rows, col, count, col_offset, col_no);
			break;
		case PhysicalType::FLOAT:
			templatedFullScanLoop<float>(rows, col, count, col_offset, col_no);
			break;
		case PhysicalType::DOUBLE:
			templatedFullScanLoop<double>(rows, col, count, col_offset, col_no);
			break;
		case PhysicalType::STRING:	{
			templatedFullScanLoop<string_t>(rows, col, count, col_offset, col_no);
			break;
		}
		default:
			ErrorHandler::errorNotImplemented("Unimplemented type for RowOperations::fullScanColumn");

	}
}

}
