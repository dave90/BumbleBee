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
#include "bumblebee/parser/statement/sql/SQLStatement.hpp"

namespace bumblebee{
namespace sql {

// ---- SubqueryPredicate implementation ----

SubqueryPredicate::SubqueryPredicate() = default;

SubqueryPredicate::SubqueryPredicate(const SubqueryPredicate& other)
    : op_(other.op_), value_(other.value_),
      subquery_(other.subquery_ ? std::make_unique<SQLStatement>(*other.subquery_) : nullptr) {}

SubqueryPredicate::SubqueryPredicate(SubqueryPredicate&& other) noexcept = default;

SubqueryPredicate& SubqueryPredicate::operator=(const SubqueryPredicate& other) {
    if (this == &other) return *this;
    op_ = other.op_;
    value_ = other.value_;
    subquery_ = other.subquery_ ? std::make_unique<SQLStatement>(*other.subquery_) : nullptr;
    return *this;
}

SubqueryPredicate& SubqueryPredicate::operator=(SubqueryPredicate&& other) noexcept = default;

SubqueryPredicate::~SubqueryPredicate() = default;

// ---- InPredicate implementation ----

InPredicate::InPredicate() = default;

InPredicate::InPredicate(const InPredicate& other)
    : isNotIn_(other.isNotIn_), value_(other.value_), values_(other.values_),
      subquery_(other.subquery_ ? std::make_unique<SQLStatement>(*other.subquery_) : nullptr) {}

InPredicate::InPredicate(InPredicate&& other) noexcept = default;

InPredicate& InPredicate::operator=(const InPredicate& other) {
    if (this == &other) return *this;
    isNotIn_ = other.isNotIn_;
    value_ = other.value_;
    values_ = other.values_;
    subquery_ = other.subquery_ ? std::make_unique<SQLStatement>(*other.subquery_) : nullptr;
    return *this;
}

InPredicate& InPredicate::operator=(InPredicate&& other) noexcept = default;

InPredicate::~InPredicate() = default;


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

void Predicate::setOp(SQLBinop op) {
    op_ = op;
}

void Predicate::setValue1(ValueExpr &value1) {
    value1_ = std::move(value1);
}

void Predicate::setValue2(ValueExpr &value2) {
    value2_ = std::move(value2);
}

SQLBinop & Predicate::getOp() {
    return op_;
}

ValueExpr & Predicate::getValue1() {
    return value1_;
}

ValueExpr & Predicate::getValue2() {
    return value2_;
}


string Predicate::toString() const {
    string opStr = (op_ == SQL_LIKE) ? "LIKE" : getBinopStr(toCoreBinop(op_));
    return value1_.toString() + " " + opStr +" "+ value2_.toString();
}

// ---- WhereGroup implementation ----
// Defined after Where because WhereGroup methods need Where to be complete.

WhereGroup::WhereGroup() : where_(std::make_unique<Where>()) {}

WhereGroup::WhereGroup(Where where) : where_(std::make_unique<Where>(std::move(where))) {}

WhereGroup::~WhereGroup() = default;

WhereGroup::WhereGroup(const WhereGroup &other)
    : where_(std::make_unique<Where>(*other.where_)) {}

WhereGroup::WhereGroup(WhereGroup &&other) noexcept = default;

WhereGroup & WhereGroup::operator=(const WhereGroup &other) {
    if (this == &other) return *this;
    where_ = std::make_unique<Where>(*other.where_);
    return *this;
}

WhereGroup & WhereGroup::operator=(WhereGroup &&other) noexcept = default;

Where & WhereGroup::getWhere() { return *where_; }
const Where & WhereGroup::getWhere() const { return *where_; }

// ---- Where implementation ----

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

void Where::addGroup(WhereGroup group) {
    items_.push_back(std::move(group));
}

void Where::addSubqueryPredicate(SubqueryPredicate sp) {
    items_.push_back(std::move(sp));
}

void Where::addInPredicate(InPredicate ip) {
    items_.push_back(std::move(ip));
}

void Where::addOperator(SQLOperator op) {
    ops_.push_back(op);
}

predicate_vector_t & Where::getItems() {
    return items_;
}

static void collectQualifiedNames(vector<QualifiedName>& names, ValueExpr& ve) {
    for (auto& vp: ve.getValues()) {
        if (vp.isSubExpr()) {
            collectQualifiedNames(names, vp.getSubExpr());
        } else if (!vp.isIsConstant()) {
            names.push_back(vp.getQualifier());
        }
    }
}

static void collectQualifiedNamesFromItem(vector<QualifiedName>& names, WhereItem& item) {
    std::visit([&](auto& wi) {
        using T = std::decay_t<decltype(wi)>;
        if constexpr (std::is_same_v<T, Predicate>) {
            collectQualifiedNames(names, wi.getValue1());
            collectQualifiedNames(names, wi.getValue2());
        } else if constexpr (std::is_same_v<T, SubqueryPredicate>) {
            // Only collect the outer LHS column reference; the subquery has its own scope.
            collectQualifiedNames(names, const_cast<ValueExpr&>(wi.value_));
        } else if constexpr (std::is_same_v<T, InPredicate>) {
            // Collect the outer LHS column; the constant list and subquery have their own scope.
            collectQualifiedNames(names, wi.value_);
            for (auto& ve: wi.values_)
                collectQualifiedNames(names, const_cast<ValueExpr&>(ve));
        } else { // WhereGroup
            for (auto& inner: wi.getWhere().getItems())
                collectQualifiedNamesFromItem(names, inner);
        }
    }, item);
}

vector<QualifiedName> Where::getQualifiedNames() {
    vector<QualifiedName> names;
    for (auto& item: items_) {
        collectQualifiedNamesFromItem(names, item);
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

string Where::toStringCondition() const {
    if (items_.empty()) return "";
    auto itemStr = [](const WhereItem& item) -> string {
        return std::visit([](const auto& wi) -> string {
            using T = std::decay_t<decltype(wi)>;
            if constexpr (std::is_same_v<T, Predicate>) {
                return wi.toString();
            } else if constexpr (std::is_same_v<T, SubqueryPredicate>) {
                string opStr = getBinopStr(toCoreBinop(wi.op_));
                return wi.value_.toString() + " " + opStr + " (SELECT ...)";
            } else if constexpr (std::is_same_v<T, InPredicate>) {
                string s = wi.value_.toString() + (wi.isNotIn_ ? " NOT IN " : " IN ");
                if (wi.subquery_) {
                    s += "(SELECT ...)";
                } else {
                    s += "(";
                    for (idx_t i = 0; i < wi.values_.size(); ++i) {
                        if (i > 0) s += ", ";
                        s += wi.values_[i].toString();
                    }
                    s += ")";
                }
                return s;
            } else { // WhereGroup
                return "(" + wi.getWhere().toStringCondition() + ")";
            }
        }, item);
    };
    string result = itemStr(items_[0]);
    for (idx_t i = 0; i < ops_.size(); i++) {
        string op{getStringFromOp(ops_[i])};
        result += " " + op + " " + itemStr(items_[i+1]);
    }
    return result;
}

string Where::toString() const {
    if (items_.empty()) return "";
    return "WHERE " + toStringCondition();
}

void Where::clear() {
    items_.clear();
    ops_.clear();
}
}
}
