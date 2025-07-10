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
#include "bumblebee/common/types/Vector.h"

namespace bumblebee{


// VectorOperations contains a set of operations that operate on sets of
// vectors.
class VectorOperations {
public:
    // static void addInPlace(Vector &A, int64_t B, idx_t count);
    //
    // static void isNotNull(Vector &A, Vector &result, idx_t count);
    // static void isNull(Vector &A, Vector &result, idx_t count);
    // static bool hasNull(Vector &A, idx_t count);
    // static bool hasNotNull(Vector &A, idx_t count);
    //
    // static void boolAnd(Vector &A, Vector &B, Vector &result, idx_t count);
    // static void boolOr(Vector &A, Vector &B, Vector &result, idx_t count);
    // static void boolNot(Vector &A, Vector &result, idx_t count);
    //
    // static void equals(Vector &A, Vector &B, Vector &result, idx_t count);
    // static void notEquals(Vector &A, Vector &B, Vector &result, idx_t count);
    // static void greaterThan(Vector &A, Vector &B, Vector &result, idx_t count);
    // static void greaterThanEquals(Vector &A, Vector &B, Vector &result, idx_t count);
    // static void lessThan(Vector &A, Vector &B, Vector &result, idx_t count);
    // static void lessThanEquals(Vector &A, Vector &B, Vector &result, idx_t count);
    //
    // static void distinctFrom(Vector &left, Vector &right, Vector &result, idx_t count);
    // static void notDistinctFrom(Vector &left, Vector &right, Vector &result, idx_t count);
    // static void distinctGreaterThan(Vector &left, Vector &right, Vector &result, idx_t count);
    // static void distinctGreaterThanEquals(Vector &left, Vector &right, Vector &result, idx_t count);
    // static void distinctLessThan(Vector &left, Vector &right, Vector &result, idx_t count);
    // static void distinctLessThanEquals(Vector &left, Vector &right, Vector &result, idx_t count);
    //
    // static idx_t equals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *true_sel, SelectionVector *false_sel);
    // static idx_t notEquals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *true_sel, SelectionVector *false_sel);
    // static idx_t greaterThan(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *true_sel, SelectionVector *false_sel);
    // static idx_t greaterThanEquals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *true_sel, SelectionVector *false_sel);
    // static idx_t lessThan(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *true_sel, SelectionVector *false_sel);
    // static idx_t lessThanEquals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *true_sel, SelectionVector *false_sel);
    //
    // static idx_t distinctFrom(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *true_sel, SelectionVector *false_sel);
    // static idx_t notDistinctFrom(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *true_sel, SelectionVector *false_sel);
    // static idx_t distinctGreaterThan(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *true_sel, SelectionVector *false_sel);
    // static idx_t distinctGreaterThanEquals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *true_sel, SelectionVector *false_sel);
    // static idx_t distinctLessThan(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *true_sel, SelectionVector *false_sel);
    // static idx_t distinctLessThanEquals(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *true_sel, SelectionVector *false_sel);
    //
    // static idx_t distinctGreaterThanNullsFirst(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *true_sel, SelectionVector *false_sel);
    // static idx_t distinctLessThanNullsFirst(Vector &left, Vector &right, const SelectionVector *sel, idx_t count, SelectionVector *true_sel, SelectionVector *false_sel);
    //
    // static idx_t nestedNotEquals(Vector &left, Vector &right, idx_t vcount, const SelectionVector &sel, idx_t count, SelectionVector *true_sel, SelectionVector *false_sel);
    // static idx_t nestedEquals(Vector &left, Vector &right, idx_t vcount, const SelectionVector &sel, idx_t count, SelectionVector *true_sel, SelectionVector *false_sel);
    // static idx_t nestedGreaterThan(Vector &left, Vector &right, idx_t vcount, const SelectionVector &sel, idx_t count, SelectionVector *true_sel, SelectionVector *false_sel);
    // static idx_t nestedGreaterThanEquals(Vector &left, Vector &right, idx_t vcount, const SelectionVector &sel, idx_t count, SelectionVector *true_sel, SelectionVector *false_sel);
    // static idx_t nestedLessThan(Vector &left, Vector &right, idx_t vcount, const SelectionVector &sel, idx_t count, SelectionVector *true_sel, SelectionVector *false_sel);
    // static idx_t nestedLessThanEquals(Vector &left, Vector &right, idx_t vcount, const SelectionVector &sel, idx_t count, SelectionVector *true_sel, SelectionVector *false_sel);
    //
    // static void hash(Vector &input, Vector &hashes, idx_t count);
    // static void hash(Vector &input, Vector &hashes, const SelectionVector &rsel, idx_t count);
    // static void combineHash(Vector &hashes, Vector &B, idx_t count);
    // static void combineHash(Vector &hashes, Vector &B, const SelectionVector &rsel, idx_t count);
    //
    static void generateSequence(Vector &result, idx_t count, int64_t start = 0, int64_t increment = 1);
    static void generateSequence(Vector &result, idx_t count, const SelectionVector &sel, int64_t start = 0, int64_t increment = 1);
    //
    // static bool tryCast(Vector &source, Vector &result, idx_t count, string *error_message, bool strict = false);
    // static void cast(Vector &source, Vector &result, idx_t count, bool strict = false);
    //
    static void copy(const Vector &source, Vector &target, idx_t source_count, idx_t source_offset, idx_t target_offset);
    static void copy(const Vector &source, Vector &target, const SelectionVector &sel, idx_t source_count, idx_t source_offset, idx_t target_offset);

};


}
