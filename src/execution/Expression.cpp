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

Expression::Expression(const Expression &other): op_(other.op_),
                                                 left_(other.left_),
                                                 right_(other.right_) {
}

Expression & Expression::operator=(const Expression &other) {
    if (this == &other)
        return *this;
    op_ = other.op_;
    left_ = other.left_;
    right_ = other.right_;
    return *this;
}

Expression & Expression::operator=(Expression &&other) noexcept {
    if (this == &other)
        return *this;
    op_ = other.op_;
    left_ = std::move(other.left_);
    right_ = std::move(other.right_);
    return *this;
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
        case Operator::PLUS:
            VectorOperations::sum(left, right, result, count);
            break;
        case Operator::MINUS:
            VectorOperations::difference(left, right, result, count);
            break;
        case Operator::TIMES:
            VectorOperations::dot(left, right, result, count);
            break;
        case Operator::DIV:
            VectorOperations::division(left, right, result, count);
            break;
        case Operator::MODULO:
            VectorOperations::modulo(left, right, result, count);
            break;
        default:
            ErrorHandler::errorNotImplemented("Operator not implemented");
    }
}

Vector Expression::executeOperands(vector_vector_t& allColumns, const Operands &op, idx_t count, LogicalType resultType){
    if (op.cols_.size() == 1) {
        Vector result(allColumns[op.cols_[0]]);
        return result;
    }

    vector_vector_t vectors;
    for (auto c: op.cols_) {
        vectors.emplace_back(allColumns[c]);
    }

    BB_ASSERT(op.operators_.size() + 1 == vectors.size());
    if (op.operators_.size() == 1) {
        if (resultType == LogicalTypeId::UNKNOWN) {
            resultType = getCommonType(vectors[0].getLogicalType(), vectors[1].getLogicalType(), op.operators_[0]);
            resultType = getBumpedLogicalType(resultType);
        }
        Vector resultVec(resultType );
        executeOperator(vectors[0], vectors[1], resultVec, count, op.operators_[0]);
        return resultVec;
    }

    Vector lastTermVec(vectors[0]);
    Vector resultVec(vectors[0].getLogicalType(), true, true );

    for (idx_t i=0;i<op.operators_.size();++i) {
        auto& nextVec = vectors[i+1];
        if ( op.operators_[i] == Operator::PLUS || op.operators_[i] == Operator::MINUS ) {
            // lower priority operators
            auto tempType = getCommonType(resultVec.getLogicalType(), lastTermVec.getLogicalType(), op.operators_[i]);
            tempType = getBumpedLogicalType(tempType);
            Vector temp(tempType);
            executeOperator(resultVec, lastTermVec, temp, count, Operator::PLUS);
            resultVec.reference(temp);
            lastTermVec.reference(nextVec);

            if (op.operators_[i] == Operator::MINUS)  {
                // multiply -1
                tempType = lastTermVec.getLogicalType();
                if (isUnsigned(tempType))
                    tempType = getSignedBumpedType(tempType);
                Vector temp2(tempType);
                VectorOperations::negate(lastTermVec, temp2, count);
                lastTermVec.reference(temp2);
            }
        }else {
            auto tempType = getCommonType(lastTermVec.getLogicalType(), nextVec.getLogicalType(), op.operators_[i]);
            tempType = getBumpedLogicalType(tempType);
            Vector temp(tempType);

            executeOperator(lastTermVec, nextVec, temp, count, op.operators_[i]);
            lastTermVec.reference(temp);
        }
    }
    if (resultType == LogicalTypeId::UNKNOWN) {
        resultType = getCommonType(resultVec.getLogicalType(), lastTermVec.getLogicalType(), Operator::PLUS);
        resultType = getBumpedLogicalType(resultType);
    }
    Vector temp(resultType);
    executeOperator(resultVec, lastTermVec, temp, count, Operator::PLUS);
    resultVec.reference(temp);

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


void Expression::executeBinop(Vector& left,Vector& right,SelectionVector& sel, idx_t count,
        SelectionVector& trueSel,SelectionVector& falseSel, idx_t& falseCount) const {

    BB_ASSERT(op_ != ASSIGNMENT && op_ != NONE_OP);
    switch (op_) {
        case EQUAL:
            VectorOperations::equals(left, right, &sel, count, &trueSel, &falseSel, falseCount);
            break;
        case UNEQUAL:
            VectorOperations::notEquals(left, right, &sel, count, &trueSel, &falseSel, falseCount);
            break;
        case GREATER:
            VectorOperations::greaterThan(left, right, &sel, count, &trueSel, &falseSel, falseCount);
            break;
        case GREATER_OR_EQ:
            VectorOperations::greaterThanEquals(left, right, &sel, count, &trueSel, &falseSel, falseCount);
            break;
        case LESS:
            VectorOperations::lessThan(left, right, &sel, count, &trueSel, &falseSel, falseCount);
            break;
        case LESS_OR_EQ:
            VectorOperations::lessThanEquals(left, right, &sel, count, &trueSel, &falseSel, falseCount);
            break;
        default:
            ErrorHandler::errorNotImplemented("Binop not implemented");
    }
}

Expression Expression::generateExpression(Binop op, idx_t leftCol, idx_t rightCol) {
    Operands left, right;
    left.cols_.push_back(leftCol);
    right.cols_.push_back(rightCol);
    Expression exp(op, left, right);
    return exp;
}


LogicalType Expression::getResultType(vector<LogicalType> types, vector<Operator> ops) {
    if (types.empty()) return LogicalTypeId::UNKNOWN;
    if (types.size() == 1) return types[0];
    BB_ASSERT(types.size() == ops.size() + 1);
    if (types.size() == 2) {
        auto resultType = getCommonType(types[0], types[1], ops[0]);
        return getBumpedLogicalType(resultType);
    }

    LogicalType lastTermType(types[0]);
    LogicalType resultType(types[0]);
    for (idx_t i=0;i<ops.size();++i) {
        auto& nextType = types[i+1];
        if ( ops[i] == Operator::PLUS || ops[i] == Operator::MINUS ) {
            // lower priority operators
            resultType = getCommonType(resultType, lastTermType, ops[i]);
            resultType = getBumpedLogicalType(resultType);
            lastTermType = nextType;

            if (ops[i] == Operator::MINUS)  {
                // multiply -1
                if (isUnsigned(lastTermType))
                    lastTermType = getSignedBumpedType(lastTermType);
            }
        }else {
            lastTermType = getCommonType(lastTermType, nextType, ops[i]);
            lastTermType = getBumpedLogicalType(lastTermType);
        }
    }
    resultType = getCommonType(resultType, lastTermType, Operator::PLUS);
    resultType = getBumpedLogicalType(resultType);
    return resultType;
}

}
