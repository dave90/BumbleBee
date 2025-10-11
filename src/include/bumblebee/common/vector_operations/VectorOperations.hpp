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
#include "bumblebee/common/types/Vector.hpp"

namespace bumblebee{


// VectorOperations contains a set of operations that operate on sets of
// vectors.
class VectorOperations {
public:

    // Comparison operations
    static idx_t equals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel);
    static idx_t notEquals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel);
    static idx_t greaterThan(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel);
    static idx_t greaterThanEquals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel);
    static idx_t lessThan(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel);
    static idx_t lessThanEquals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel);
    static idx_t equals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel, SelectionVector *falseSel, idx_t& falseCount);
    // TODO implement other comparison with false sel and count

    // Arithmetic operations
    static void sum(Vector &left, Vector &right, Vector &result, idx_t count);
    static void dot(Vector &left, Vector &right, Vector &result, idx_t count);
    static void division(Vector &left, Vector &right, Vector &result, idx_t count);
    static void difference(Vector &left, Vector &right, Vector &result, idx_t count);
    static void modulo(Vector &left, Vector &right, Vector &result, idx_t count);
    static void lAnd(Vector &left, Vector &right, Vector &result, idx_t count);


    // Hash the vector
    static void hash(Vector &input, Vector &hashes, idx_t count);
    // hash only in the index inside the selection vector ( loc ignoring the idx)
    // do the computation only for the sel index
    static void hash(Vector &input, Vector &hashes, const SelectionVector &rsel, idx_t count);
    static void combineHash(Vector &hashes, Vector &B, idx_t count);
    static void combineHash(Vector &hashes, Vector &B, const SelectionVector &rsel, idx_t count);

    // Sequence generations
    static void generateSequence(Vector &result, idx_t count, int64_t start, int64_t increment);
    static void generateSequence(Vector &result, idx_t count, int64_t start, int64_t offset,  int64_t stride, int64_t end);
    // generate sequence only in the index inside the selection vector
    static void generateSequence(Vector &result, idx_t count, const SelectionVector &sel, int64_t start, int64_t increment);
    static void generateSequence(Vector &result, idx_t count, const SelectionVector &sel, int64_t start, int64_t offset, int64_t stride, int64_t end );

    // Copy
    static void copy(const Vector &source, Vector &target, idx_t source_count, idx_t source_offset, idx_t target_offset);
    static void copy(const Vector &source, Vector &target, const SelectionVector &sel, idx_t source_count, idx_t source_offset, idx_t target_offset);
    static void copy(const Vector &source, Vector &target, const SelectionVector &sel,const SelectionVector *targetSel, idx_t source_count, idx_t source_offset, idx_t target_offset);

    // Cast
    static void cast(Vector &source, Vector &target, idx_t source_count);
    static bool tryCast(Vector &source, Vector &target, idx_t source_count, string *errorMessage);


};


}
