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

PhysicalExpression::PhysicalExpression(idx_t col,Value &constantValue, std::vector<ConstantType>& types) : PhysicalAtom(types, 1), constantAssignment_(true), constantValue_(std::move(constantValue)) {
    // put the col in the left side of the expression
    expression_.left_.cols_.push_back(col);
}

PhysicalExpression::~PhysicalExpression() {}


string PhysicalExpression::getName() const {
    return "PHYSICAL_EXPRESSION";
}

string PhysicalExpression::toString() const {
    std::string result = getName();
    if (!constantAssignment_)
        return result += "( " + expression_.toString() + " )";
    return result += "( " + std::to_string(expression_.left_.cols_[0]) + " = Value(" + constantValue_.toString() + ") )";
}

AtomResultType PhysicalExpression::execute(ThreadContext& context, DataChunk &input, DataChunk &chunk, PhysicalAtomState &state) const {
    context.profiler_.startPhysicalAtom(this);
    if (input.getSize() == 0) {
        context.profiler_.endPhysicalAtom(chunk);
        return AtomResultType::NEED_MORE_INPUT;
    }
    auto &vectors = input.data_;
    if (constantAssignment_) {
        // constant assignment, assign in the data chunk the constant
        BB_ASSERT(expression_.left_.cols_.size() == 1);
        auto col = expression_.left_.cols_[0];
        Vector vec(constantValue_);
        chunk.reference(input);
        chunk.data_[col].reference(vec);
        context.profiler_.endPhysicalAtom(chunk);
        return AtomResultType::NEED_MORE_INPUT;
    }
    if (expression_.op_ == ASSIGNMENT) {
        // execute the right operand and assign to left column
        BB_ASSERT(expression_.left_.cols_.size() == 1);
        auto colToAssign = expression_.left_.cols_[0];
        BB_ASSERT(expression_.right_.cols_.size() >= 1);
        BB_ASSERT(colToAssign < types_.size());
        auto result = expression_.executeRight(vectors, input.getSize(), types_[colToAssign] );
        chunk.reference(input);
        BB_ASSERT(types_[colToAssign] == result.getType());

        chunk.data_[expression_.left_.cols_[0]].reference(result);
        context.profiler_.endPhysicalAtom(chunk);
        return AtomResultType::NEED_MORE_INPUT;
    }
    auto leftResult = expression_.executeLeft(vectors, input.getSize());
    auto rightResult = expression_.executeRight(vectors, input.getSize());

    SelectionVector sel(STANDARD_VECTOR_SIZE);
    idx_t count = expression_.executeBinop(leftResult, rightResult, sel, input.getSize());
    chunk.reference(input);
    chunk.slice(sel, count);
    chunk.setCardinality(count);

    context.profiler_.endPhysicalAtom(chunk);
    return AtomResultType::NEED_MORE_INPUT;

}

}
