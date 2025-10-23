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
#include "bumblebee/parser/statement/sql/From.hpp"
#include "bumblebee/parser/statement/sql/SQLStatement.hpp"

namespace bumblebee{
namespace sql {
FromItem::FromItem(const string &table_name): tableName_(table_name) {
}

FromItem::FromItem(std::unique_ptr<SQLStatement> &statement): statement_(std::move(statement)) {
}

FromItem::FromItem(FromItem &&other) noexcept: statement_(std::move(other.statement_)),
                                                tableName_(std::move(other.tableName_)),
                                                alias_(std::move(other.alias_)) {
}

FromItem &FromItem::operator=(FromItem &&other) noexcept {
    if (this == &other)
        return *this;
    statement_ = std::move(other.statement_);
    tableName_ = std::move(other.tableName_);
    alias_ = std::move(other.alias_);
    return *this;
}


string FromItem::toString() const{
    string result = "";
    if (statement_) {
        result = "( " + statement_->toString() + ")";
    } else {
        result = tableName_;
        if (!alias_.empty())
            result += " AS " + alias_;
    }

    return result;
}

void FromItem::setAlias(string& alias) {
    alias_ = alias;
}


From::From(From &&other) noexcept: items_(std::move(other.items_)),
                                   subQueries_(std::move(other.subQueries_)) {
}


From & From::operator=(From &&other) noexcept {
    if (this == &other)
        return *this;
    items_ = std::move(other.items_);
    subQueries_ = std::move(other.subQueries_);
    return *this;
}

void From::addItem(FromItem &item) {
    items_.push_back(std::move(item));
}

string From::toString() const {
    string result = "FROM ";
    idx_t idx = 0;
    for (auto& item:items_) {
        if (idx > 0) result += ", ";
        result += item.toString();
        ++idx;
    }

    for (auto& item:subQueries_) {
        if (idx > 0) result += ", ";
        result += "( "+ item.toString() + ") ";
        auto alias = item.getAlias();
        if (!alias.empty()) result += " AS " + alias;
        ++idx;
    }
    return result;
}

void From::clear() {
    items_.clear();
}

void From::addSubqueries(SQLStatement &sql, string& alias) {
    sql.setAlias(alias);
    subQueries_.push_back(std::move(sql));
}
}
}
