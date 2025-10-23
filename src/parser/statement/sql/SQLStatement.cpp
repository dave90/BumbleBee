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
#include "bumblebee/parser/statement/sql/SQLStatement.hpp"

namespace bumblebee{
namespace sql {


SQLStatement::SQLStatement(SQLStatement &&other) noexcept: select_(std::move(other.select_)),
                                                           from_(std::move(other.from_)),
                                                           where_(std::move(other.where_)),
                                                           alias_(std::move(other.alias_)){
}


SQLStatement & SQLStatement::operator=(SQLStatement &&other) noexcept {
    if (this == &other)
        return *this;
    select_ = std::move(other.select_);
    from_ = std::move(other.from_);
    where_ = std::move(other.where_);
    alias_ = std::move(other.alias_);
    return *this;
}

void SQLStatement::setWhere(Where &where) {
    where_ = std::move(where);
}

void SQLStatement::setSelect(Select &select) {
    select_ = std::move(select);
}

void SQLStatement::setFrom(From &from) {
    from_ = std::move(from);
}

void SQLStatement::setAlias(string &alias) {
    alias_ = std::move(alias);
}

string SQLStatement::getAlias() const {
    return alias_;
}

Select& SQLStatement::getSelect() {
    return select_;
}

From& SQLStatement::getFrom()  {
    return from_;
}

Where& SQLStatement::getWhere() {
    return where_;
}

string SQLStatement::toString() const{
    string result = "";
    result += select_.toString() + "\n";
    result += from_.toString() + "\n";
    result += where_.toString();
    return result;
}

void SQLStatement::clear() {
    from_.clear();
    where_.clear();
    select_.clear();
}
}
}
