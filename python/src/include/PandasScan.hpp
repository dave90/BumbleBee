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
#include "VectorConversion.hpp"
#include "bumblebee/function/Function.hpp"
#include "bumblebee/function/FunctionRegister.hpp"
#include "bumblebee/function/PredFunction.hpp"
#include "pybind11/pytypes.h"

namespace bumblebee::python {

struct PandasScanDataChunk {
    // inclusive start and end
    idx_t start_;
    idx_t end_;  // if 0 no data to read
};

struct PandasScanData : public FunctionData {
    PandasScanData(const pybind11::handle &df, idx_t row_count, vector<PandasColumnBindData> &bind_data,
        const vector<LogicalType> &types)
        : df(df),
          rowCount_(row_count),
          linesCount_(0),
          bindData_(std::move(bind_data)),
          types_(types) {
    }

    ~PandasScanData() override {
        pybind11::gil_scoped_acquire acquire;
        bindData_.clear();
    }

    idx_t getMaxThread();
    PandasScanDataChunk getNextFileToRead() ;

    pybind11::handle df;
    idx_t rowCount_;
    idx_t linesCount_;
    vector<PandasColumnBindData> bindData_;
    vector<LogicalType> types_;
    std::mutex mutex_;
};

struct PandasScanOperatorData : public FunctionOperatorData {
    PandasScanDataChunk chunkToRead_;
    DataChunk chunk_;
};


class PandasScanFunc : public FunctionGenerator{
public:
    string getName() override;

    function_ptr_t createFunction(const vector<LogicalType> &type) override;

    // register the functions
    static void registerFunction(FunctionRegister& funcRegister);
};

}
