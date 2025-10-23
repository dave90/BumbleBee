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
#include "bumblebee/common/TypeDefs.hpp"

namespace bumblebee {
namespace sql {

class SQLStatement;

class FromItem {
public:
    FromItem(const string &table_name);
    explicit FromItem(std::unique_ptr<SQLStatement> &statement);
    FromItem(const FromItem &other) = delete;
    FromItem(FromItem &&other) noexcept;

    FromItem & operator=(const FromItem &other) = delete;
    FromItem & operator=(FromItem &&other) noexcept;

    void setAlias(string&);

    string toString() const;

private:
    std::unique_ptr<SQLStatement> statement_;
    string tableName_;
    string alias_;
};

using from_items_t = vector<FromItem>;

class From {
public:
    From() = default;

    From(const From &other) = delete;
    From(From &&other) noexcept;

    From & operator=(const From &other) = delete;
    From & operator=(From &&other) noexcept;

    void addItem(FromItem &item);

    string toString() const;

    void addSubqueries(SQLStatement& sql,string & alias);

    void clear();

private:
    from_items_t items_;
    vector<SQLStatement> subQueries_;
};


}
}