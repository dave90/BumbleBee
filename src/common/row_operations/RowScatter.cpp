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

#include "bumblebee/common/row_operations/RowOperations.hpp"
#include "bumblebee/common/types/DataChunk.hpp"
#include "bumblebee/common/types/RowDataCollection.hpp"
#include "bumblebee/common/types/RowLayout.hpp"

namespace bumblebee {


template <class T>
static void templatedScatter(VectorData &col, Vector &rows, const SelectionVector &sel, const idx_t count,
                             const idx_t col_offset, const idx_t col_no) {
	auto data = (T *) col.data_;
	auto ptrs = FlatVector::getData<data_ptr_t>(rows);

	for (idx_t i = 0; i < count; i++) {
		auto idx = sel.getIndex(i);
		auto col_idx = col.sel_->getIndex(idx);
		auto row = ptrs[idx];
		store<T>(data[col_idx], row + col_offset);
	}

}

static void computeStringEntrySizes(const VectorData &col, idx_t entry_sizes[], const SelectionVector &sel,
                                    const idx_t count, const idx_t offset = 0) {
	auto data = (const string_t *)col.data_;
	for (idx_t i = 0; i < count; i++) {
		auto idx = sel.getIndex(i);
		auto col_idx = col.sel_->getIndex(idx) + offset;
		const auto &str = data[col_idx];
		if (!str.isInlined()) {
			entry_sizes[i] += str.size();
		}
	}
}

static void scatterStringVector(VectorData &col, Vector &rows, data_ptr_t str_locations[], const SelectionVector &sel,
                                const idx_t count, const idx_t col_offset, const idx_t col_no) {
	auto string_data = (string_t *)col.data_;
	auto ptrs = FlatVector::getData<data_ptr_t>(rows);

	for (idx_t i = 0; i < count; i++) {
		auto idx = sel.getIndex(i);
		auto col_idx = col.sel_->getIndex(idx);
		auto row = ptrs[idx];
		if (string_data[col_idx].isInlined()) {
			store<string_t>(string_data[col_idx], row + col_offset);
		} else {
			const auto &str = string_data[col_idx];
			string_t inserted((const char *)str_locations[i], str.size());
			memcpy(inserted.getDataWriteable(), str.getDataUnsafe(), str.size());
			str_locations[i] += str.size();
			store<string_t>(inserted, row + col_offset);
		}
	}
}


void RowOperations::scatter(DataChunk &columns, VectorData col_data[], const RowLayout &layout, Vector &rows,
                    RowDataCollection &string_heap, const SelectionVector &sel, idx_t count) {
    if (count == 0) {
		return;
	}

	// Set the validity mask for each row before inserting data
	auto ptrs = FlatVector::getData<data_ptr_t>(rows);

	auto &offsets = layout.getOffsets();
	auto &types = layout.getTypes();

	// Compute the entry size of the variable size columns
	vector<buffer_handle_ptr_t> handles;
	idx_t locations_size = (count < STANDARD_VECTOR_SIZE) ? STANDARD_VECTOR_SIZE: count;
	data_ptr_t data_locations[locations_size];
	if (!layout.allConstant()) {
		idx_t entry_sizes[locations_size];
		std::fill_n(entry_sizes, count, sizeof(uint32_t)); // all entry size start with 4 byte size (heap  size)
		for (idx_t col_no = 0; col_no < types.size(); col_no++) {
			if (typeIsConstantSize(types[col_no])) {
				continue;
			}

			auto &col = col_data[col_no];
			switch (types[col_no]) {
			case STRING:
				computeStringEntrySizes(col, entry_sizes, sel, count);
				break;
			default:
					ErrorHandler::errorGeneric("Unsupported type for RowOperations::Scatter");
			}
		}

		// Build out the buffer space
		string_heap.build(count, data_locations, entry_sizes);

		// Serialize information that is needed for swizzling if the computation goes out-of-core
		const idx_t heap_pointer_offset = layout.getHeapPointerOffset();
		for (idx_t i = 0; i < count; i++) {
			auto row_idx = sel.getIndex(i);
			auto row = ptrs[row_idx];
			// Pointer to this row in the heap block
			store<data_ptr_t>(data_locations[i], row + heap_pointer_offset);
			// Row size is stored in the heap in front of each row
			store<uint32_t>(entry_sizes[i], data_locations[i]);
			data_locations[i] += sizeof(uint32_t);
		}
	}

	for (idx_t col_no = 0; col_no < types.size(); col_no++) {
		auto &col = col_data[col_no];
		auto col_offset = offsets[col_no];

		switch (types[col_no]) {
			case ConstantType::TINYINT:
				templatedScatter<int8_t>(col, rows, sel, count, col_offset, col_no);
				break;
			case ConstantType::SMALLINT:
				templatedScatter<int16_t>(col, rows, sel, count, col_offset, col_no);
				break;
			case ConstantType::INTEGER:
				templatedScatter<int32_t>(col, rows, sel, count, col_offset, col_no);
				break;
			case ConstantType::BIGINT:
				templatedScatter<int64_t>(col, rows, sel, count, col_offset, col_no);
				break;
			case ConstantType::UTINYINT:
				templatedScatter<uint8_t>(col, rows, sel, count, col_offset, col_no);
				break;
			case ConstantType::USMALLINT:
				templatedScatter<uint16_t>(col, rows, sel, count, col_offset, col_no);
				break;
			case ConstantType::UINTEGER:
				templatedScatter<uint32_t>(col, rows, sel, count, col_offset, col_no);
				break;
			case ConstantType::UBIGINT:
				templatedScatter<uint64_t>(col, rows, sel, count, col_offset, col_no);
				break;
			case ConstantType::FLOAT:
				templatedScatter<float>(col, rows, sel, count, col_offset, col_no);
				break;
			case ConstantType::DOUBLE:
				templatedScatter<double>(col, rows, sel, count, col_offset, col_no);
				break;
			case ConstantType::STRING:	{
				scatterStringVector(col, rows, data_locations, sel, count, col_offset, col_no);
				break;
			}
			default:
				ErrorHandler::errorNotImplemented("Unimplemented type for RowOperations::scatter");
		}

	}
}


}
