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
#include <memory>

#include "bumblebee/common/Constants.h"
#include "bumblebee/common/TypeDefs.h"

namespace bumblebee{

using sel_ptr_t = std::shared_ptr<sel_t[]>;

// The SelectionVector class provides an abstraction for managing and manipulating selection vectors,
// which are commonly used in vectorized execution engines to represent filtered subsets of data.
// It allows efficient referencing and transformation of indices in a columnar data structure,
// supporting various initialization and modification operations.
class SelectionVector {

public:
    SelectionVector();
    explicit SelectionVector(sel_t *sel);
    explicit SelectionVector(idx_t count);
    SelectionVector(idx_t start, idx_t count); // init the selection vector i -> start + i
    SelectionVector(const SelectionVector &sel_vector);
    explicit SelectionVector(sel_ptr_t data);

    void initialize(sel_t *sel);
    void initialize(idx_t count = STANDARD_VECTOR_SIZE);
    void initialize(sel_ptr_t data);
    void initialize(const SelectionVector &other);

    void setIndex(idx_t idx, idx_t loc);
    void swap(idx_t i, idx_t j);
    idx_t getIndex(idx_t idx) const; // if sel_vector point to null return the index
    sel_t * getData() ;
    const sel_t * getData() const;
    sel_ptr_t getSelData() const;
    sel_ptr_t slice(const SelectionVector &sel, idx_t count) const;

    std::string toString(idx_t count = 0);

    sel_t &operator[](idx_t index);

private:
    sel_t* sel_vector_;
    sel_ptr_t sel_data_;
};


}
