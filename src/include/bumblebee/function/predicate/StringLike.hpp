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
#include "bumblebee/function/Function.hpp"
#include "bumblebee/function/PredFunction.hpp"

namespace bumblebee{


struct StringLikeData : public FunctionData {
    StringLikeData( string& likePattern, char escape = '\0');

    bool match(string_t& str);

    vector<string> segments_; // segment patterns in like
    bool hasStartPercentage_;
    bool hasEndPercentage_;

    // When the pattern contains '_' or an escape sequence the optimised
    // segment-based path cannot be used; we fall back to a full generic match.
    bool useGenericMatch_;
    string originalPattern_;
    char escape_;
};

struct StringLikeOperatorData : public FunctionOperatorData {

};


class StringLikeFunc : public FunctionGenerator{
public:
    string getName() override;

    function_ptr_t createFunction(const vector<LogicalType> &type) override;

    // register the functions
    static void registerFunction(FunctionRegister& funcRegister);
};


}
