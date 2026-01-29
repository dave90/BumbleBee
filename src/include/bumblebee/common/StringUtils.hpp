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
#include <sstream>
#include <unordered_map>

#include "TypeDefs.hpp"
#include "Vector.hpp"
#include "types/BumbleString.hpp"
#include "types/Vector.hpp"
#include "utf8proc/utf8proc.hpp"

namespace bumblebee{

// Utility class for string operations
class StringUtils {
public:

    static vector<string> split(const string &str, char delimiter);
    static vector<string> split(const string &input, const string &split);
    static bool contains(const string &input, const string &split);
    static bool glob(const char *string, idx_t slen, const char *pattern, idx_t plen);
    static bool glob(const string& str,const string& pattern);
    static bool hasGlob(const string &str);
    static bool characterIsSpace(char c);
    static bool characterIsDigit(char c);
    static bool characterIsNewline(char c);
    static string upper(const string &str);
    static string lower(const string &str);
    static void removeQuote(string& str);
    static bool startsWith(string str, string prefix);
    static string trim(const string& str);
    static string normalizeColumnName(const string &col_name);
    static char characterToLower(char c);
    static std::unordered_map<string,string> parseColMapping(const string &columns, const vector<string> &varNames);

    template <typename... Args>
    static std::string format(std::string_view fmt, Args&&... args) {
        try {
            // store the args such that we can pass lvalue and rvalue args
            auto stored = std::tuple<std::decay_t<Args>...>(std::forward<Args>(args)...);
            return std::apply(
                [&](auto&... a) {
                    return std::vformat(fmt, std::make_format_args(a...));
                },
                stored
            );
        } catch (const std::format_error& e) {
            ErrorHandler::errorGeneric(std::string("Format error: ") + e.what());
            return {};
        }
    }

};


}
