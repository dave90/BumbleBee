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


Select::Select(const Select &other): items_(other.items_) {
}

Select::Select(Select &&other) noexcept: items_(std::move(other.items_)) {
}

Select & Select::operator=(const Select &other) {
    if (this == &other)
        return *this;
    items_ = other.items_;
    return *this;
}

Select & Select::operator=(Select &&other) noexcept {
    if (this == &other)
        return *this;
    items_ = std::move(other.items_);
    return *this;
}

void Select::addItem(ValueExpr &item) {
    items_.push_back(std::move(item));
}

string Select::toString() const{
    string result = "SELECT ";
    idx_t idx = 0;
    for (auto& item : items_) {
        if(idx > 0)result += ", ";
        result += item.toString();
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
}
}
