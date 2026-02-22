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

// SQL-specific binop that extends the core Binop with SQL-only operators.
// LIKE (value 8) is SQL-only and must NOT be passed to Datalog builtin atoms.
enum SQLBinop {
    SQL_NONE_OP = 0,
    SQL_EQUAL = 1,
    SQL_UNEQUAL = 2,
    SQL_LESS = 3,
    SQL_GREATER = 4,
    SQL_LESS_OR_EQ = 5,
    SQL_GREATER_OR_EQ = 6,
    SQL_ASSIGNMENT = 7,
    SQL_LIKE = 8,
};

inline SQLBinop toSQLBinop(Binop op) {
    return static_cast<SQLBinop>(op);
}

inline Binop toCoreBinop(SQLBinop op) {
    return static_cast<Binop>(op);
}

class Predicate {
public:
    Predicate() = default;
    Predicate(const Predicate &other);
    Predicate(Predicate &&other) noexcept;

    Predicate & operator=(const Predicate &other);
    Predicate & operator=(Predicate &&other) noexcept;

    void setOp(SQLBinop op);
    void setValue1(ValueExpr &value1);
    void setValue2(ValueExpr &value2);

    SQLBinop & getOp();

    ValueExpr & getValue1();

    ValueExpr & getValue2();

    string toString() const;


private:
    SQLBinop op_{SQL_NONE_OP};
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
    vector<QualifiedName> getQualifiedNames();

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
