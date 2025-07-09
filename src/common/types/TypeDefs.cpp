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

#include "bumblebee/common/TypeDefs.h"

#include "bumblebee/common/types/BumbleString.h"
namespace bumblebee {

idx_t getCTypeSize(ConstantType type) {
    switch (type) {
        case TINYINT:    return sizeof(int8_t);
        case SMALLINT:   return sizeof(int16_t);
        case INTEGER:    return sizeof(int32_t);
        case BIGINT:     return sizeof(int64_t);
        case UTINYINT:   return sizeof(uint8_t);
        case USMALLINT:  return sizeof(uint16_t);
        case UINTEGER:   return sizeof(uint32_t);
        case UBIGINT:    return sizeof(uint64_t);
        case FLOAT:      return sizeof(float);
        case DOUBLE:     return sizeof(double);
        case STRING:     return sizeof(bumblebee::string_t);  // size of string object, not content
        default:         return 0; // or throw an error
    }
}

}