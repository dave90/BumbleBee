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
    static idx_t equals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel, SelectionVector *falseSel, idx_t& falseCount);
    static idx_t notEquals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel);
    static idx_t notEquals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel, SelectionVector *falseSel, idx_t& falseCount);
    static idx_t greaterThan(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel);
    static idx_t greaterThan(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel, SelectionVector *falseSel, idx_t& falseCount);
    static idx_t greaterThanEquals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel);
    static idx_t greaterThanEquals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel, SelectionVector *falseSel, idx_t& falseCount);
    static idx_t lessThan(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel);
    static idx_t lessThan(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel, SelectionVector *falseSel, idx_t& falseCount);
    static idx_t lessThanEquals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel);
    static idx_t lessThanEquals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel, SelectionVector *falseSel, idx_t& falseCount);

    // NULL-aware comparators.
    //
    // The standard equals/notEquals above implement the SQL "=" / "!=" semantics:
    // a NULL operand makes the comparison UNKNOWN, which is never TRUE, so the
    // row is excluded from `trueSel`. That is the right behavior for equi-join
    // keys, where a NULL key must never match.
    //
    // notDistinctFrom implements SQL `IS NOT DISTINCT FROM`: two NULLs compare
    // equal, a NULL and a non-NULL do not. distinctFrom is the inverse. These
    // are the comparators to use for dedup, DISTINCT, GROUP BY, and the
    // recursive head-predicate hash tables — without them a recursive program
    // over null-bearing facts re-derives `a(1,NULL)` forever.
    static idx_t notDistinctFrom(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel);
    static idx_t distinctFrom(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *trueSel);

    // Unary NULL predicates: pure mask reads, independent of the underlying value.
    static idx_t isNull(Vector &input, const SelectionVector *sel, idx_t count, SelectionVector *trueSel);
    static idx_t isNotNull(Vector &input, const SelectionVector *sel, idx_t count, SelectionVector *trueSel);
    // 7-arg variant produces both selections so OR-eval can feed unmatched rows to the next branch.
    static idx_t isNull(Vector &input, const SelectionVector *sel, idx_t count, SelectionVector *trueSel, SelectionVector *falseSel, idx_t &falseCount);
    static idx_t isNotNull(Vector &input, const SelectionVector *sel, idx_t count, SelectionVector *trueSel, SelectionVector *falseSel, idx_t &falseCount);

    // Arithmetic operations
    static void sum(Vector &left, Vector &right, Vector &result, idx_t count);
    static void dot(Vector &left, Vector &right, Vector &result, idx_t count);
    static void division(Vector &left, Vector &right, Vector &result, idx_t count);
    static void difference(Vector &left, Vector &right, Vector &result, idx_t count);
    static void negate(Vector &input, Vector &result, idx_t count);
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
