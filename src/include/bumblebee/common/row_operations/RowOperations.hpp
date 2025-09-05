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
                       idx_t count, idx_t col_offset, idx_t col_no, idx_t build_size = 0);
    // // Full Scan an entire columns
    static void fullScanColumn(const RowLayout &layout, Vector &rows, Vector &col, idx_t count, idx_t col_idx);


};


}
