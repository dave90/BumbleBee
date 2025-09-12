#include "bumblebee/common/Helper.hpp"
#include "bumblebee/common/operator/ComparisonOperators.hpp"
#include "bumblebee/common/row_operations/RowOperations.hpp"
#include "bumblebee/common/types/DataChunk.hpp"
#include "bumblebee/common/vector_operations/VectorOperations.hpp"
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
namespace bumblebee {

template <class ROW_TYPE, class VEC_TYPE, class OP, bool NO_MATCH_SEL>
static void templatedMatchType(VectorData &col, Vector &rows, SelectionVector &sel, idx_t &count, idx_t col_offset,
							   idx_t col_no, SelectionVector *no_match, idx_t &no_match_count) {

	auto data = (VEC_TYPE *)col.data_;
	auto ptrs = FlatVector::getData<data_ptr_t>(rows);
	idx_t match_count = 0;
	for (idx_t i = 0; i < count; i++) {
		auto idx = sel.getIndex(i);
		auto row = ptrs[idx];
		auto col_idx = col.sel_->getIndex(idx);
		auto value = load<ROW_TYPE>(row + col_offset);
		if (OP::operation(data[col_idx], value)) {
			sel.setIndex(match_count++, idx);
		} else {
			if (NO_MATCH_SEL) {
				no_match->setIndex(no_match_count++, idx);
			}
		}
	}

	count = match_count;
}

template<class ROW_TYPE, class VEC_TYPE, class COMMON_TYPE, class OP>
	struct ComparisonCommonCast {
	static inline bool operation(ROW_TYPE left, VEC_TYPE right) {
		return OP::operation(static_cast<COMMON_TYPE>(left), static_cast<COMMON_TYPE>(right));
	}
};

template <class ROW_TYPE, class VEC_TYPE, class OP, bool NO_MATCH_SEL>
static void templatedMatchOpSwitchCommon(Vector &vec, VectorData &col, const idx_t vcount, const RowLayout &layout, Vector &rows,
							 SelectionVector &sel, idx_t &count, idx_t col_no, SelectionVector *no_match,
							 idx_t &no_match_count) {
	auto col_offset = layout.getOffsets()[col_no];
	auto commonType = getCommonType(vec.getType(), layout.getTypes()[col_no]);

	// for int and uint collapse to int 64 bit
	// for decimal collapse to double
	switch (commonType) {
		case ConstantType::TINYINT:
		case ConstantType::SMALLINT:
		case ConstantType::INTEGER:
		case ConstantType::UTINYINT:
		case ConstantType::USMALLINT:
		case ConstantType::UINTEGER:
		case ConstantType::UBIGINT:
		case ConstantType::BIGINT:
			templatedMatchType<ROW_TYPE,VEC_TYPE, ComparisonCommonCast<ROW_TYPE, VEC_TYPE, int64_t, OP>, NO_MATCH_SEL>(col, rows, sel, count, col_offset, col_no, no_match, no_match_count);
			break;
		case ConstantType::FLOAT:
		case ConstantType::DOUBLE:
			templatedMatchType<ROW_TYPE,VEC_TYPE, ComparisonCommonCast<ROW_TYPE, VEC_TYPE, double, OP>, NO_MATCH_SEL>(col, rows, sel, count, col_offset, col_no, no_match, no_match_count);
			break;
		default:
			ErrorHandler::errorNotImplemented("Unimplemented row equal for select operation!");
	}
}

template <class ROW_TYPE, class OP, bool NO_MATCH_SEL>
static void templatedMatchOpSwitchVec(Vector &vec, VectorData &col, const idx_t vcount, const RowLayout &layout, Vector &rows,
							 SelectionVector &sel, idx_t &count, idx_t col_no, SelectionVector *no_match,
							 idx_t &no_match_count) {

	switch (vec.getType()) {
		case ConstantType::TINYINT:
			return templatedMatchOpSwitchCommon<ROW_TYPE, int8_t,OP,NO_MATCH_SEL>(vec, col, vcount, layout, rows, sel, count, col_no, no_match, no_match_count);
		case ConstantType::SMALLINT:
			return templatedMatchOpSwitchCommon<ROW_TYPE, int16_t,OP,NO_MATCH_SEL>(vec, col, vcount, layout, rows, sel, count, col_no, no_match, no_match_count);
		case ConstantType::INTEGER:
			return templatedMatchOpSwitchCommon<ROW_TYPE, int32_t,OP,NO_MATCH_SEL>(vec, col, vcount, layout, rows, sel, count, col_no, no_match, no_match_count);
		case ConstantType::BIGINT:
			return templatedMatchOpSwitchCommon<ROW_TYPE, int64_t,OP,NO_MATCH_SEL>(vec, col, vcount, layout, rows, sel, count, col_no, no_match, no_match_count);
		case ConstantType::UTINYINT:
			return templatedMatchOpSwitchCommon<ROW_TYPE, uint8_t,OP,NO_MATCH_SEL>(vec, col, vcount, layout, rows, sel, count, col_no, no_match, no_match_count);
		case ConstantType::USMALLINT:
			return templatedMatchOpSwitchCommon<ROW_TYPE, uint16_t,OP,NO_MATCH_SEL>(vec, col, vcount, layout, rows, sel, count, col_no, no_match, no_match_count);
		case ConstantType::UINTEGER:
			return templatedMatchOpSwitchCommon<ROW_TYPE, uint32_t,OP,NO_MATCH_SEL>(vec, col, vcount, layout, rows, sel, count, col_no, no_match, no_match_count);
		case ConstantType::UBIGINT:
			return templatedMatchOpSwitchCommon<ROW_TYPE, uint64_t,OP,NO_MATCH_SEL>(vec, col, vcount, layout, rows, sel, count, col_no, no_match, no_match_count);
		case ConstantType::FLOAT:
			return templatedMatchOpSwitchCommon<ROW_TYPE, float,OP,NO_MATCH_SEL>(vec, col, vcount, layout, rows, sel, count, col_no, no_match, no_match_count);
		case ConstantType::DOUBLE:
			return templatedMatchOpSwitchCommon<ROW_TYPE, double,OP,NO_MATCH_SEL>(vec, col, vcount, layout, rows, sel, count, col_no, no_match, no_match_count);

		default:
			ErrorHandler::errorNotImplemented("Unimplemented row equal for select operation!");
	}
}



template <class OP, bool NO_MATCH_SEL>
static void templatedMatchOpSwitchRow(Vector &vec, VectorData &col, const idx_t vcount, const RowLayout &layout, Vector &rows,
                             SelectionVector &sel, idx_t &count, idx_t col_no, SelectionVector *no_match,
                             idx_t &no_match_count) {
	BB_ASSERT(vec.getType() != ConstantType::STRING && layout.getTypes()[col_no] != ConstantType::STRING);

	if (count == 0) {
		return;
	}
	switch (layout.getTypes()[col_no]) {
		case ConstantType::TINYINT:
			return templatedMatchOpSwitchVec<int8_t,OP,NO_MATCH_SEL>(vec, col, vcount, layout, rows, sel, count, col_no, no_match, no_match_count);
		case ConstantType::SMALLINT:
			return templatedMatchOpSwitchVec<int16_t,OP,NO_MATCH_SEL>(vec, col, vcount, layout, rows, sel, count, col_no, no_match, no_match_count);
		case ConstantType::INTEGER:
			return templatedMatchOpSwitchVec<int32_t,OP,NO_MATCH_SEL>(vec, col, vcount, layout, rows, sel, count, col_no, no_match, no_match_count);
		case ConstantType::BIGINT:
			return templatedMatchOpSwitchVec<int64_t,OP,NO_MATCH_SEL>(vec, col, vcount, layout, rows, sel, count, col_no, no_match, no_match_count);
		case ConstantType::UTINYINT:
			return templatedMatchOpSwitchVec<uint8_t,OP,NO_MATCH_SEL>(vec, col, vcount, layout, rows, sel, count, col_no, no_match, no_match_count);
		case ConstantType::USMALLINT:
			return templatedMatchOpSwitchVec<uint16_t,OP,NO_MATCH_SEL>(vec, col, vcount, layout, rows, sel, count, col_no, no_match, no_match_count);
		case ConstantType::UINTEGER:
			return templatedMatchOpSwitchVec<uint32_t,OP,NO_MATCH_SEL>(vec, col, vcount, layout, rows, sel, count, col_no, no_match, no_match_count);
		case ConstantType::UBIGINT:
			return templatedMatchOpSwitchVec<uint64_t,OP,NO_MATCH_SEL>(vec, col, vcount, layout, rows, sel, count, col_no, no_match, no_match_count);
		case ConstantType::FLOAT:
			return templatedMatchOpSwitchVec<float,OP,NO_MATCH_SEL>(vec, col, vcount, layout, rows, sel, count, col_no, no_match, no_match_count);
		case ConstantType::DOUBLE:
			return templatedMatchOpSwitchVec<double,OP,NO_MATCH_SEL>(vec, col, vcount, layout, rows, sel, count, col_no, no_match, no_match_count);

		default:
			ErrorHandler::errorNotImplemented("Unimplemented row equal for select operation!");
	}
}

template <class T, class OP, bool NO_MATCH_SEL>
static void templatedMatchTypeEqualType(VectorData &col, Vector &rows, SelectionVector &sel, idx_t &count, idx_t col_offset,
								   idx_t col_no, SelectionVector *no_match, idx_t &no_match_count) {

	auto data = (T *)col.data_;
	auto ptrs = FlatVector::getData<data_ptr_t>(rows);
	idx_t match_count = 0;
	for (idx_t i = 0; i < count; i++) {
		auto idx = sel.getIndex(i);
		auto row = ptrs[idx];
		auto col_idx = col.sel_->getIndex(idx);
		auto value = load<T>(row + col_offset);
		if (OP::template operation<T>(data[col_idx], value)) {
			sel.setIndex(match_count++, idx);
		} else {
			if (NO_MATCH_SEL) {
				no_match->setIndex(no_match_count++, idx);
			}
		}
	}

	count = match_count;
}


template <class OP, bool NO_MATCH_SEL>
static void templatedMatchOp(Vector &vec, VectorData &col, const idx_t vcount, const RowLayout &layout, Vector &rows,
                             SelectionVector &sel, idx_t &count, idx_t col_no, SelectionVector *no_match,
                             idx_t &no_match_count) {
	if (count == 0) {
		return;
	}
	auto col_offset = layout.getOffsets()[col_no];
	switch (layout.getTypes()[col_no]) {
		case ConstantType::TINYINT:
			return templatedMatchTypeEqualType<int8_t,OP,NO_MATCH_SEL>(col, rows, sel, count, col_offset, col_no, no_match, no_match_count);
		case ConstantType::SMALLINT:
			return templatedMatchTypeEqualType<int16_t,OP,NO_MATCH_SEL>(col, rows, sel, count, col_offset, col_no, no_match, no_match_count);
		case ConstantType::INTEGER:
			return templatedMatchTypeEqualType<int32_t,OP,NO_MATCH_SEL>(col, rows, sel, count, col_offset, col_no, no_match, no_match_count);
		case ConstantType::BIGINT:
			return templatedMatchTypeEqualType<int64_t,OP,NO_MATCH_SEL>(col, rows, sel, count, col_offset, col_no, no_match, no_match_count);
		case ConstantType::UTINYINT:
			return templatedMatchTypeEqualType<uint8_t,OP,NO_MATCH_SEL>(col, rows, sel, count, col_offset, col_no, no_match, no_match_count);
		case ConstantType::USMALLINT:
			return templatedMatchTypeEqualType<uint16_t,OP,NO_MATCH_SEL>(col, rows, sel, count, col_offset, col_no, no_match, no_match_count);
		case ConstantType::UINTEGER:
			return templatedMatchTypeEqualType<uint32_t,OP,NO_MATCH_SEL>(col, rows, sel, count, col_offset, col_no, no_match, no_match_count);
		case ConstantType::UBIGINT:
			return templatedMatchTypeEqualType<uint64_t,OP,NO_MATCH_SEL>(col, rows, sel, count, col_offset, col_no, no_match, no_match_count);
		case ConstantType::FLOAT:
			return templatedMatchTypeEqualType<float,OP,NO_MATCH_SEL>(col, rows, sel, count, col_offset, col_no, no_match, no_match_count);
		case ConstantType::DOUBLE:
			return templatedMatchTypeEqualType<double,OP,NO_MATCH_SEL>(col, rows, sel, count, col_offset, col_no, no_match, no_match_count);
		case ConstantType::STRING:
			return templatedMatchTypeEqualType<string_t,OP,NO_MATCH_SEL>(col, rows, sel, count, col_offset, col_no, no_match, no_match_count);

		default:
			ErrorHandler::errorNotImplemented("Unimplemented row equal for select operation!");
	}
}

template <class OP, bool NO_MATCH_SEL>
static void templatedMatch(DataChunk &columns, VectorData col_data[], const RowLayout &layout, Vector &rows,
                            SelectionVector &sel, idx_t &count, SelectionVector *no_match,
                           idx_t &no_match_count) {
	const idx_t vcount = columns.getSize();
	for (idx_t col_no = 0; col_no < columns.columnCount(); ++col_no) {
		auto &vec = columns.data_[col_no];
		auto &col = col_data[col_no];
		if (layout.getTypes()[col_no] == vec.getType())
			templatedMatchOp<OP, NO_MATCH_SEL>(vec, col, vcount, layout, rows, sel, count, col_no, no_match,
				                                       no_match_count);
		else
			templatedMatchOpSwitchRow<OP, NO_MATCH_SEL>(vec, col, vcount, layout, rows, sel, count, col_no, no_match,
													   no_match_count);
	}
}

idx_t RowOperations::equal(DataChunk &columns, VectorData col_data[], const RowLayout &layout, Vector &rows,
                            SelectionVector &sel, idx_t count, SelectionVector *no_match,
                           idx_t &no_match_count) {
	if (count == 0) {
		no_match_count = 0;
		return 0;
	}
	if (no_match) {
		templatedMatch<Equals, true>(columns, col_data, layout, rows, sel, count, no_match, no_match_count);
	} else {
		templatedMatch<Equals, false>(columns, col_data, layout, rows, sel, count, no_match, no_match_count);
	}

	return count;
}

}