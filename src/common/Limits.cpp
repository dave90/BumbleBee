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

#include "bumblebee/common/Limits.hpp"

namespace bumblebee{

using std::numeric_limits;

int8_t NumericLimits<int8_t>::minimum() {
	return numeric_limits<int8_t>::lowest();
}

int8_t NumericLimits<int8_t>::maximum() {
	return numeric_limits<int8_t>::max();
}

int16_t NumericLimits<int16_t>::minimum() {
	return numeric_limits<int16_t>::lowest();
}

int16_t NumericLimits<int16_t>::maximum() {
	return numeric_limits<int16_t>::max();
}

int32_t NumericLimits<int32_t>::minimum() {
	return numeric_limits<int32_t>::lowest();
}

int32_t NumericLimits<int32_t>::maximum() {
	return numeric_limits<int32_t>::max();
}

int64_t NumericLimits<int64_t>::minimum() {
	return numeric_limits<int64_t>::lowest();
}

int64_t NumericLimits<int64_t>::maximum() {
	return numeric_limits<int64_t>::max();
}

float NumericLimits<float>::minimum() {
	return numeric_limits<float>::lowest();
}

float NumericLimits<float>::maximum() {
	return numeric_limits<float>::max();
}

double NumericLimits<double>::minimum() {
	return numeric_limits<double>::lowest();
}

double NumericLimits<double>::maximum() {
	return numeric_limits<double>::max();
}

uint8_t NumericLimits<uint8_t>::minimum() {
	return numeric_limits<uint8_t>::lowest();
}

uint8_t NumericLimits<uint8_t>::maximum() {
	return numeric_limits<uint8_t>::max();
}

uint16_t NumericLimits<uint16_t>::minimum() {
	return numeric_limits<uint16_t>::lowest();
}

uint16_t NumericLimits<uint16_t>::maximum() {
	return numeric_limits<uint16_t>::max();
}

uint32_t NumericLimits<uint32_t>::minimum() {
	return numeric_limits<uint32_t>::lowest();
}

uint32_t NumericLimits<uint32_t>::maximum() {
	return numeric_limits<uint32_t>::max();
}

uint64_t NumericLimits<uint64_t>::minimum() {
	return numeric_limits<uint64_t>::lowest();
}

uint64_t NumericLimits<uint64_t>::maximum() {
	return numeric_limits<uint64_t>::max();
}


}