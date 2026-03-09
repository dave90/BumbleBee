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
#include "bumblebee/common/types/StringHeap.hpp"

#include "bumblebee/common/Constants.hpp"
#include "bumblebee/common/types/Assert.hpp"

namespace bumblebee{

string_t StringHeap::addBlob(const char *data, idx_t len) {
    BB_ASSERT(len <= MINIMUM_HEAP_SIZE && "String too large for chunk");
    auto newString = addEmptyString(len);
    memcpy(newString.getDataWriteable(), data, len);
    newString.getDataWriteable()[len] = '\0';
    return newString;
}


string_t StringHeap::addEmptyString(idx_t len) {
    BB_ASSERT(len <= MINIMUM_HEAP_SIZE && "String too large for chunk");
    if (!chunk_ || chunk_->current_position_ + len +1 >= chunk_->maximum_size_) {
        // create a new chunk
        // NOTE: Max string len supported is MINIMUM_HEAP_SIZE
        auto newChunk = std::make_unique<StringChunk>(MINIMUM_HEAP_SIZE);
        if (chunk_)
            newChunk->prev_ = std::move(chunk_);

        chunk_ = std::move(newChunk);
    }
    // create a new string
    char * dataPtr = chunk_->data_.get() + chunk_->current_position_;
    chunk_->current_position_ += len +1;// +1 for null termination
    //add null termination
    dataPtr[0] = '\0';
    return string_t(dataPtr, len);
}


}
