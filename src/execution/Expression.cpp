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
#include "bumblebee/execution/Expression.h"

namespace bumblebee{
Expression::Expression(Binop op, const Operands &left, const Operands &right): op_(op),
    left_(left),
    right_(right) {
    BB_ASSERT(verify() && "Expression verification failed");
}

std::string Expression::toString() {
    std::string result ="";
    for (idx_t i=0;i<left_.operators_.size();++i) {
        result += std::to_string(left_.cols_[i]) + " "+getOperatorChar(left_.operators_[i]) + " "+ std::to_string(left_.cols_[i+1]) ;
    }
    result += getBinopStr(op_);
    for (idx_t i=0;i<right_.operators_.size();++i) {
        result += std::to_string(right_.cols_[i]) + " "+getOperatorChar(right_.operators_[i]) + " " +std::to_string(right_.cols_[i+1]) ;
    }
    return result;
}

bool Expression::verify() {
    if (left_.cols_.size() == 0 || right_.cols_.size() == 0) return false;
    if (left_.cols_.size() != left_.operators_.size() +1) return false;
    if (right_.cols_.size() != right_.operators_.size() +1) return false;
    return true;
}

void Expression::execute(DataChunk &input, DataChunk &output) {
    if (op_ == ASSIGNMENT) {
        // execute the right operand and assign to left column
        BB_ASSERT(left_.cols_.size() == 1);
        BB_ASSERT(right_.cols_.size() > 1);
        auto result = executeOperands(input, right_);
        output.data_[left_.cols_[0]].reference(result);
        return;
    }
    auto leftResult = executeOperands(input, left_);
    auto rightResult = executeOperands(input, right_);
    SelectionVector sel(STANDARD_VECTOR_SIZE);
    idx_t count = executeBinop(leftResult, rightResult, sel);
    output.reference(input);
    output.slice(sel, count);
    output.setCardinality(count);
}

Vector Expression::executeOperands(DataChunk &input, Operands &op) {
    if (op.cols_.size() == 1) {
        Vector result(input.data_[op.cols_[0]]);
        return result;
    }
    vector_vector_t vectors;
    std::vector<ConstantType> types;
    ConstantType resultType = UNKNOWN;
    for (auto c: op.cols_) {
        vectors.emplace_back(input.data_[c]);
        resultType = getBumpedType(resultType, input.data_[c].getType());
    }
    Vector result(resultType);
    // TODO
    return result;
}

idx_t Expression::executeBinop(Vector& left,Vector& right, SelectionVector& sel) {
    // TODO
    return 0;
}

}
