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
FromItem::FromItem(const string &table_name): tableName_(table_name), type_(TABLE) {
}

FromItem::FromItem(std::unique_ptr<SQLStatement> &statement): statement_(std::move(statement)), type_(SUBQUERY) {
}

FromItem::FromItem(vector<Value> &input_values, string &ext_table_name,
    std::unordered_map<string, Value> &named_parameters): inputValues_(std::move(input_values)),
                                                                extTableName_(ext_table_name),
                                                                namedParameters_(std::move(named_parameters)), type_(EXTERNAL) {
}

FromItem::FromItem(const FromItem &other): tableName_(other.tableName_),
                                           extTableName_(other.extTableName_),
                                           alias_(other.alias_),
                                           type_(other.type_) {
    if (other.statement_)
        statement_ = std::make_unique<SQLStatement>(*other.statement_);
    for (auto& v:other.inputValues_)
        inputValues_.push_back(v.clone());
    for (auto & [k,v] : other.namedParameters_)
        namedParameters_[k] = v.clone();
}

FromItem & FromItem::operator=(const FromItem &other) {
    if (this == &other)
        return *this;
    if (other.statement_)
        statement_ = std::make_unique<SQLStatement>(*other.statement_);
    tableName_ = other.tableName_;
    extTableName_ = other.extTableName_;
    alias_ = other.alias_;
    type_ = other.type_;
    for (auto& v:other.inputValues_)
        inputValues_.push_back(v.clone());
    for (auto & [k,v] : other.namedParameters_)
        namedParameters_[k] = v.clone();
    return *this;
}

FromItem::FromItem(FromItem &&other) noexcept: statement_(std::move(other.statement_)),
                                               tableName_(std::move(other.tableName_)),
                                               alias_(std::move(other.alias_)), type_(other.type_),
                                               extTableName_(std::move(other.extTableName_)),
                                               inputValues_(std::move(other.inputValues_)),
                                               namedParameters_(std::move(other.namedParameters_)){
}

FromItem &FromItem::operator=(FromItem &&other) noexcept {
    if (this == &other)
        return *this;
    statement_ = std::move(other.statement_);
    tableName_ = std::move(other.tableName_);
    alias_ = std::move(other.alias_);
    type_ = other.type_;
    extTableName_ = std::move(other.extTableName_);
    inputValues_ = std::move(other.inputValues_);
    namedParameters_ = std::move(other.namedParameters_);
    return *this;
}

vector<Value> & FromItem::getInputValues() {
    return inputValues_;
}

string & FromItem::getExtTableName() {
    return extTableName_;
}

std::unordered_map<string, Value> & FromItem::getNamedParameters() {
    return namedParameters_;
}


string FromItem::toString() const{
    string result = "";
    switch (type_) {
        case TABLE: {
            result = tableName_;
            break;
        }
        case SUBQUERY: {
            result += "( " + statement_->toString() + ")";
            break;
        }
        case EXTERNAL: {
            result += extTableName_ + "(";
            idx_t i = 0;
            for (auto& iv: inputValues_) {
                if (i > 0)result += ", ";
                result += iv.toString();
                ++i;
            }
            result += " ; ";
            i = 0;
            for (auto& [key, value]: namedParameters_) {
                if (i > 0)result += ", ";
                result += key + " = " + value.toString();
                ++i;
            }

            result += ")";
        }
    }
    if (!alias_.empty())
        result += " AS " + alias_;

    return result;
}

FromItemType FromItem::getType() const {
    return type_;
}


void FromItem::setAlias(const string& alias) {
    alias_ = alias;
}

string FromItem::getAlias() const {
    return alias_;
}


From::From(From &&other) noexcept: items_(std::move(other.items_)),
                                   subQueries_(std::move(other.subQueries_)) {
}

From::From(const From &other): items_(other.items_),
                               subQueries_(other.subQueries_) {
}

From & From::operator=(const From &other) {
    if (this == &other)
        return *this;
    items_ = other.items_;
    subQueries_ = other.subQueries_;
    return *this;
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

from_items_t & From::getItems() {
    return items_;
}

vector<SQLStatement> & From::getSubQueries() {
    return subQueries_;
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
