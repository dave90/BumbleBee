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
#include "Groupby.hpp"

#include "bumblebee/common/types/Assert.hpp"

namespace bumblebee{
namespace sql{

Groupby::Groupby(Groupby &&other) noexcept: items_(std::move(other.items_)) {
}

Groupby & Groupby::operator=(Groupby &&other) noexcept {
    if (this == &other)
        return *this;
    items_ = std::move(other.items_);
    return *this;
}

bool Groupby::isEmpty() {
    return items_.empty();
}

value_expr_vector_t & Groupby::getItems() {
    return items_;
}

vector<QualifiedName> Groupby::getQualifiedNames() {
    vector<QualifiedName> result;
    for (auto& ve: items_) {
        BB_ASSERT(ve.getValues().size() == 1);
        auto vp =ve.getValues()[0];
        BB_ASSERT(!vp.isIsConstant());
        result.push_back(vp.getQualifier());
    }
    return result;
}

void Groupby::addItem(ValueExpr &item) {
    items_.push_back(std::move(item));
}

string Groupby::toString() const {
    if (items_.empty())return "";
    string result = "GROUP BY ";
    idx_t idx = 0;
    for (auto& item : items_) {
        if(idx > 0)result += ", ";
        result += item.toString();
        idx++;
    }
    return result;
}
}
}
