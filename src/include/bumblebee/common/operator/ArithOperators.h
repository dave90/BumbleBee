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
#include "bumblebee/common/types/BumbleString.h"

namespace bumblebee{

//===--------------------------------------------------------------------===//
// Arith Operations
//===--------------------------------------------------------------------===//
struct Sum {
    template <class T>
    static inline T operation(T left, T right) {
        return left+right;
    }
};

struct Division {
    template <class T>
    static inline T operation(T left, T right) {
        return left/right;
    }
};

struct Dot {
    template <class T>
    static inline T operation(T left, T right) {
        return left*right;
    }
};

struct Difference {
    template <class T>
    static inline T operation(T left, T right) {
        return left-right;
    }
};

struct Modulo {
    template <class T>
    static inline T operation(T left, T right) {
        return left % right;
    }
};

//===--------------------------------------------------------------------===//
// Float and Double Modulo Operations
//===--------------------------------------------------------------------===//

template<>
inline float Modulo::operation(float left, float right) {
    return std::fmod(left, right);
}

template<>
inline double Modulo::operation(double left, double right) {
    return std::fmod(left, right);
}

//===--------------------------------------------------------------------===//
// String  Operations
//===--------------------------------------------------------------------===//
template<>
inline string_t Sum::operation(string_t left, string_t right) {
    // TODO
    return left;
}
template<>
inline string_t Division::operation(string_t left, string_t right) {
    // TODO
    return left;
}
template<>
inline string_t Dot::operation(string_t left, string_t right) {
    // TODO
    return left;
}
template<>
inline string_t Difference::operation(string_t left, string_t right) {
    // TODO
    return left;
}

template<>
inline string_t Modulo::operation(string_t left, string_t right) {
    // TODO
    return left;
}
}
