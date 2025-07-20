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

#include "bumblebee/common/types/BumbleString.h"

#include <locale>

namespace bumblebee {
BumbleString::BumbleString(const char *data): BumbleString(data, strlen(data)) {}

BumbleString::BumbleString(const char *data, uint32_t len) {
    value_.length = len;
    if (isInlined()) {
        // store the data in prefix
        // +1 for string termination
        memcpy(value_.prefix, data, len * sizeof(char));
        value_.prefix[len] = '\0';
        return;
    }
    memcpy(value_.prefix, data, PREFIX_LENGTH * sizeof(char));
    value_.prefix[PREFIX_LENGTH] = '\0';
    value_.ptr = (char *)(data);
}

BumbleString::BumbleString(const BumbleString &other): BumbleString(other.c_str(), other.size()) {}


char * BumbleString::getDataWriteable() const {
    if (isInlined())
        return (char*)value_.prefix;
    return value_.ptr;
}

string BumbleString::getString() const {
    return string(getDataUnsafe());
}


bool BumbleString::operator<(const BumbleString &r) const {
    // compare the data
    auto cmp = strcmp(getDataUnsafe(), r.getDataUnsafe());
    return (cmp < 0);
}

const char * BumbleString::c_str() const {
    return getDataUnsafe();
}

bool BumbleString::isInlined( uint32_t len) {
    return len <= PREFIX_LENGTH;
}
}
