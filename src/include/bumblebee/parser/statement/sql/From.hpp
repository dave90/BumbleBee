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
#include "bumblebee/common/types/Value.hpp"

namespace bumblebee {
namespace sql {

class SQLStatement;

enum FromItemType {
    TABLE,
    SUBQUERY,
    EXTERNAL
};

class FromItem {
public:
    FromItem(const string &table_name);
    explicit FromItem(std::unique_ptr<SQLStatement> &statement);
    FromItem(vector<Value> &input_values, string &ext_table_name,
        std::unordered_map<string, Value> &named_parameters);

    FromItem(const FromItem &other);

    FromItem & operator=(const FromItem &other);

    FromItem(FromItem &&other) noexcept;

    FromItem & operator=(FromItem &&other) noexcept;


    vector<Value> & getInputValues();

    string & getExtTableName();

    std::unordered_map<string, Value> & getNamedParameters();

    string getAlias() const;

    void setAlias(const string&);

    string toString() const;

    FromItemType getType() const;

private:
    std::unique_ptr<SQLStatement> statement_;

    string tableName_;

    vector<Value> inputValues_;
    string extTableName_;
    std::unordered_map<string, Value> namedParameters_;

    string alias_;
    FromItemType type_;
};

using from_items_t = vector<FromItem>;

class From {
public:
    From() = default;

    From(From &&other) noexcept;
    From(const From &other);

    From & operator=(const From &other);
    From & operator=(From &&other) noexcept;

    void addItem(FromItem &item);

    from_items_t & getItems();

    vector<SQLStatement> & getSubQueries();

    string toString() const;

    void addSubqueries(SQLStatement& sql,string & alias);

    void clear();

private:
    from_items_t items_;
    vector<SQLStatement> subQueries_;
};


}
}
