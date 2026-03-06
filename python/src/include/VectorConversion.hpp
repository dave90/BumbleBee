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
#include <algorithm>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "PythonObjContainer.hpp"
#include "bumblebee/common/TypeDefs.hpp"
#include "bumblebee/common/types/Vector.hpp"

namespace bumblebee::python {


enum class PandasType : uint8_t {
    BOOLEAN,
    TINYINT,
    SMALLINT,
    INTEGER,
    BIGINT,
    UTINYINT,
    USMALLINT,
    UINTEGER,
    UBIGINT,
    FLOAT,
    DOUBLE,
    TIMESTAMP,
    INTERVAL,
    VARCHAR,
    OBJECT,
    CATEGORY
};

struct NumPyArrayWrapper {
    explicit NumPyArrayWrapper(pybind11::array numpy_array) : numpyArray_(std::move(numpy_array)) {
    }

    pybind11::array numpyArray_;
};


struct PandasColumnBindData {
    PandasType pandasType_;
    pybind11::array numpyCol_;
    // A stride in NumPy is the number of bytes you need to step in memory to move to the next element
    idx_t numpyStride_;
    std::unique_ptr<NumPyArrayWrapper> mask_;
    // Only for categorical types
    string internalCategoricalType_;
    // When object types are cast we must hold their data somewhere
    PythonObjectContainer<pybind11::str> objectStrVal_;
    // enum entries, only for categorical types
    vector<string> enumEntries_;
};

class VectorConversion {
public:
    static void numpyToBumbleBee(PandasColumnBindData &bind_data, pybind11::array &numpy_col, idx_t count, idx_t offset,
                              Vector &out);

    static void bindPandas(pybind11::handle df, vector<PandasColumnBindData> &out, vector<LogicalType> &return_types,
                           vector<string> &names);
};


}
