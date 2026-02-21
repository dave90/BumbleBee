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
#include "bumblebee/execution/atom/expression/PhysicalExpression.hpp"

#include "bumblebee/common/vector_operations/VectorOperations.hpp"

namespace bumblebee{

PhysicalExpression::PhysicalExpression(Expression& expr, vector<LogicalType>& types) : PhysicalAtom(types) {
    expressions_.push_back(std::move(expr));
}

PhysicalExpression::PhysicalExpression(vector<Expression> &expr, vector<LogicalType> &types):PhysicalAtom(types), expressions_(std::move(expr)) {
}

PhysicalExpression::PhysicalExpression(idx_t col,Value &constantValue, vector<LogicalType>& types) : PhysicalAtom(types), constantAssignment_(true), constantValue_(std::move(constantValue)) {
    // put the col in the left side of the expression
    Expression expression;
    expression.left_.cols_.push_back(col);
    expressions_.push_back(std::move(expression));
}

PhysicalExpression::~PhysicalExpression() {}


string PhysicalExpression::getName() const {
    return "PHYSICAL_EXPRESSION";
}

string PhysicalExpression::toString() const {
    std::string result = getName();
    if (constantAssignment_)
        return result += "( " + std::to_string(expressions_[0].left_.cols_[0]) + " = Value(" + constantValue_.toString() + ") )";
    for (auto& expression : expressions_)
        result += "( " + expression.toString() + " ), ";
    result.resize(result.size() - 2); // remove last ", "
    return result;
}

AtomResultType PhysicalExpression::execute(ThreadContext& context, DataChunk &input, DataChunk &chunk, PhysicalAtomState &state) const {
    context.profiler_.startPhysicalAtom(this);
    if (input.getSize() == 0) {
        chunk.setCardinality(0);
        context.profiler_.endPhysicalAtom(chunk);
        return AtomResultType::NEED_MORE_INPUT;
    }
    auto &vectors = input.data_;
    if (constantAssignment_) {
        // constant assignment, assign in the data chunk the constant
        BB_ASSERT(expressions_.size() == 1);
        BB_ASSERT(expressions_[0].left_.cols_.size() == 1);
        auto col = expressions_[0].left_.cols_[0];
        Vector vec(constantValue_);
        // Cast the constant to the target type if they differ (e.g., integer constant used in DECIMAL context)
        if (vec.getLogicalType() != types_[col]) {
            Vector castedVec(types_[col]);
            castedVec.setVectorType(VectorType::CONSTANT_VECTOR);
            VectorOperations::cast(vec, castedVec, 1);
            chunk.reference(input);
            chunk.data_[col].reference(castedVec);
        } else {
            chunk.reference(input);
            chunk.data_[col].reference(vec);
        }
        context.profiler_.endPhysicalAtom(chunk);
        return AtomResultType::NEED_MORE_INPUT;
    }
    if (expressions_.size() == 1 && expressions_[0].op_ == ASSIGNMENT) {
        // execute the right operand and assign to left column
        auto& expression = expressions_[0];
        BB_ASSERT(expression.left_.cols_.size() == 1);
        auto colToAssign = expression.left_.cols_[0];
        BB_ASSERT(expression.right_.cols_.size() >= 1);
        BB_ASSERT(colToAssign < types_.size());
        auto result = expression.executeRight(vectors, input.getSize(), types_[colToAssign] );
        chunk.reference(input);
        BB_ASSERT(types_[colToAssign] == result.getLogicalType());

        chunk.data_[expression.left_.cols_[0]].reference(result);
        context.profiler_.endPhysicalAtom(chunk);
        return AtomResultType::NEED_MORE_INPUT;
    }


    SelectionVector matchSel(STANDARD_VECTOR_SIZE);
    idx_t matchCount = 0;
    SelectionVector trueSel(STANDARD_VECTOR_SIZE);
    SelectionVector falseSel(STANDARD_VECTOR_SIZE);
    idx_t falseCount = 0;
    SelectionVector sel = FlatVector::INCREMENTAL_SELECTION_VECTOR;
    idx_t count = input.getSize();
    for (auto& expression : expressions_) {
        auto leftResult = expression.executeLeft(vectors, input.getSize());
        auto rightResult = expression.executeRight(vectors, input.getSize());
        expression.executeBinop(leftResult, rightResult, sel, count, trueSel, falseSel, falseCount);
        // add the true sel to the matched
        BB_ASSERT(count >= falseCount);
        for (idx_t i = 0; i < count - falseCount; ++i)
            matchSel.setIndex(matchCount++, trueSel.getIndex(i));
        if (falseCount == 0) break; // all matches stop the expr evaluation
        // add the false sel to the next iteration
        sel.initialize(falseSel);
        count = falseCount;
    }

    chunk.reference(input);
    chunk.slice(matchSel, matchCount);
    chunk.setCardinality(matchCount);

    context.profiler_.endPhysicalAtom(chunk);
    return AtomResultType::NEED_MORE_INPUT;
}


}
