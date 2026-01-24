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

void FunctionRegister::registerFunctionGen(function_gen_ptr_t& function) {

    // check if it is already present
    if (funcmap_.contains( function->getName()))
        return;

    funcmap_[function->getName()] = std::move(function);
}

function_ptr_t FunctionRegister::getFunction(const string& name, const vector<LogicalType>& arguments) {
    if (!funcmap_.contains(name) )
        return nullptr;

    return funcmap_[name]->getFunction(arguments);
}
}
