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

// Resolve a match given the per-side NULL flags. NOT_DISTINCT collapses at compile time
// so the join hot path (NULL-excludes) and the dedup path (IS-NOT-DISTINCT-FROM) each
// reduce to straight-line code with no extra branches.
template <bool NOT_DISTINCT, class OP, class L, class R>
static inline bool matchOrNull(bool col_null, bool row_null, L& lhs, R& rhs) {
	if constexpr (NOT_DISTINCT) {
		// IS-NOT-DISTINCT-FROM: two NULLs match; exactly-one-NULL never matches.
		if (col_null || row_null) return col_null && row_null;
	} else {
		// NULL-excludes: any NULL → no match (the SQL "unknown" result is treated as false).
		if (col_null || row_null) return false;
	}
	return OP::operation(lhs, rhs);
}

template <class ROW_TYPE, class VEC_TYPE, class OP, bool NO_MATCH_SEL, bool NOT_DISTINCT>
static void templatedMatchType(VectorData &col, Vector &rows, SelectionVector &sel, idx_t &count, idx_t col_offset,
							   idx_t col_no, SelectionVector *no_match, idx_t &no_match_count) {

	auto data = (VEC_TYPE *)col.data_;
	auto ptrs = FlatVector::getData<data_ptr_t>(rows);
	const auto col_all_valid = !col.validity_ || col.validity_->allValid();
	idx_t match_count = 0;
	for (idx_t i = 0; i < count; i++) {
		auto idx = sel.getIndex(i);
		auto row = ptrs[idx];
		auto col_idx = col.sel_->getIndex(idx);
		auto value = load<ROW_TYPE>(row + col_offset);
		const bool col_null = !col_all_valid && !col.validity_->rowIsValid(col_idx);
		const bool row_null = !rowIsValid(row, col_no);
		if (matchOrNull<NOT_DISTINCT, OP>(col_null, row_null, data[col_idx], value)) {
			sel.setIndex(match_count++, idx);
		} else {
			if (NO_MATCH_SEL) {
				no_match->setIndex(no_match_count++, idx);
			}
		}
	}

	count = match_count;
}



template <class ROW_TYPE, class VEC_TYPE, class OP, bool NO_MATCH_SEL, bool NOT_DISTINCT>
static void templatedMatchTypeSelection(VectorData &col, Vector &rows, SelectionVector &col_sel,SelectionVector &row_sel, idx_t &count, idx_t col_offset,
								   idx_t col_no, SelectionVector *no_match, idx_t &no_match_count) {

	auto data = (VEC_TYPE *)col.data_;
	auto ptrs = FlatVector::getData<data_ptr_t>(rows);
	const auto col_all_valid = !col.validity_ || col.validity_->allValid();
	idx_t match_count = 0;
	for (idx_t i = 0; i < count; i++) {
		auto cidx = col_sel.getIndex(i);
		auto ridx = row_sel.getIndex(i);
		auto row = ptrs[ridx];
		auto col_idx = col.sel_->getIndex(cidx);
		auto value = load<ROW_TYPE>(row + col_offset);
		const bool col_null = !col_all_valid && !col.validity_->rowIsValid(col_idx);
		const bool row_null = !rowIsValid(row, col_no);
		if (matchOrNull<NOT_DISTINCT, OP>(col_null, row_null, data[col_idx], value)) {
			col_sel.setIndex(match_count, cidx);
			row_sel.setIndex(match_count++, ridx);
		} else {
			if (NO_MATCH_SEL) {
				no_match->setIndex(no_match_count++, i);
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

template <class ROW_TYPE, class VEC_TYPE, class OP, bool NO_MATCH_SEL, bool HAS_ROW_SEL, bool NOT_DISTINCT>
static void templatedMatchOpSwitchCommon(Vector &vec, VectorData &col, const idx_t vcount, const RowLayout &layout, Vector &rows,
							 SelectionVector &sel,SelectionVector *row_sel, idx_t &count, idx_t col_no, SelectionVector *no_match,
							 idx_t &no_match_count) {
	auto col_offset = layout.getOffsets()[col_no];
	auto commonType = getCommonType(vec.getType(), layout.getTypes()[col_no].getPhysicalType());

	// for int and uint collapse to int 64 bit
	// for decimal collapse to double
	switch (commonType.getPhysicalType()) {
		case PhysicalType::TINYINT:
		case PhysicalType::SMALLINT:
		case PhysicalType::INTEGER:
		case PhysicalType::UTINYINT:
		case PhysicalType::USMALLINT:
		case PhysicalType::UINTEGER:
		case PhysicalType::UBIGINT:
		case PhysicalType::BIGINT: {
			if (!HAS_ROW_SEL)
				templatedMatchType<ROW_TYPE,VEC_TYPE, ComparisonCommonCast<ROW_TYPE, VEC_TYPE, int64_t, OP>, NO_MATCH_SEL, NOT_DISTINCT>(col, rows, sel, count, col_offset, col_no, no_match, no_match_count);
			else
				templatedMatchTypeSelection<ROW_TYPE,VEC_TYPE, ComparisonCommonCast<ROW_TYPE, VEC_TYPE, int64_t, OP>, NO_MATCH_SEL, NOT_DISTINCT>(col, rows, sel, *row_sel, count, col_offset, col_no, no_match, no_match_count);

			break;
		}case PhysicalType::FLOAT:
		case PhysicalType::DOUBLE:
			if (!HAS_ROW_SEL)
				templatedMatchType<ROW_TYPE,VEC_TYPE, ComparisonCommonCast<ROW_TYPE, VEC_TYPE, double, OP>, NO_MATCH_SEL, NOT_DISTINCT>(col, rows, sel, count, col_offset, col_no, no_match, no_match_count);
			else
				templatedMatchTypeSelection<ROW_TYPE,VEC_TYPE, ComparisonCommonCast<ROW_TYPE, VEC_TYPE, double, OP>, NO_MATCH_SEL, NOT_DISTINCT>(col, rows, sel, *row_sel, count, col_offset, col_no, no_match, no_match_count);

			break;
		default:
			ErrorHandler::errorNotImplemented("Unimplemented row equal for select operation!");
	}
}

template <class ROW_TYPE, class OP, bool NO_MATCH_SEL, bool HAS_ROW_SEL, bool NOT_DISTINCT>
static void templatedMatchOpSwitchVec(Vector &vec, VectorData &col, const idx_t vcount, const RowLayout &layout, Vector &rows,
							 SelectionVector &sel,SelectionVector *row_sel, idx_t &count, idx_t col_no, SelectionVector *no_match,
							 idx_t &no_match_count) {

	switch (vec.getType()) {
		case PhysicalType::TINYINT:
			return templatedMatchOpSwitchCommon<ROW_TYPE, int8_t,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(vec, col, vcount, layout, rows, sel, row_sel, count, col_no, no_match, no_match_count);
		case PhysicalType::SMALLINT:
			return templatedMatchOpSwitchCommon<ROW_TYPE, int16_t,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(vec, col, vcount, layout, rows, sel, row_sel, count, col_no, no_match, no_match_count);
		case PhysicalType::INTEGER:
			return templatedMatchOpSwitchCommon<ROW_TYPE, int32_t,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(vec, col, vcount, layout, rows, sel, row_sel, count, col_no, no_match, no_match_count);
		case PhysicalType::BIGINT:
			return templatedMatchOpSwitchCommon<ROW_TYPE, int64_t,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(vec, col, vcount, layout, rows, sel, row_sel, count, col_no, no_match, no_match_count);
		case PhysicalType::UTINYINT:
			return templatedMatchOpSwitchCommon<ROW_TYPE, uint8_t,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(vec, col, vcount, layout, rows, sel, row_sel, count, col_no, no_match, no_match_count);
		case PhysicalType::USMALLINT:
			return templatedMatchOpSwitchCommon<ROW_TYPE, uint16_t,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(vec, col, vcount, layout, rows, sel, row_sel, count, col_no, no_match, no_match_count);
		case PhysicalType::UINTEGER:
			return templatedMatchOpSwitchCommon<ROW_TYPE, uint32_t,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(vec, col, vcount, layout, rows, sel, row_sel, count, col_no, no_match, no_match_count);
		case PhysicalType::UBIGINT:
			return templatedMatchOpSwitchCommon<ROW_TYPE, uint64_t,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(vec, col, vcount, layout, rows, sel, row_sel, count, col_no, no_match, no_match_count);
		case PhysicalType::FLOAT:
			return templatedMatchOpSwitchCommon<ROW_TYPE, float,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(vec, col, vcount, layout, rows, sel, row_sel, count, col_no, no_match, no_match_count);
		case PhysicalType::DOUBLE:
			return templatedMatchOpSwitchCommon<ROW_TYPE, double,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(vec, col, vcount, layout, rows, sel, row_sel, count, col_no, no_match, no_match_count);

		default:
			ErrorHandler::errorNotImplemented("Unimplemented row equal for select operation!");
	}
}



template <class OP, bool NO_MATCH_SEL, bool HAS_ROW_SEL, bool NOT_DISTINCT>
static void templatedMatchOpSwitchRow(Vector &vec, VectorData &col, const idx_t vcount, const RowLayout &layout, Vector &rows,
                             SelectionVector &sel,SelectionVector *row_sel, idx_t &count, idx_t col_no, SelectionVector *no_match,
                             idx_t &no_match_count) {
	BB_ASSERT(vec.getType() != PhysicalType::STRING && layout.getTypes()[col_no] != PhysicalType::STRING);

	if (count == 0) {
		return;
	}
	switch (layout.getTypes()[col_no].getPhysicalType()) {
		case PhysicalType::TINYINT:
			return templatedMatchOpSwitchVec<int8_t,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(vec, col, vcount, layout, rows, sel, row_sel, count, col_no, no_match, no_match_count);
		case PhysicalType::SMALLINT:
			return templatedMatchOpSwitchVec<int16_t,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(vec, col, vcount, layout, rows, sel, row_sel, count, col_no, no_match, no_match_count);
		case PhysicalType::INTEGER:
			return templatedMatchOpSwitchVec<int32_t,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(vec, col, vcount, layout, rows, sel, row_sel, count, col_no, no_match, no_match_count);
		case PhysicalType::BIGINT:
			return templatedMatchOpSwitchVec<int64_t,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(vec, col, vcount, layout, rows, sel, row_sel, count, col_no, no_match, no_match_count);
		case PhysicalType::UTINYINT:
			return templatedMatchOpSwitchVec<uint8_t,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(vec, col, vcount, layout, rows, sel, row_sel, count, col_no, no_match, no_match_count);
		case PhysicalType::USMALLINT:
			return templatedMatchOpSwitchVec<uint16_t,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(vec, col, vcount, layout, rows, sel, row_sel, count, col_no, no_match, no_match_count);
		case PhysicalType::UINTEGER:
			return templatedMatchOpSwitchVec<uint32_t,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(vec, col, vcount, layout, rows, sel, row_sel, count, col_no, no_match, no_match_count);
		case PhysicalType::UBIGINT:
			return templatedMatchOpSwitchVec<uint64_t,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(vec, col, vcount, layout, rows, sel, row_sel, count, col_no, no_match, no_match_count);
		case PhysicalType::FLOAT:
			return templatedMatchOpSwitchVec<float,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(vec, col, vcount, layout, rows, sel, row_sel, count, col_no, no_match, no_match_count);
		case PhysicalType::DOUBLE:
			return templatedMatchOpSwitchVec<double,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(vec, col, vcount, layout, rows, sel, row_sel, count, col_no, no_match, no_match_count);

		default:
			ErrorHandler::errorNotImplemented("Unimplemented row equal for select operation!");
	}
}


template <class T, class OP, bool NO_MATCH_SEL, bool HAS_ROW_SEL, bool NOT_DISTINCT>
static void templatedMatchTypeEqualType(VectorData &col, Vector &rows, SelectionVector &sel,SelectionVector *row_sel, idx_t &count, idx_t col_offset,
								   idx_t col_no, SelectionVector *no_match, idx_t &no_match_count) {
	if (!HAS_ROW_SEL)
		templatedMatchType<T,T,OP,NO_MATCH_SEL,NOT_DISTINCT>(col, rows, sel, count, col_offset, col_no, no_match, no_match_count);
	else
		templatedMatchTypeSelection<T,T,OP,NO_MATCH_SEL,NOT_DISTINCT>(col, rows, sel, *row_sel, count, col_offset, col_no, no_match, no_match_count);

}

template <class OP, bool NO_MATCH_SEL, bool HAS_ROW_SEL, bool NOT_DISTINCT>
static void templatedMatchOp(Vector &vec, VectorData &col, const idx_t vcount, const RowLayout &layout, Vector &rows,
                             SelectionVector &sel,SelectionVector *row_sel, idx_t &count, idx_t col_no, SelectionVector *no_match,
                             idx_t &no_match_count) {
	if (count == 0) {
		return;
	}
	auto col_offset = layout.getOffsets()[col_no];
	switch (layout.getTypes()[col_no].getPhysicalType()) {
		case PhysicalType::TINYINT:
			return templatedMatchTypeEqualType<int8_t,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(col, rows, sel, row_sel, count, col_offset, col_no, no_match, no_match_count);
		case PhysicalType::SMALLINT:
			return templatedMatchTypeEqualType<int16_t,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(col, rows, sel, row_sel, count, col_offset, col_no, no_match, no_match_count);
		case PhysicalType::INTEGER:
			return templatedMatchTypeEqualType<int32_t,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(col, rows, sel, row_sel, count, col_offset, col_no, no_match, no_match_count);
		case PhysicalType::BIGINT:
			return templatedMatchTypeEqualType<int64_t,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(col, rows, sel, row_sel, count, col_offset, col_no, no_match, no_match_count);
		case PhysicalType::UTINYINT:
			return templatedMatchTypeEqualType<uint8_t,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(col, rows, sel, row_sel, count, col_offset, col_no, no_match, no_match_count);
		case PhysicalType::USMALLINT:
			return templatedMatchTypeEqualType<uint16_t,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(col, rows, sel, row_sel, count, col_offset, col_no, no_match, no_match_count);
		case PhysicalType::UINTEGER:
			return templatedMatchTypeEqualType<uint32_t,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(col, rows, sel, row_sel, count, col_offset, col_no, no_match, no_match_count);
		case PhysicalType::UBIGINT:
			return templatedMatchTypeEqualType<uint64_t,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(col, rows, sel, row_sel, count, col_offset, col_no, no_match, no_match_count);
		case PhysicalType::FLOAT:
			return templatedMatchTypeEqualType<float,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(col, rows, sel, row_sel, count, col_offset, col_no, no_match, no_match_count);
		case PhysicalType::DOUBLE:
			return templatedMatchTypeEqualType<double,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(col, rows, sel, row_sel, count, col_offset, col_no, no_match, no_match_count);
		case PhysicalType::STRING:
			return templatedMatchTypeEqualType<string_t,OP,NO_MATCH_SEL,HAS_ROW_SEL,NOT_DISTINCT>(col, rows, sel, row_sel, count, col_offset, col_no, no_match, no_match_count);

		default:
			ErrorHandler::errorNotImplemented("Unimplemented row equal for select operation!");
	}
}

template <class OP, bool NO_MATCH_SEL, bool HAS_ROW_SEL, bool NOT_DISTINCT>
static void templatedMatch(DataChunk &columns, VectorData col_data[], const RowLayout &layout, Vector &rows,
                            SelectionVector &sel,SelectionVector *row_sel,  idx_t &count, SelectionVector *no_match,
                           idx_t &no_match_count) {
	const idx_t vcount = columns.getSize();
	for (idx_t col_no = 0; col_no < columns.columnCount(); ++col_no) {
		auto &vec = columns.data_[col_no];
		auto &col = col_data[col_no];
		if (layout.getTypes()[col_no] == vec.getType())
			templatedMatchOp<OP, NO_MATCH_SEL, HAS_ROW_SEL, NOT_DISTINCT>(vec, col, vcount, layout, rows, sel, row_sel, count, col_no, no_match,
				                                       no_match_count);
		else
			templatedMatchOpSwitchRow<OP, NO_MATCH_SEL, HAS_ROW_SEL, NOT_DISTINCT>(vec, col, vcount, layout, rows, sel, row_sel, count, col_no, no_match,
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
		templatedMatch<Equals, true, false, false>(columns, col_data, layout, rows, sel, nullptr, count, no_match, no_match_count);
	} else {
		templatedMatch<Equals, false, false, false>(columns, col_data, layout, rows, sel,nullptr, count, no_match, no_match_count);
	}

	return count;
}

idx_t RowOperations::equal(DataChunk &columns, VectorData col_data[], const RowLayout &layout, Vector &rows,
							SelectionVector &chunkSel,SelectionVector &rowSel, idx_t count, SelectionVector *no_match,idx_t &no_match_count) {
	if (count == 0) {
		no_match_count = 0;
		return 0;
	}
	if (no_match) {
		templatedMatch<Equals, true, true, false>(columns, col_data, layout, rows, chunkSel, &rowSel, count, no_match, no_match_count);
	} else {
		templatedMatch<Equals, false, true, false>(columns, col_data, layout, rows, chunkSel,&rowSel, count, no_match, no_match_count);
	}

	return count;
}

// IS-NOT-DISTINCT-FROM equality: two NULLs in the same column are considered equal.
// Required by dedup / GROUP BY / recursion-fixed-point so null-bearing tuples are deduplicated.
idx_t RowOperations::notDistinctFrom(DataChunk &columns, VectorData col_data[], const RowLayout &layout, Vector &rows,
                            SelectionVector &sel, idx_t count, SelectionVector *no_match,
                           idx_t &no_match_count) {
	if (count == 0) {
		no_match_count = 0;
		return 0;
	}
	if (no_match) {
		templatedMatch<Equals, true, false, true>(columns, col_data, layout, rows, sel, nullptr, count, no_match, no_match_count);
	} else {
		templatedMatch<Equals, false, false, true>(columns, col_data, layout, rows, sel, nullptr, count, no_match, no_match_count);
	}

	return count;
}

}