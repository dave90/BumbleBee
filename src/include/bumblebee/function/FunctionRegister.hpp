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


class FunctionGenerator {
public:
    virtual ~FunctionGenerator() = default;

    virtual string getName() = 0;
    function_ptr_t getFunction(const vector<LogicalType>& type) {
        if (!funcCache_.contains(type))
            funcCache_[type] = createFunction(type);
        return funcCache_[type];
    }

protected:
    virtual function_ptr_t createFunction(const vector<LogicalType>& type) = 0;
    std::unordered_map<vector<LogicalType>, function_ptr_t, VectorLogicTypeHash, VectorLogicTypeHash> funcCache_;
};

using function_gen_ptr_t = std::unique_ptr<FunctionGenerator>;

class FunctionRegister {
private:
    // Hash functor for vector<ConstantType>

    using arguments_func_map = std::unordered_map<vector<LogicalType>, function_ptr_t, VectorLogicTypeHash>;

public:
    FunctionRegister() = default;

    void registerFunctionGen(function_gen_ptr_t& function);

    // if the function is not present return nullptr
    function_ptr_t getFunction(const string& name,const vector<LogicalType>& arguments);
private:

    std::unordered_map<string, function_gen_ptr_t> funcmap_;
};


}
