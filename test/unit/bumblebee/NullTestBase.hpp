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

#include "BumbleBaseTest.hpp"
#include "bumblebee/common/types/DataChunk.hpp"
#include "bumblebee/common/types/Vector.hpp"

// Shared helpers for NULL-aware tests. Builds flat vectors / chunks with
// validity bits cleared at specific positions and asserts nullness via the mask.
class NullTestBase : public BumbleBaseTest {
protected:
    // A flat vector of `count` rows of the given type, NULL at the listed positions.
    Vector createVectorWithNulls(LogicalType type, idx_t count,
                                 const std::vector<idx_t> &nullPositions) {
        Vector v(type, count);
        for (idx_t i = 0; i < count; i++) {
            v.setValue(i, Value((int64_t)(i + 1)).cast(type.getPhysicalType()));
        }
        for (auto pos : nullPositions) {
            v.setValue(pos, Value::null());
        }
        return v;
    }

    // assert helper reading the mask
    static bool isNull(const Vector &v, idx_t idx) { return !v.rowIsValid(idx); }

    // Reproducible null layout: ~density fraction of [0,count) marked null.
    std::vector<idx_t> randomNullPlacement(idx_t count, double density, std::uint64_t seed) {
        std::mt19937_64 gen(seed);
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        std::vector<idx_t> positions;
        for (idx_t i = 0; i < count; i++) {
            if (dist(gen) < density) positions.push_back(i);
        }
        return positions;
    }
};