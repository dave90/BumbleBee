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
#include "TypeDefs.hpp"
#include "Vector.hpp"
#include "types/BumbleString.hpp"
#include "types/Vector.hpp"

namespace bumblebee{

// Utility class for string operations
class StringUtils {
public:

    static vector<string> split(const string &str, char delimiter);
    static vector<string> split(const string &input, const string &split);
    static bool contains(const string &input, const string &split);
    static bool glob(const char *string, idx_t slen, const char *pattern, idx_t plen);
    static bool glob(const string& str,const string& pattern);
    static bool characterIsSpace(char c);
    static bool characterIsDigit(char c);
    static bool characterIsNewline(char c);
    static string upper(const string &str);
    static string lower(const string &str);
    static void removeQuote(string& str);
    static bool startsWith(string str, string prefix);
};


}
