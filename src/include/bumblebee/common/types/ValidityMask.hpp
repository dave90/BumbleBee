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
#include <cstdint>
#include <memory>

#include "Assert.hpp"
#include "bumblebee/common/Constants.hpp"
#include "bumblebee/common/TypeDefs.hpp"

namespace bumblebee{

class SelectionVector;

// Per-value validity bitmask. A set bit (1) means the row is VALID
// (i.e. NOT null). A null/empty buffer means "all valid" — zero memory, zero overhead,
// which is the common case. The buffer is allocated lazily on the first write of a null
// via ensureWritable().
//
// The backing buffer is held by a shared_ptr so that copying a mask (e.g. when a Vector
// references another) shares the buffer cheaply, matching the "referenced vectors share
// their data" model. slice()/gatherFrom() always produce an independent buffer.
//
// NOTE: critical class -> the hot-path accessors are inlined here.
class ValidityMask {
public:
    static constexpr idx_t BITS_PER_WORD = 64;

    ValidityMask() = default;
    ValidityMask(const ValidityMask &other) = default;            // shallow: shares the buffer
    ValidityMask &operator=(const ValidityMask &other) = default; // shallow: shares the buffer
    ValidityMask(ValidityMask &&) noexcept = default;
    ValidityMask &operator=(ValidityMask &&) noexcept = default;

    // Deep copy: allocate an independent buffer holding the same bits.
    ValidityMask copy() const;

    // True when the buffer is absent, i.e. every row is valid.
    inline bool allValid() const { return mask_ == nullptr; }

    inline const uint64_t *data() const { return mask_; }
    inline uint64_t *data() { return mask_; }

    // Row is valid (not null). All-valid fast path: no buffer means everything is valid.
    // An idx beyond the allocated buffer is treated as valid (the buffer was only ever
    // grown by setInvalid up to whatever index the caller passed; rows past the high-water
    // mark have not been observed as null and therefore default to valid).
    inline bool rowIsValid(idx_t idx) const {
        if (!mask_) return true;
        if (idx / BITS_PER_WORD >= capacityWords_) return true;
        return (mask_[idx / BITS_PER_WORD] & (uint64_t(1) << (idx % BITS_PER_WORD))) != 0;
    }

    // Mark a row valid. No-op when all-valid (nothing is null yet) or when the row is
    // past the allocated buffer (which would already report valid via rowIsValid above).
    inline void setValid(idx_t idx) {
        if (!mask_) return;
        if (idx / BITS_PER_WORD >= capacityWords_) return;
        mask_[idx / BITS_PER_WORD] |= (uint64_t(1) << (idx % BITS_PER_WORD));
    }

    // Mark a row invalid (null). Lazily allocates the buffer on first use.
    inline void setInvalid(idx_t idx) {
        ensureWritable(idx + 1);
        // invariant: ensureWritable must have grown the buffer to cover idx
        BB_ASSERT(mask_ && idx / BITS_PER_WORD < capacityWords_);
        mask_[idx / BITS_PER_WORD] &= ~(uint64_t(1) << (idx % BITS_PER_WORD));
    }

    // DEBUG invariant: a present buffer is owned and large enough for `count` rows.
    inline bool debugConsistent(idx_t count) const {
        if (!mask_) return true;
        return mask_ == ownedData_.get() && capacityWords_ >= wordCount(count);
    }

    inline void set(idx_t idx, bool valid) {
        if (valid) setValid(idx); else setInvalid(idx);
    }

    // Unchecked WRITE accessors. Precondition: the writer called ensureWritable(count) once for
    // its full row count before the loop. No unsafe read exists: an input mask may be sized
    // below its vector (grown only to its last null), so reads keep the bounds-checked rowIsValid.
    inline void setValidUnsafe(idx_t idx) {
        BB_ASSERT(mask_ && idx / BITS_PER_WORD < capacityWords_);
        mask_[idx / BITS_PER_WORD] |= (uint64_t(1) << (idx % BITS_PER_WORD));
    }
    inline void setInvalidUnsafe(idx_t idx) {
        BB_ASSERT(mask_ && idx / BITS_PER_WORD < capacityWords_);
        mask_[idx / BITS_PER_WORD] &= ~(uint64_t(1) << (idx % BITS_PER_WORD));
    }

    // Drop the buffer: the mask becomes all-valid again.
    void setAllValid();
    // Allocate (if needed) and clear all bits in [0, count): every row becomes null.
    void setAllInvalid(idx_t count);
    inline void reset() { setAllValid(); }

    // Lazily allocate an all-ones (all-valid) buffer large enough to address `capacity` rows.
    void ensureWritable(idx_t capacity = STANDARD_VECTOR_SIZE);

    // True if there is no null in [0, count).
    bool checkAllValid(idx_t count) const;

    // Make this an owned copy of bits [offset, offset+count) of `other`.
    void slice(const ValidityMask &other, idx_t offset, idx_t count = STANDARD_VECTOR_SIZE);

    // Gather through a selection: this[i] = other.rowIsValid(sel[i]) for i in [0, count).
    void gatherFrom(const ValidityMask &other, const SelectionVector &sel, idx_t count);

    // NULL propagation (logical AND): this[i] is valid only if both masks are valid.
    void combine(const ValidityMask &other, idx_t count);

private:
    static inline idx_t wordCount(idx_t capacity) {
        return (capacity + BITS_PER_WORD - 1) / BITS_PER_WORD;
    }

    // All-ones-initialized buffer (shared on copy). mask_ points into it (or is null => all valid).
    std::shared_ptr<uint64_t[]> ownedData_;
    uint64_t *mask_{nullptr};
    idx_t capacityWords_{0};
};

}