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
#include "From.hpp"
#include "Select.hpp"
#include "bumblebee/parser/statement/sql/Where.hpp"

namespace bumblebee{
namespace sql {
class SQLStatement {
public:
    SQLStatement() = default;
    SQLStatement(const SQLStatement &other) = delete;
    SQLStatement(SQLStatement &&other) noexcept;

    SQLStatement & operator=(const SQLStatement &other) = delete;
    SQLStatement & operator=(SQLStatement &&other) noexcept;

    void setWhere(Where &where);
    void setSelect(Select &select);
    void setFrom(From &from);
    void setAlias(string &alias);
    string getAlias() const;

    Select& getSelect() ;
    From& getFrom() ;
    Where& getWhere() ;

    string toString() const;

    void clear();

private:
    Select select_;
    From from_;
    Where where_;

    string alias_;
};
}
}
