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
#include "bumblebee/function/FunctionRegister.hpp"

#include "bumblebee/common/ErrorHandler.hpp"

namespace bumblebee{
void FunctionRegister::registerFunction(function_ptr function) {
    // check if the function name contains "_", it is not supported as in the internal predicates
    // we use "_" character with function names
    if (function->name_.find('_') != std::string::npos) {
        ErrorHandler::errorNotImplemented("Functions with '_' are not supported.");
    }
    // check if it is already present
    if (funcmap_.contains( function->name_) && funcmap_[function->name_].contains(function->arguments_))
        return;

    funcmap_[function->name_][function->arguments_] = function;
}

function_ptr FunctionRegister::getFunction(const string& name, const vector<ConstantType>& arguments) {
    if (!funcmap_.contains(name) )
        return nullptr;
    if (!funcmap_[name].contains(arguments))
        return nullptr;

    return funcmap_[name][arguments];
}
}
