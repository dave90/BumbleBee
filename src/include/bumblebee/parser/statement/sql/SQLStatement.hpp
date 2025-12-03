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
#include "Groupby.hpp"
#include "OrderBy.hpp"
#include "Select.hpp"
#include "bumblebee/execution/TopNHeap.hpp"
#include "bumblebee/parser/statement/sql/Where.hpp"

namespace bumblebee{
namespace sql {
class SQLStatement {
public:
    SQLStatement() = default;
    SQLStatement(SQLStatement &&other) noexcept;
    SQLStatement(const SQLStatement &other);

    SQLStatement & operator=(const SQLStatement &other);
    SQLStatement & operator=(SQLStatement &&other) noexcept;

    void setWhere(Where &where);
    void setSelect(Select &select);
    void setFrom(From &from);
    void setGroupby(Groupby& groupby);
    void setOrderby(OrderBy& orderby);

    void setExportPath(string& exportPath);

    void setExportNamedParameters(std::unordered_map<string, Value> & exportNamedParameters);

    bool containsAggregations();
    idx_t getNumAggregations();

    void setAlias(const string &alias);
    string getAlias() const;

    void clearGroupBy();
    void clearOrderBy();
    void clearWhere();

    Select& getSelect() ;
    From& getFrom() ;
    Where& getWhere() ;
    Groupby& getGroupby() ;
    OrderBy& getOrderby() ;
    string & getExportPath();
    std::unordered_map<string, Value> & getExportNamedParameters();
    idx_t & getLimit();
    void setLimit(idx_t limit);

    string toString() const;

    void clear();

private:
    Select select_;
    From from_;
    Where where_;
    Groupby groupby_;
    OrderBy orderby_;

    string alias_;
    string exportPath_;
    std::unordered_map<string, Value> exportNamedParameters_;

    idx_t limit_{0};

};
}
}
