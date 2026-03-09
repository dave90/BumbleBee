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
#include "bumblebee/parser/statement/sql/ValueExpr.hpp"

namespace bumblebee {
namespace sql{

ValuePrimary::ValuePrimary(Value &value): value_(std::move(value)), isConstant_(true) {
}

ValuePrimary::ValuePrimary(QualifiedName &qualifier): qualifier_(std::move(qualifier)) {
}

ValuePrimary::ValuePrimary(ValueExpr&& subExpr): subExpr_(std::make_unique<ValueExpr>(std::move(subExpr))) {
}

ValuePrimary::ValuePrimary(const ValuePrimary &other): value_(other.value_.cast(other.value_.getPhysicalType())),
                                                       qualifier_(other.qualifier_),
                                                       isConstant_(other.isConstant_){
    if (other.subExpr_)
        subExpr_ = std::make_unique<ValueExpr>(*other.subExpr_);
}

ValuePrimary::ValuePrimary(ValuePrimary &&other) noexcept: value_(std::move(other.value_)),
                                                           qualifier_(std::move(other.qualifier_)),
                                                           isConstant_(other.isConstant_),
                                                           subExpr_(std::move(other.subExpr_)) {
}

ValuePrimary & ValuePrimary::operator=(const ValuePrimary &other) {
    if (this == &other)
        return *this;
    value_ = other.value_.cast(other.value_.getPhysicalType());
    qualifier_ = other.qualifier_;
    isConstant_ = other.isConstant_;
    if (other.subExpr_)
        subExpr_ = std::make_unique<ValueExpr>(*other.subExpr_);
    else
        subExpr_.reset();
    return *this;
}

ValuePrimary & ValuePrimary::operator=(ValuePrimary &&other) noexcept {
    if (this == &other)
        return *this;
    value_ = std::move(other.value_);
    qualifier_ = std::move(other.qualifier_);
    isConstant_ = other.isConstant_;
    subExpr_ = std::move(other.subExpr_);
    return *this;
}

Value & ValuePrimary::getValue() {
    return value_;
}

bool & ValuePrimary::isIsConstant() {
    return isConstant_;
}

QualifiedName & ValuePrimary::getQualifier() {
    return qualifier_;
}

void ValuePrimary::setQualifier(QualifiedName &qualifier) {
    qualifier_ = qualifier;
}

bool ValuePrimary::isSubExpr() const {
    return subExpr_ != nullptr;
}

ValueExpr& ValuePrimary::getSubExpr() {
    return *subExpr_;
}

string ValuePrimary::toString() const {
    if (subExpr_)
        return "(" + subExpr_->toString(false) + ")";
    if (isConstant_) {
        return value_.toString();
    }
    if (qualifier_.table_.empty())
        return qualifier_.name_ ;
    return qualifier_.table_+"."+qualifier_.name_;
}

ValueExpr::ValueExpr(const ValueExpr &other): values_(other.values_),
                                              operators_(other.operators_),
                                              alias_(other.alias_) {
}

ValueExpr::ValueExpr(const QualifiedName &qualifier) {
    auto q = qualifier;
    ValuePrimary vp(q);
    addValuePrimary(vp);
}

ValueExpr::ValueExpr(ValueExpr &&other) noexcept: values_(std::move(other.values_)),
                                                  operators_(std::move(other.operators_)),
                                                  alias_(std::move(other.alias_)) {
}

ValueExpr & ValueExpr::operator=(const ValueExpr &other) {
    if (this == &other)
        return *this;
    values_ = other.values_;
    operators_ = other.operators_;
    alias_ = other.alias_;
    return *this;
}

ValueExpr & ValueExpr::operator=(ValueExpr &&other) noexcept {
    if (this == &other)
        return *this;
    values_ = std::move(other.values_);
    operators_ = std::move(other.operators_);
    alias_ = std::move(other.alias_);
    return *this;
}

void ValueExpr::addValuePrimary(ValuePrimary &vp) {
    values_.push_back(std::move(vp));
}

void ValueExpr::addOperator(Operator op) {
    operators_.push_back(op);
}

string ValueExpr::toString(bool alias) const {
    if (values_.empty())return "";
    string result = values_[0].toString();
    for (idx_t i =0;i<operators_.size();i++) {
        string op{getOperatorChar(operators_[i])};
        result += " "+op + " "+ values_[i+1].toString();
    }
    if (!alias_.empty() && alias)
        result += " AS " + alias_;
    return result;
}

vector<ValuePrimary>& ValueExpr::getValues() {
    return values_;
}

vector<Operator>& ValueExpr::getOperators() {
    return operators_;
}

string ValueExpr::getAlias() const {
    return alias_;
}

void ValueExpr::setAlias(string &alias) {
    alias_ = alias;
}

void ValueExpr::setAlias(const string &alias) {
    alias_ = alias;
}

void ValueExpr::clear() {
    operators_.clear();
    values_.clear();
    alias_.clear();
}

}
}
