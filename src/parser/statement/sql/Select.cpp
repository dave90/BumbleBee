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
#include "bumblebee/parser/statement/sql/Select.hpp"

namespace bumblebee{
namespace sql {


Select::Select(const Select &other): items_(other.items_), aggFunctions_(other.aggFunctions_) {
}

Select::Select(Select &&other) noexcept: items_(std::move(other.items_)), aggFunctions_(std::move(other.aggFunctions_)) {
}

Select & Select::operator=(const Select &other) {
    if (this == &other)
        return *this;
    items_ = other.items_;
    aggFunctions_ = other.aggFunctions_;
    return *this;
}

Select & Select::operator=(Select &&other) noexcept {
    if (this == &other)
        return *this;
    items_ = std::move(other.items_);
    aggFunctions_ = std::move(other.aggFunctions_);
    return *this;
}

void Select::addItem(ValueExpr &item) {
    items_.push_back(std::move(item));
    if (items_.size() > aggFunctions_.size()) aggFunctions_.push_back(NONE);
}

void Select::addAggFunction(AggregateFunctionType agg) {
    aggFunctions_.push_back(agg);
}

string Select::toString() const{
    string result = "SELECT ";
    idx_t idx = 0;
    for (auto& item : items_) {
        if(idx > 0)result += ", ";
        if (aggFunctions_[idx] != NONE) {
            result += Atom::getAggFunction(aggFunctions_[idx]) + "( ";
        }
        result += item.toString(false);
        if (aggFunctions_[idx] != NONE) {
            result += " )";
        }
        if (!item.getAlias().empty())
            result += " AS " + item.getAlias();
        idx++;
    }
    return result;
}

void Select::clear() {
    items_.clear();
}

value_expr_vector_t & Select::getItems() {
    return items_;
}


vector<AggregateFunctionType> & Select::getAggFunctions() {
    return aggFunctions_;
}


bool Select::containsAggregations() {
    for (auto& item : aggFunctions_)
        if (item != NONE) return true;
    return false;

}
}
}
