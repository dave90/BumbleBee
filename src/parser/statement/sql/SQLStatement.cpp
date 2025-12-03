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
                                                           groupby_(std::move(other.groupby_)),
                                                           orderby_(std::move(other.orderby_)),
                                                           exportPath_(std::move(other.exportPath_)),
                                                           exportNamedParameters_(std::move(other.exportNamedParameters_)),
                                                           limit_(other.limit_),
                                                           alias_(std::move(other.alias_))
{}

SQLStatement::SQLStatement(const SQLStatement &other): select_(other.select_),
                                                       from_(other.from_),
                                                       where_(other.where_),
                                                       groupby_(other.groupby_),
                                                       orderby_(other.orderby_),
                                                       alias_(other.alias_),
                                                       exportPath_(other.exportPath_),
                                                       limit_(other.limit_) {
    for (auto & [k,v] : other.exportNamedParameters_)
        exportNamedParameters_[k] = v.clone();

}

SQLStatement & SQLStatement::operator=(const SQLStatement &other) {
    if (this == &other)
        return *this;
    select_ = other.select_;
    from_ = other.from_;
    where_ = other.where_;
    groupby_ = other.groupby_;
    orderby_ = other.orderby_;
    alias_ = other.alias_;
    exportPath_ = other.exportPath_;
    limit_ = other.limit_;
    for (auto & [k,v] : other.exportNamedParameters_)
        exportNamedParameters_[k] = v.clone();

    return *this;
}


SQLStatement & SQLStatement::operator=(SQLStatement &&other) noexcept {
    if (this == &other)
        return *this;
    select_ = std::move(other.select_);
    from_ = std::move(other.from_);
    where_ = std::move(other.where_);
    alias_ = std::move(other.alias_);
    groupby_ = std::move(other.groupby_);
    orderby_ = std::move(other.orderby_);
    exportPath_ = std::move(other.exportPath_);
    exportNamedParameters_ = std::move(other.exportNamedParameters_);
    limit_ = other.limit_;
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

void SQLStatement::setGroupby(Groupby& groupby) {
    groupby_ = std::move(groupby);
}

void SQLStatement::setOrderby(OrderBy &groupby) {
    orderby_ = std::move(groupby);
}

OrderBy & SQLStatement::getOrderby() {
    return orderby_;
}

void SQLStatement::setExportPath(string& exportPath) {
    exportPath_ = std::move(exportPath);
}

void SQLStatement::setExportNamedParameters(std::unordered_map<string, Value>& exportNamedParameters) {
    exportNamedParameters_ = std::move(exportNamedParameters);
}

bool SQLStatement::containsAggregations() {
    for (auto& agg: select_.getAggFunctions())
        if (agg != NONE)
            return true;
    return false;
}

idx_t SQLStatement::getNumAggregations() {
    idx_t result = 0;
    for (auto& agg: select_.getAggFunctions())
        if (agg != NONE)
            result++;
    return result;
}

void SQLStatement::setAlias(const string &alias) {
    alias_ = alias;
}

string SQLStatement::getAlias() const {
    return alias_;
}

void SQLStatement::clearGroupBy() {
    Groupby gb;
    groupby_ = gb;
}

void SQLStatement::clearOrderBy() {
    OrderBy ob;
    orderby_ = ob;
}

void SQLStatement::clearWhere() {
    where_.clear();
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

Groupby & SQLStatement::getGroupby() {
    return groupby_;
}

string & SQLStatement::getExportPath() {
    return exportPath_;
}

std::unordered_map<string, Value> & SQLStatement::getExportNamedParameters() {
    return exportNamedParameters_;
}

idx_t & SQLStatement::getLimit() {
    return limit_;
}

void SQLStatement::setLimit(idx_t limit) {
    limit_ = limit;
}

string SQLStatement::toString() const{
    string result = "";
    result += select_.toString() + "\n";
    result += from_.toString() + "\n";
    result += where_.toString() + "\n";
    result += groupby_.toString()+ "\n";
    result += orderby_.toString(select_)+ "\n";

    if (limit_ > 0)
        result += "\n LIMIT "+std::to_string(limit_) + "\n";

    if (!exportPath_.empty()) {
        result = "COPY ( " + result + " ) " +" TO '"+ exportPath_ + "' (";
        idx_t idx = 0;
        for (auto& [key,value]: exportNamedParameters_) {
            if (idx > 0) result += ", ";
            result+= key+"="+value.toString();
            idx++;
        }
        result += ")";
    }
    return result;
}

void SQLStatement::clear() {
    from_.clear();
    where_.clear();
    select_.clear();
}
}
}
