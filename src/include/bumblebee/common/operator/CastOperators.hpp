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
#include <type_traits>
#include <limits>
#include <cmath>

namespace bumblebee{


//===--------------------------------------------------------------------===//
// Cast Operations
//===--------------------------------------------------------------------===//
struct Cast {
private:
    template <class T>
    static constexpr bool is_arith_v = std::is_arithmetic_v<T>;

    template <class T>
    static constexpr bool is_int_v = std::is_integral_v<T>;

    template <class T>
    static constexpr bool is_float_v = std::is_floating_point_v<T>;


public:
    // Generic arithmetic → arithmetic cast with safety checks.
    template <class SRC, class DST>
    static inline DST operation(SRC input) {
        if constexpr (is_arith_v<SRC> && is_arith_v<DST>) {
            return static_cast<DST>(input);
        }
        // Non-arithmetic (e.g., string) lands here
        ErrorHandler::errorNotImplemented("Unimplemented type for cast :(");
        return static_cast<DST>(input); // never returned
    }
};






}
