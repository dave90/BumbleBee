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
#include "bumblebee/parser/statement/sql/Where.hpp"

namespace bumblebee{
namespace sql {


Predicate::Predicate(const Predicate &other): op_(other.op_),
                                                  value1_(other.value1_),
                                                  value2_(other.value2_) {
}

Predicate::Predicate(Predicate &&other) noexcept: op_(other.op_),
                                                  value1_(std::move(other.value1_)),
                                                  value2_(std::move(other.value2_)) {
}

Predicate & Predicate::operator=(const Predicate &other) {
    if (this == &other)
        return *this;
    op_ = other.op_;
    value1_ = other.value1_;
    value2_ = other.value2_;
    return *this;
}

Predicate & Predicate::operator=(Predicate &&other) noexcept {
    if (this == &other)
        return *this;
    op_ = other.op_;
    value1_ = std::move(other.value1_);
    value2_ = std::move(other.value2_);
    return *this;
}

void Predicate::setOp(Binop op) {
    op_ = op;
}

void Predicate::setValue1(ValueExpr &value1) {
    value1_ = std::move(value1);
}

void Predicate::setValue2(ValueExpr &value2) {
    value2_ = std::move(value2);
}

Binop & Predicate::getOp() {
    return op_;
}

ValueExpr & Predicate::getValue1() {
    return value1_;
}

ValueExpr & Predicate::getValue2() {
    return value2_;
}


string Predicate::toString() const {
    return value1_.toString() + " " + getBinopStr(op_) +" "+ value2_.toString();
}

Where::Where(const Where &other): items_(other.items_),
                                  ops_(other.ops_) {
}

Where::Where(Where &&other) noexcept: items_(std::move(other.items_)),
                                      ops_(std::move(other.ops_)) {
}

Where & Where::operator=(const Where &other) {
    if (this == &other)
        return *this;
    items_ = other.items_;
    ops_ = other.ops_;
    return *this;
}

Where & Where::operator=(Where &&other) noexcept {
    if (this == &other)
        return *this;
    items_ = std::move(other.items_);
    ops_ = std::move(other.ops_);
    return *this;
}


void Where::addItem(Predicate &condition) {
    items_.push_back(std::move(condition));
}

void Where::addOperator(SQLOperator op) {
    ops_.push_back(op);
}

predicate_vector_t & Where::getItems() {
    return items_;
}

vector<QualifiedName> Where::getQualifiedNames() {
    vector<QualifiedName> names;
    for (auto& p:getItems()) {
        for (auto& vp: p.getValue1().getValues()) {
            if (vp.isIsConstant())continue;
            names.push_back(vp.getQualifier());
        }
        for (auto& vp: p.getValue2().getValues()) {
            if (vp.isIsConstant())continue;
            names.push_back(vp.getQualifier());
        }
    }
    return names;
}

vector<SQLOperator> & Where::getOps() {
    return ops_;
}

SQLOperator Where::getOp(string &op) {
    if (op == "AND")
        return SQL_AND;
    return SQL_OR;
}

string Where::getStringFromOp(SQLOperator op) {
    switch (op) {
        case SQL_AND:
            return "AND";
        case SQL_OR:
            return "OR";
    }
    ErrorHandler::errorNotImplemented("Invalid type conversion from SQLOperator");
    return "";
}

string Where::toString() const {
    if (items_.empty())return "";
    string result = "WHERE ";
    result += items_[0].toString();
    for (idx_t i =0;i<ops_.size();i++) {
        string op{getStringFromOp(ops_[i])};
        result += " "+op + " "+ items_[i+1].toString();
    }
    return result;
}

void Where::clear() {
    items_.clear();
    ops_.clear();
}
}
}
