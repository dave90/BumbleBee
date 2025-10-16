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

#include "bumblebee/function/Function.hpp"

namespace bumblebee {
Function::Function(const string &name, const vector<ConstantType> &arguments, ConstantType result): name_(name),
    arguments_(arguments),
    result_(result) {
}

Function::Function(const string &name, const vector<ConstantType> &arguments) : Function(name, arguments, UNKNOWN) {
}

Function::Function(const Function &other): name_(other.name_),
                                           arguments_(other.arguments_),
                                           result_(other.result_) {
}

Function::Function(Function &&other) noexcept: name_(std::move(other.name_)),
                                               arguments_(std::move(other.arguments_)),
                                               result_(other.result_) {
}

Function & Function::operator=(const Function &other) {
    if (this == &other)
        return *this;
    name_ = other.name_;
    arguments_ = other.arguments_;
    result_ = other.result_;
    return *this;
}

Function & Function::operator=(Function &&other) noexcept {
    if (this == &other)
        return *this;
    name_ = std::move(other.name_);
    arguments_ = std::move(other.arguments_);
    result_ = other.result_;
    return *this;
}

bool operator==(const Function &lhs, const Function &rhs) {
    return lhs.name_ == rhs.name_
           && lhs.arguments_ == rhs.arguments_
           && lhs.result_ == rhs.result_;
}

bool operator!=(const Function &lhs, const Function &rhs) {
    return !(lhs == rhs);
}
}
