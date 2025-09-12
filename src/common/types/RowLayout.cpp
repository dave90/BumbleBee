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
#include "bumblebee/common/types/RowLayout.hpp"

#include "bumblebee/common/Helper.hpp"
#include "bumblebee/function/AggregateFunction.hpp"

namespace bumblebee{
RowLayout::RowLayout(): dataWidth_(0), aggrWidth_(0), rowWidth_(0){
}

void RowLayout::initialize(vector<ConstantType> types, Aggregates aggregates, bool align) {
    offsets_.clear();
    types_ = std::move(types);

    rowWidth_ = 0;
    // Whether all columns are constant size.
    allConstant_ = true;
    for (const auto &type : types) {
        allConstant_ = allConstant_ && typeIsConstantSize(type);
    }

    // This enables pointer swizzling for out-of-core computation.
    if (!allConstant_) {
        // When unswizzled the pointer lives here.
        // When swizzled, the pointer is replaced by an offset.
        heapPointerOffset_ = rowWidth_;
        // The 8 byte pointer will be replaced with an 8 byte idx_t when swizzled.
        rowWidth_ += sizeof(idx_t);
    }

    // Data columns. No alignment required.
    for (const auto &type : types_) {
        offsets_.push_back(rowWidth_);
        rowWidth_ += getCTypeSize(type);
    }

    if (align) {
        rowWidth_ = alignValue(rowWidth_);
    }
    dataWidth_ = rowWidth_;

    // Aggregate fields.
    aggregates_ = std::move(aggregates);
    for (auto &aggregate : aggregates_) {
        offsets_.push_back(rowWidth_);
        auto payloadSize = aggregate->stateSize_();
        rowWidth_ += payloadSize;
    }
    aggrWidth_ = rowWidth_ - dataWidth_;

    // Alignment padding for the next row
    if (align) {
        rowWidth_ = alignValue(rowWidth_);
    }
}

void RowLayout::initialize(vector<ConstantType> types, bool align) {
    initialize(std::move(types),Aggregates(), align);
}

void RowLayout::initialize(Aggregates aggregates_p, bool align) {
    initialize(vector<ConstantType>(),std::move(aggregates_p),align);
}
}
