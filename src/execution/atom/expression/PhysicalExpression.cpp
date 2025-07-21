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
#include "bumblebee/execution/atom/expression/PhysicalExpression.h"

namespace bumblebee{

PhysicalExpression::PhysicalExpression(Expression& expr, std::vector<ConstantType>& types, idx_t estimated_cardinality) : PhysicalAtom(types, estimated_cardinality), expression_(std::move(expr)) {
}

PhysicalExpression::~PhysicalExpression() {}


string PhysicalExpression::getName() const {
    return "PHYSICAL_EXPRESSION";
}

string PhysicalExpression::toString() const {
    std::string result = getName();
    result += "( " + expression_.toString() + " )";
    return result;
}

AtomResultType PhysicalExpression::execute(DataChunk &input, DataChunk &chunk, PhysicalAtomState &state) const {
    auto &vectors = input.data_;
    if (expression_.op_ == ASSIGNMENT) {
        // execute the right operand and assign to left column
        BB_ASSERT(expression_.left_.cols_.size() == 1);
        BB_ASSERT(expression_.right_.cols_.size() >= 1);
        auto result = expression_.executeRight(vectors, input.getSize());
        chunk.reference(input);
        chunk.data_[expression_.left_.cols_[0]].reference(result);

        return AtomResultType::NEED_MORE_INPUT;
    }
    auto leftResult = expression_.executeLeft(vectors, input.getSize());
    auto rightResult = expression_.executeRight(vectors, input.getSize());

    SelectionVector sel(STANDARD_VECTOR_SIZE);
    idx_t count = expression_.executeBinop(leftResult, rightResult, sel, input.getSize());
    chunk.reference(input);
    chunk.slice(sel, count);
    chunk.setCardinality(count);
    return AtomResultType::NEED_MORE_INPUT;

}

}
