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
#pragma once
#include "ValueExpr.hpp"
#include "bumblebee/common/TypeDefs.hpp"
#include "bumblebee/parser/statement/Term.hpp"

namespace bumblebee {
namespace sql{

class Select {
public:
    Select() = default;
    Select(const Select &other);
    Select(Select &&other) noexcept;

    Select & operator=(const Select &other);
    Select & operator=(Select &&other) noexcept;

    void addItem(ValueExpr& item);
    void clear();

    string toString() const;

private:
    value_expr_vector_t items_;
};

}
}
