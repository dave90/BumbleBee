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
#include "bumblebee/common/ErrorHandler.hpp"
#include "bumblebee/common/TypeDefs.hpp"

#include "bumblebee/common/Hash.hpp"

namespace bumblebee{

struct FunctionData {
    virtual ~FunctionData() {
    }

    virtual std::unique_ptr<FunctionData> copy() {
        ErrorHandler::errorNotImplemented("Unimplemented copy for FunctionData");
        return nullptr;
    };

    virtual bool equals(FunctionData &other) {
        return true;
    }

    static bool equals(FunctionData *left, FunctionData *right) {
        if (left == right) {
            return true;
        }
        if (!left || !right) {
            return false;
        }
        return left->equals(*right);
    }
};

using function_data_ptr_t = std::unique_ptr<FunctionData>;

// General function that can be used during the execution
// TODO: Make this pluggable: allow loading function implementations at runtime
//       (e.g., fetch from Git, compile, and register so they’re available during execution).
class Function {

public:
    Function(const string &name, const vector<LogicalType> &arguments, LogicalType result);

    Function(const string &name, const vector<LogicalType> &arguments);

    Function(const Function &other);

    Function(Function &&other) noexcept;

    Function & operator=(const Function &other);

    Function & operator=(Function &&other) noexcept;

    friend bool operator==(const Function &lhs, const Function &rhs);

    friend bool operator!=(const Function &lhs, const Function &rhs);

    string name_;
    vector<LogicalType> arguments_;
    LogicalType result_;
};

using function_ptr_t = std::shared_ptr<Function>;

}
