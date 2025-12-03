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
#include "bumblebee/execution/Expression.hpp"

#include "bumblebee/common/vector_operations/VectorOperations.hpp"

namespace bumblebee{
Expression::Expression(Binop op, const Operands &left, const Operands &right): op_(op),
    left_(left),
    right_(right) {
    BB_ASSERT(verify() && "Expression verification failed");
}

Expression::Expression(Binop op, vector<idx_t>& leftCols, vector<Operator>& leftOps, vector<idx_t> &rightCols, vector<Operator> &rightOps) : op_(op) {
    left_.cols_ = std::move(leftCols);
    left_.operators_ = std::move(leftOps);
    right_.cols_ = std::move(rightCols);
    right_.operators_ = std::move(rightOps);
}

Expression::Expression(Expression &&other) noexcept: op_(other.op_),
                                                     left_(std::move(other.left_)),
                                                     right_(std::move(other.right_)) {
}

std::string Expression::toString() const{
    std::string result ="";
    result += std::to_string(left_.cols_[0])+" ";
    for (idx_t i=0;i<left_.operators_.size();++i) {
        result += getOperatorChar(left_.operators_[i]);
        result +=   " "+ std::to_string(left_.cols_[i+1]) ;
    }
    result += " "+getBinopStr(op_)+" ";
    result += std::to_string(right_.cols_[0]) + " ";
    for (idx_t i=0;i<right_.operators_.size();++i) {
        result += getOperatorChar(right_.operators_[i]);
        result += " " +std::to_string(right_.cols_[i+1]) ;
    }
    return result;
}

bool Expression::verify() const{
    if (left_.cols_.size() == 0 || right_.cols_.size() == 0) return false;
    if (left_.cols_.size() != left_.operators_.size() +1) return false;
    if (right_.cols_.size() != right_.operators_.size() +1) return false;
    return true;
}



void executeOperator(Vector& left, Vector& right, Vector& result, idx_t count, Operator op) {
    switch (op) {
        case PLUS:
            VectorOperations::sum(left, right, result, count);
            break;
        case MINUS:
            VectorOperations::difference(left, right, result, count);
            break;
        case TIMES:
            VectorOperations::dot(left, right, result, count);
            break;
        case DIV:
            VectorOperations::division(left, right, result, count);
            break;
        case MODULO:
            VectorOperations::modulo(left, right, result, count);
            break;
        default:
            ErrorHandler::errorNotImplemented("Operator not implemented");
    }
}

Vector Expression::executeOperands(vector_vector_t& allColumns, const Operands &op, idx_t count, ConstantType resultType){
    if (op.cols_.size() == 1) {
        Vector result(allColumns[op.cols_[0]]);
        return result;
    }

    vector_vector_t vectors;
    for (auto c: op.cols_) {
        vectors.emplace_back(allColumns[c]);
    }
    // Find the result type of the final vector if is not passed
    if (resultType == UNKNOWN) {
        for (auto c: op.cols_) {
            resultType = getCommonType(resultType, allColumns[c].getType());
            // we need bump the common type as operation can overflow the data
            resultType = getBumpedType(resultType);
        }
        // if result type is unsigned, and we have subtraction
        // set result type as bigint
        bool diff = std::find(op.operators_.begin(), op.operators_.end(), MINUS) != op.operators_.end();
        if (diff && isUnsigned(resultType))
            resultType = BIGINT;
    }

    BB_ASSERT(op.operators_.size() + 1 == vectors.size());
    Vector resultVec(resultType, true, true );
    if (op.operators_.size() == 1) {
        executeOperator(vectors[0], vectors[1], resultVec, count, op.operators_[0]);
        return resultVec;
    }

    Vector lastTermVec(resultType);
    VectorOperations::cast(vectors[0], lastTermVec, count);

    Vector temp(resultType);
    for (idx_t i=0;i<op.operators_.size();++i) {
        auto& nextVec = vectors[i+1];
        if ( op.operators_[i] == Operator::PLUS || op.operators_[i] == Operator::MINUS ) {
            // lower priority operators
            executeOperator(resultVec, lastTermVec, temp, count, Operator::PLUS);
            resultVec.swap(temp);

            VectorOperations::cast(nextVec, lastTermVec, count);
            if (op.operators_[i] == Operator::MINUS)  {
                // multiply -1
                Vector v((int8_t)-1);
                executeOperator(lastTermVec, v, temp, count, Operator::TIMES);
                lastTermVec.swap(temp);
            }
        }else {
            executeOperator(lastTermVec, nextVec, temp, count, op.operators_[i]);
            lastTermVec.swap(temp);
        }
    }
    executeOperator(resultVec, lastTermVec, temp, count, Operator::PLUS);
    resultVec.swap(temp);

    return resultVec;
}

idx_t Expression::executeBinop(Vector& left,Vector& right, SelectionVector& sel, idx_t count) const{
    BB_ASSERT(op_ != ASSIGNMENT && op_ != NONE_OP);
    switch (op_) {
        case EQUAL:
            return VectorOperations::equals(left, right, nullptr, count, &sel);
        case UNEQUAL:
            return VectorOperations::notEquals(left, right, nullptr, count, &sel);
        case GREATER:
            return VectorOperations::greaterThan(left, right, nullptr, count, &sel);
        case GREATER_OR_EQ:
            return VectorOperations::greaterThanEquals(left, right, nullptr, count, &sel);
        case LESS:
            return VectorOperations::lessThan(left, right, nullptr, count, &sel);
        case LESS_OR_EQ:
            return VectorOperations::lessThanEquals(left, right, nullptr, count, &sel);
        default:
            ErrorHandler::errorNotImplemented("Binop not implemented");
            return 0;
    }
    return 0;
}

Expression Expression::generateExpression(Binop op, idx_t leftCol, idx_t rightCol) {
    Operands left, right;
    left.cols_.push_back(leftCol);
    right.cols_.push_back(rightCol);
    Expression exp(op, left, right);
    return exp;
}

}
