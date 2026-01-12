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
#include <unordered_map>

#include "Function.hpp"

namespace bumblebee{


class FunctionRegister {
private:
    // Hash functor for vector<ConstantType>

    using arguments_func_map = std::unordered_map<vector<LogicalType>, function_ptr_t, VectorLogicTypeHash>;

public:
    FunctionRegister() = default;

    void registerFunction(function_ptr_t function);
    // if the function is not present return nullptr
    function_ptr_t getFunction(const string& name,const vector<LogicalType>& arguments);
private:

    std::unordered_map<string, arguments_func_map> funcmap_;
};


}
