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
#pragma once
#include <cstring>

#include "bumblebee/common/types/RowLayout.hpp"
#include "bumblebee/common/types/Vector.hpp"

namespace bumblebee{
    class RowDataCollection;
    struct VectorData;
    class DataChunk;


struct RowOperations {

    //===--------------------------------------------------------------------===//
    // Read/Write Operators
    //===--------------------------------------------------------------------===//
    // Scatter group data to the rows. Initialises the ValidityMask.
    static void scatter(DataChunk &columns, VectorData col_data[], const RowLayout &layout, Vector &rows,
                        RowDataCollection &string_heap, const SelectionVector &sel, idx_t count);
    // Gather a single column. 
    static void gather(Vector &rows, const SelectionVector &row_sel, Vector &col, const SelectionVector &col_sel,
                       idx_t count, idx_t col_offset, idx_t col_no);
    // // Full Scan an entire columns
    static void fullScanColumn(const RowLayout &layout, Vector &rows, Vector &col, idx_t count, idx_t col_idx);


    //===--------------------------------------------------------------------===//
    // Comparison Operators
    //===--------------------------------------------------------------------===//
    // NULL-excludes equality: a NULL key on either side never matches. Used by all
    // 4 join hash tables (decision #1 of the NULL plan).
    static idx_t equal(DataChunk &columns, VectorData col_data[], const RowLayout &layout, Vector &rows,
                        SelectionVector &sel, idx_t count, SelectionVector *no_match,idx_t &no_match_count);
    static idx_t equal(DataChunk &columns, VectorData col_data[], const RowLayout &layout, Vector &rows,
                        SelectionVector &chunkSel,SelectionVector &rowSel, idx_t count, SelectionVector *no_match,idx_t &no_match_count);

    // IS-NOT-DISTINCT-FROM equality: two NULLs in the same column are considered equal.
    // Used by the 3 dedup/group hash tables (recursion fixed-point, GROUP BY, DISTINCT).
    static idx_t notDistinctFrom(DataChunk &columns, VectorData col_data[], const RowLayout &layout, Vector &rows,
                        SelectionVector &sel, idx_t count, SelectionVector *no_match,idx_t &no_match_count);


};

//===--------------------------------------------------------------------===//
// Inline helpers for the per-row validity bitmap prefix (row offset 0)
//===--------------------------------------------------------------------===//
// Bit = 1 means valid (not NULL), matching ValidityMask convention.

// Mark every column in this row as valid (set every prefix byte to 0xFF).
inline void rowSetAllValid(data_ptr_t row, idx_t flagWidth) {
    if (flagWidth) std::memset(row, 0xFF, flagWidth);
}
// Clear bit col_no in the row's validity prefix (marks the column NULL).
// >> 3 : integer division by 8 trick, & 7 is modulo division by 8
inline void rowSetInvalid(data_ptr_t row, idx_t col_no) {
    row[col_no >> 3] &= ~(uint8_t(1) << (col_no & 7));
}
// True when column col_no of this row is valid (not NULL).
// >> 3 : integer division by 8 trick, & 7 is modulo division by 8
inline bool rowIsValid(const_data_ptr_t row, idx_t col_no) {
    return (row[col_no >> 3] & (uint8_t(1) << (col_no & 7))) != 0;
}

}
