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
#include "bumblebee/parser/statement/sql/OrderBy.hpp"

#include "bumblebee/parser/statement/sql/Select.hpp"

namespace bumblebee{
namespace sql {
OrderBy::OrderBy(const OrderBy &other): colModifiers_(other.colModifiers_) {
}

OrderBy::OrderBy(OrderBy &&other) noexcept: colModifiers_(std::move(other.colModifiers_)) {
}

OrderBy & OrderBy::operator=(const OrderBy &other) {
    if (this == &other)
        return *this;
    colModifiers_ = other.colModifiers_;
    return *this;
}

OrderBy & OrderBy::operator=(OrderBy &&other) noexcept {
    if (this == &other)
        return *this;
    colModifiers_ = std::move(other.colModifiers_);
    return *this;
}

vector<ColModifier> & OrderBy::getColModifiers() {
    return colModifiers_;
}

void OrderBy::addColModifier(ColModifier &colModifier) {
    colModifiers_.push_back(std::move(colModifier));
}

bool OrderBy::empty() const {
    return colModifiers_.empty();
}

string OrderBy::toString(Select sqlSelect) const {
    if (empty()) return "";
    string result = "ORDER BY ";
    idx_t i = 0;
    for (auto& cm:colModifiers_) {
        if (i>0) result += ", ";
        BB_ASSERT(cm.col_ < sqlSelect.getItems().size());
        auto& ve = sqlSelect.getItems()[cm.col_];
        if (ve.getAlias().empty()) {
            result += ve.toString()+" ";
        }else
            result += ve.getAlias()+" ";
        result += cm.modifier_.toString();
        ++i;
    }
    return result;
}

}
}
