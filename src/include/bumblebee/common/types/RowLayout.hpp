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
#include "bumblebee/common/TypeDefs.hpp"
#include "bumblebee/common/Vector.hpp"

namespace bumblebee{

class AggregateFunction;

/*
 * Row layout:
 * [DATA] [HEAP POINTER] [AGGREGATES] | [DATA] [HEAP POINTER] [AGGREGATES] ...
*/
class RowLayout {
    using Aggregates = vector<AggregateFunction*>;
public:

    RowLayout();

    // Initializes the RowLayout with the specified types and aggregates to an empty RowLayout
    void initialize(vector<LogicalType> types_p, Aggregates aggregates, bool align = true);
    // Initializes the RowLayout with the specified types to an empty RowLayout
    void initialize(vector<LogicalType> types, bool align = true);
    // Initializes the RowLayout with the specified aggregates to an empty RowLayout
    void initialize(Aggregates aggregates_p, bool align = true);
    // Returns the number of data columns
    inline idx_t columnCount() const {
        return types_.size();
    }
    // Returns a list of the column types for this data chunk
    inline const vector<LogicalType> &getTypes() const {
        return types_;
    }
    // Returns the number of aggregates
    inline idx_t aggregateCount() const {
        return aggregates_.size();
    }
    // Returns a list of the aggregates for this data chunk
    inline Aggregates &getAggregates() {
        return aggregates_;
    }
    // Returns the total width required for each row, including padding
    inline idx_t getRowWidth() const {
        return rowWidth_;
    }
    // Returns the total width required for the data, including padding
    inline idx_t getDataWidth() const {
        return dataWidth_;
    }
    // Returns the offset to the start of the aggregates
    inline idx_t getAggrOffset() const {
        return dataWidth_;
    }
    // Returns the total width required for the aggregates, including padding
    inline idx_t getAggrWidth() const {
        return aggrWidth_;
    }
    // Returns the column offsets into each row
    inline const vector<idx_t> &getOffsets() const {
        return offsets_;
    }
    // Returns whether all columns in this layout are constant size
    inline bool allConstant() const {
        return allConstant_;
    }
    inline idx_t getHeapPointerOffset() const {
        return heapPointerOffset_;
    }

private:
    // The types of the data columns
    vector<LogicalType> types_;
    // The aggregate functions
    Aggregates aggregates_;
    // The width of the data portion
    idx_t dataWidth_;
    // The width of the aggregate state portion
    idx_t aggrWidth_;
    // The width of the entire row
    idx_t rowWidth_;
    // The offsets to the columns and aggregate data in each row
    vector<idx_t> offsets_;
    // Whether all columns in this layout are constant size
    bool allConstant_;
    // Offset to the pointer to the heap for each row
    idx_t heapPointerOffset_;
};


}
