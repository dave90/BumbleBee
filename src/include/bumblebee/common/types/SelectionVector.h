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
#include <unordered_map>

#include "bumblebee/common/Constants.h"
#include "bumblebee/common/TypeDefs.h"

namespace bumblebee{

using sel_ptr_t = std::shared_ptr<sel_t[]>;

// The SelectionVector class provides an abstraction for managing and manipulating selection vectors,
// which are commonly used in vectorized execution engines to represent filtered subsets of data.
// It allows efficient referencing and transformation of indices in a columnar data structure,
// supporting various initialization and modification operations.
//
// NOTE: SelectionVector is critical class -> all the inlined functions are defined in the .h
class SelectionVector {

public:
    SelectionVector() {}
    SelectionVector(sel_t *sel) {initialize(sel);}
    SelectionVector(idx_t count) {initialize(count);}
    SelectionVector(idx_t start, idx_t count) {
        initialize(STANDARD_VECTOR_SIZE);
        for (unsigned i = 0; i < count; i++) {
            setIndex(i, start+i);
        }
    }
    SelectionVector(const SelectionVector &sel_vector) {initialize(sel_vector);}
    SelectionVector(sel_ptr_t data) {initialize(data);}



    inline void initialize(sel_t *sel) {
        sel_vector_ = sel;
        sel_data_.reset();
    }
    inline void initialize(idx_t count) {
        sel_data_ = sel_ptr_t(new sel_t[count]);
        sel_vector_ = sel_data_.get();
    }
    inline void initialize(sel_ptr_t data) {
        sel_data_ = data;
    }
    inline void initialize(const SelectionVector &other) {
        sel_data_ = other.sel_data_;
        sel_vector_ = other.sel_vector_;
    }
    inline void setIndex(idx_t idx, idx_t loc) {
        sel_vector_[idx] = loc;
    }
    inline void swap(idx_t i, idx_t j) {
        sel_t tmp = sel_vector_[i];
        sel_vector_[i] = sel_vector_[j];
        sel_vector_[j] = tmp;
    }
    inline sel_t * getData() {
        return sel_vector_;
    }
    inline const sel_t * getData() const {
        return sel_vector_;
    }
    inline sel_ptr_t getSelData() const{
        return sel_data_;
    }

    // if sel_vector point to null return the index
    inline idx_t getIndex(idx_t idx) const{
        return sel_vector_?sel_vector_[idx]:idx;
    }
    sel_ptr_t slice(const SelectionVector &sel, idx_t count) const;
    std::string toString(idx_t count = 0);


    inline sel_t & operator[](idx_t index) {
        return sel_vector_[index];
    }
private:
    sel_t* sel_vector_{nullptr};
    sel_ptr_t sel_data_;
};

}
