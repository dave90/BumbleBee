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
#include "bumblebee/common/types/Value.hpp"

namespace bumblebee {
namespace sql{

struct QualifiedName {
    string name_;
    string table_;
};

class ValuePrimary {
public:
    explicit ValuePrimary(Value &value);
    explicit ValuePrimary(QualifiedName &qualifier);
    ValuePrimary(const ValuePrimary &other);
    ValuePrimary(ValuePrimary &&other) noexcept;

    ValuePrimary & operator=(const ValuePrimary &other);
    ValuePrimary & operator=(ValuePrimary &&other) noexcept;

    string toString() const;

private:
    Value value_;
    bool isConstant_{false};

    QualifiedName qualifier_;
};

class ValueExpr {
public:
    ValueExpr() = default;

    ValueExpr(const ValueExpr &other);
    ValueExpr(ValueExpr &&other) noexcept;

    ValueExpr & operator=(const ValueExpr &other);
    ValueExpr & operator=(ValueExpr &&other) noexcept;

    void addValuePrimary(ValuePrimary& vp);
    void addOperator(Operator op);

    void setAlias(string& alias);

    string toString() const;

    vector<ValuePrimary> getValues() const;
    vector<Operator> getOperators() const;
    string getAlias() const;

    void clear();

private:
    vector<ValuePrimary> values_;
    vector<Operator> operators_;

    string alias_;
};

using value_expr_vector_t = vector<ValueExpr>;

}
}