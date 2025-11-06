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
                                                           exportPath_(std::move(other.exportPath_)),
                                                           exportNamedParameters_(std::move(other.exportNamedParameters_)),
                                                           alias_(std::move(other.alias_)){
}


SQLStatement & SQLStatement::operator=(SQLStatement &&other) noexcept {
    if (this == &other)
        return *this;
    select_ = std::move(other.select_);
    from_ = std::move(other.from_);
    where_ = std::move(other.where_);
    alias_ = std::move(other.alias_);
    groupby_ = std::move(other.groupby_);
    exportPath_ = std::move(other.exportPath_);
    exportNamedParameters_ = std::move(other.exportNamedParameters_);
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

void SQLStatement::setGroupby(Groupby groupby) {
    groupby_ = std::move(groupby);
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

Groupby & SQLStatement::getGroupby() {
    return groupby_;
}

string & SQLStatement::getExportPath() {
    return exportPath_;
}

std::unordered_map<string, Value> & SQLStatement::getExportNamedParameters() {
    return exportNamedParameters_;
}

string SQLStatement::toString() const{
    string result = "";
    result += select_.toString() + "\n";
    result += from_.toString() + "\n";
    result += where_.toString();
    result += groupby_.toString();
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
