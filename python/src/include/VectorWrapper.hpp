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
#include "bumblebee/common/types/DataChunk.hpp"
#include "bumblebee/common/types/Vector.hpp"
#include "pybind11/numpy.h"

namespace bumblebee::python {


struct RawVectorWrapper {
    explicit RawVectorWrapper(const LogicalType &type);

    pybind11::array array_;
    data_ptr_t data_;
    LogicalType type_;
    idx_t width_;
    idx_t count_;

public:
    void initialize(idx_t capacity);
    void resize(idx_t new_capacity);
};

struct VectorWrapper {
    explicit VectorWrapper(const LogicalType &type);

    std::unique_ptr<RawVectorWrapper> data_;
    //TODO handle nulls

public:
    void initialize(idx_t capacity);
    void resize(idx_t new_capacity);
    void append(idx_t current_offset, Vector &input, idx_t count);
    pybind11::object toArray() const;
};

class NumpyResultConversion {
public:
    NumpyResultConversion(const vector<LogicalType> &types, idx_t initial_capacity);

    void append(DataChunk &chunk);

    pybind11::object toArray(idx_t col_idx) {
        return data_[col_idx].toArray();
    }

private:
    void resize(idx_t new_capacity);

private:
    vector<VectorWrapper> data_;
    idx_t count_;
    idx_t capacity_;
};

}
