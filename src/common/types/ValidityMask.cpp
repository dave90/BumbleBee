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
#include "bumblebee/common/types/ValidityMask.hpp"

#include <cstring>

#include "bumblebee/common/Helper.hpp"
#include "bumblebee/common/types/SelectionVector.hpp"

namespace bumblebee{

void ValidityMask::ensureWritable(idx_t capacity) {
    // Always cover at least a full standard vector so that bulk readers (slice/copy)
    // never run past the buffer for a normal-sized vector.
    if (capacity < STANDARD_VECTOR_SIZE) capacity = STANDARD_VECTOR_SIZE;
    idx_t words = wordCount(capacity);
    if (mask_ && capacityWords_ >= words) {
        // already allocated and large enough
        return;
    }
    // (re)allocate an all-ones (all-valid) buffer, preserving existing bits
    auto newData = std::shared_ptr<uint64_t[]>(new uint64_t[words]);
    std::memset(newData.get(), 0xFF, words * sizeof(uint64_t));
    if (mask_) {
        // preserve previously written bits
        std::memcpy(newData.get(), mask_, capacityWords_ * sizeof(uint64_t));
    }
    ownedData_ = std::move(newData);
    mask_ = ownedData_.get();
    capacityWords_ = words;
}

void ValidityMask::setAllValid() {
    ownedData_.reset();
    mask_ = nullptr;
    capacityWords_ = 0;
}

void ValidityMask::setAllInvalid(idx_t count) {
    ensureWritable(count);
    std::memset(mask_, 0x00, capacityWords_ * sizeof(uint64_t));
}

bool ValidityMask::checkAllValid(idx_t count) const {
    if (!mask_) return true;
    for (idx_t i = 0; i < count; i++) {
        if (!rowIsValid(i)) return false;
    }
    return true;
}

ValidityMask ValidityMask::copy() const {
    ValidityMask result;
    if (mask_) {
        result.ensureWritable(capacityWords_ * BITS_PER_WORD);
        std::memcpy(result.mask_, mask_, capacityWords_ * sizeof(uint64_t));
    }
    return result;
}

void ValidityMask::slice(const ValidityMask &other, idx_t offset, idx_t count) {
    // Produce an independent buffer (do not alias a shared mask).
    setAllValid();
    if (other.allValid()) {
        return;
    }
    // bit offsets are generally not word-aligned: materialize an owned copy.
    // Bound the read by the source's covered range; rows past it are implicitly valid.
    ensureWritable(count);
    idx_t srcBits = other.capacityWords_ * BITS_PER_WORD;
    idx_t limit = (offset < srcBits) ? minValue(count, srcBits - offset) : 0;
    for (idx_t i = 0; i < limit; i++) {
        set(i, other.rowIsValid(offset + i));
    }
}

void ValidityMask::gatherFrom(const ValidityMask &other, const SelectionVector &sel, idx_t count) {
    // Produce an independent buffer (do not alias a shared mask).
    setAllValid();
    if (other.allValid()) {
        // nothing to gather: leave this all-valid
        return;
    }
    ensureWritable(count);
    for (idx_t i = 0; i < count; i++) {
        set(i, other.rowIsValid(sel.getIndex(i)));
    }
}

void ValidityMask::combine(const ValidityMask &other, idx_t count) {
    if (other.allValid()) {
        return;
    }
    ensureWritable(count);
    for (idx_t i = 0; i < count; i++) {
        if (!other.rowIsValid(i)) {
            setInvalid(i);
        }
    }
}

}