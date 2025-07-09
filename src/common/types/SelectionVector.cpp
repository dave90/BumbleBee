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
#include "bumblebee/common/types/SelectionVector.h"

#include <string>

namespace bumblebee{
SelectionVector::SelectionVector() : sel_vector_(nullptr) {}

SelectionVector::SelectionVector(sel_t *sel) {
    initialize(sel);
}

SelectionVector::SelectionVector(idx_t count) {
    initialize(count);
}

SelectionVector::SelectionVector(idx_t start, idx_t count) {
    initialize(STANDARD_VECTOR_SIZE);
    for (unsigned i = 0; i < count; i++) {
        setIndex(i, start+i);
    }
}

SelectionVector::SelectionVector(const SelectionVector &sel_vector) {
    initialize(sel_vector);
}

SelectionVector::SelectionVector(sel_ptr_t data) {
    initialize(data);
}

void SelectionVector::initialize(sel_t *sel) {
    sel_vector_ = sel;
    sel_data_.reset();
}

void SelectionVector::initialize(idx_t count) {
    sel_data_ = sel_ptr_t(new sel_t[count]);
    sel_vector_ = sel_data_.get();
}

void SelectionVector::initialize(sel_ptr_t data) {
    sel_data_ = data;
}

void SelectionVector::initialize(const SelectionVector &other) {
    sel_data_ = other.sel_data_;
    sel_vector_ = other.sel_vector_;
}

void SelectionVector::setIndex(idx_t idx, idx_t loc) {
    sel_vector_[idx] = loc;
}

void SelectionVector::swap(idx_t i, idx_t j) {
    sel_t tmp = sel_vector_[i];
    sel_vector_[i] = sel_vector_[j];
    sel_vector_[j] = tmp;
}

idx_t SelectionVector::getIndex(idx_t idx) const{
    return sel_vector_?sel_vector_[idx]:idx;
}

sel_t * SelectionVector::getData() {
    return sel_vector_;
}

const sel_t * SelectionVector::getData() const {
    return sel_vector_;
}

sel_ptr_t SelectionVector::getSelData() const{
    return sel_data_;
}

sel_ptr_t SelectionVector::slice(const SelectionVector &sel, idx_t count) const {
    sel_ptr_t data = sel_ptr_t(new sel_t[count]);
    for (unsigned i = 0; i < count; i++) {
        auto idx = sel.getIndex(i);
        data[i] = this->getIndex(idx);
    }
    return data;
}

std::string SelectionVector::toString(idx_t count) {
    if (sel_vector_ == nullptr) return "";
    std::string r = "";
    for (unsigned i = 0; i < count; i++) {
        r += std::to_string(sel_vector_[i]) + ", ";
    }
    return r;
}

sel_t & SelectionVector::operator[](idx_t index) {
    return sel_vector_[index];
}
}
