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
#include "bumblebee/common/types/DataChunk.h"
#include "bumblebee/parser/statement/Atom.h"

namespace bumblebee{

// Represents the operands and associated operators used in a compound expression.
// `cols_` holds the indices of the input columns involved in the expression.
// These are evaluated in the order they appear.
// `operators_` specifies the binary operations to apply between consecutive columns.
// The number of operators must be one less than the number of columns.
struct Operands {
    std::vector<idx_t> cols_;
    std::vector<Operator> operators_;
};

struct Expression {
    Binop op_{NONE_OP};
    Operands left_;
    Operands right_;
    Expression() = default;

    Expression(Binop op, const Operands &left, const Operands &right);
    Expression(Binop op, std::vector<idx_t>& leftCols, std::vector<Operator>& leftOps,std::vector<idx_t> &rightCols, std::vector<Operator>& rightOps );
    Expression(Expression &&other) noexcept;

    std::string toString() const;
    // Execute the binop operation and set the results in the selection vector
    // and returns the numbers of rows filtered
    idx_t executeBinop(Vector& left,Vector& right, SelectionVector& sel, idx_t count) const;
    // verify the expression
    bool verify() const;
    // Execute the right side of the expression (expected vector of all the columns of the data chunk)
    inline Vector executeRight(vector_vector_t& allColumns, idx_t count, ConstantType resultType = UNKNOWN) const {
        return executeOperands(allColumns, right_, count);
    }
    // Execute the left side of the expression (expected vector of all the columns of the data chunk)
    inline Vector executeLeft(vector_vector_t& allColumns, idx_t count, ConstantType resultType = UNKNOWN) const{
        return executeOperands(allColumns, left_, count);
    }
    // Execute a operands and set the result in result vector
    static Vector executeOperands(vector_vector_t& vectors, const Operands& op, idx_t count, ConstantType resultType = UNKNOWN);


};


}
