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
#include "bumblebee/common/Mutex.hpp"
#include "bumblebee/common/TypeDefs.hpp"
#include "bumblebee/function/Function.hpp"
#include "bumblebee/ClientContext.hpp"
#include "bumblebee/function/PredFunction.hpp"

namespace bumblebee{


struct GenIdData : public FunctionData {
    atomic<idx_t> counter_{0};

    idx_t getNextId(idx_t step);
};



class GenIdFunc : public FunctionGenerator {
public:
    string getName() override;
    function_ptr_t createFunction(const vector<LogicalType> &type) override;

    // register the functions
    static void registerFunction(FunctionRegister& funcRegister);
};



}
