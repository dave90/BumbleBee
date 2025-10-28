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

namespace bumblebee{
namespace sql {

enum SQLOperator {
    SQL_AND,
    SQL_OR,
};

class Predicate {
public:
    Predicate() = default;
    Predicate(const Predicate &other);
    Predicate(Predicate &&other) noexcept;

    Predicate & operator=(const Predicate &other);
    Predicate & operator=(Predicate &&other) noexcept;

    void setOp(Binop op);
    void setValue1(ValueExpr &value1);
    void setValue2(ValueExpr &value2);

    Binop & getOp();

    ValueExpr & getValue1();

    ValueExpr & getValue2();

    string toString() const;


private:
    Binop op_;
    ValueExpr value1_;
    ValueExpr value2_;
};

using predicate_vector_t = std::vector<Predicate>;

class Where {
public:
    Where() = default;
    Where(const Where &other);
    Where(Where &&other) noexcept;

    Where & operator=(const Where &other);
    Where & operator=(Where &&other) noexcept;

    void addItem(Predicate& condition);
    void addOperator(SQLOperator op);

    predicate_vector_t & getItems();

    vector<SQLOperator> & getOps();

    string toString()const;

    static SQLOperator getOp(string& op);
    static string getStringFromOp(SQLOperator op);

    void clear();

private:
    predicate_vector_t items_;
    vector<SQLOperator> ops_;
};


}
}
