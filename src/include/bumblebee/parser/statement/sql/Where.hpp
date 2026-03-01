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
#include <memory>
#include <variant>
#include "ValueExpr.hpp"
#include "bumblebee/common/TypeDefs.hpp"

namespace bumblebee{
namespace sql {

// Forward declaration to break circular include
// (SQLStatement.hpp includes Where.hpp, SubqueryPredicate needs SQLStatement)
class SQLStatement;

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

// A scalar subquery predicate: LHS_value OP (SELECT agg FROM ...)
// Uses unique_ptr<SQLStatement> to break the circular include dependency.
struct SubqueryPredicate {
    SubqueryPredicate();
    SubqueryPredicate(const SubqueryPredicate&);
    SubqueryPredicate(SubqueryPredicate&&) noexcept;
    SubqueryPredicate& operator=(const SubqueryPredicate&);
    SubqueryPredicate& operator=(SubqueryPredicate&&) noexcept;
    ~SubqueryPredicate();

    SQLBinop op_{SQL_NONE_OP};
    ValueExpr value_;                          // Left-hand side (column reference)
    std::unique_ptr<SQLStatement> subquery_;   // Inner SELECT (scalar subquery)
};

// An IN / NOT IN predicate: LHS_value [NOT] IN (const_list | subquery)
struct InPredicate {
    InPredicate();
    InPredicate(const InPredicate&);
    InPredicate(InPredicate&&) noexcept;
    InPredicate& operator=(const InPredicate&);
    InPredicate& operator=(InPredicate&&) noexcept;
    ~InPredicate();

    bool isNotIn_{false};
    ValueExpr value_;                          // Left-hand side (column reference)
    vector<ValueExpr> values_;                 // Constant list (empty if subquery)
    std::unique_ptr<SQLStatement> subquery_;   // Inner SELECT (nullptr if constant list)
};

// Forward-declare Where so WhereGroup can hold a unique_ptr<Where>
class Where;

// WhereGroup wraps a parenthesized nested WHERE condition.
// Uses unique_ptr<Where> to break the circular type dependency.
class WhereGroup {
public:
    WhereGroup();
    explicit WhereGroup(Where where);
    ~WhereGroup();
    WhereGroup(const WhereGroup &other);
    WhereGroup(WhereGroup &&other) noexcept;
    WhereGroup & operator=(const WhereGroup &other);
    WhereGroup & operator=(WhereGroup &&other) noexcept;

    Where & getWhere();
    const Where & getWhere() const;

private:
    std::unique_ptr<Where> where_;
};

// A WhereItem is a flat Predicate, a parenthesized WhereGroup, a scalar SubqueryPredicate,
// or an IN/NOT IN predicate.
using WhereItem = std::variant<Predicate, WhereGroup, SubqueryPredicate, InPredicate>;
using predicate_vector_t = std::vector<WhereItem>;

class Where {
public:
    Where() = default;
    Where(const Where &other);
    Where(Where &&other) noexcept;

    Where & operator=(const Where &other);
    Where & operator=(Where &&other) noexcept;

    void addItem(Predicate& condition);
    void addGroup(WhereGroup group);
    void addSubqueryPredicate(SubqueryPredicate sp);
    void addInPredicate(InPredicate ip);
    void addOperator(SQLOperator op);

    predicate_vector_t & getItems();
    vector<QualifiedName> getQualifiedNames();

    vector<SQLOperator> & getOps();

    string toString() const;
    string toStringCondition() const;

    static SQLOperator getOp(string& op);
    static string getStringFromOp(SQLOperator op);

    void clear();

private:
    predicate_vector_t items_;
    vector<SQLOperator> ops_;
};


}
}
