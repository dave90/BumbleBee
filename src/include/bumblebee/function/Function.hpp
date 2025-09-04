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

#include "bumblebee/common/Hash.hpp"

namespace bumblebee{


// General function that can be used during the execution
// TODO: Make this pluggable: allow loading function implementations at runtime
//       (e.g., fetch from Git, compile, and register so they’re available during execution).
class Function {

public:
    Function(const string &name, const vector<ConstantType> &arguments, ConstantType result);

    Function(const Function &other);

    Function(Function &&other) noexcept;

    Function & operator=(const Function &other);

    Function & operator=(Function &&other) noexcept;

    friend bool operator==(const Function &lhs, const Function &rhs);

    friend bool operator!=(const Function &lhs, const Function &rhs);

    string name_;
    vector<ConstantType> arguments_;
    ConstantType result_;
};

using function_ptr = std::shared_ptr<Function>;

}
