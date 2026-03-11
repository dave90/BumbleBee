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
#include <pybind11/pybind11.h>
#include "bumblebee/BumbleBeeDB.hpp"
#include "bumblebee/catalog/Schema.hpp"
#include "PyPredicateTable.hpp"
#include <map>
#include <string>
#include <vector>

namespace bumblebee::python {
class RegisteredObject {
public:
    explicit RegisteredObject(pybind11::object obj_p) : obj_(std::move(obj_p)) {
    }
    virtual ~RegisteredObject() {
        obj_ = pybind11::none();
    }

    pybind11::object obj_;
};

class PyBumbleBee {
public:
    explicit PyBumbleBee(const std::map<std::string, std::string>& args = {});

    void run(const std::string& program);
    void sql(const std::string& program, const std::string& alias = "", bool overwrite = true);
    void runFile(const std::string& filepath);
    std::string explain(const std::string& program);

    void loadDataframe(pybind11::object df, const std::string& alias);

    pybind11::list getOutputPredicates();
    PyPredicateTable getTable(const std::string& name, int arity = -1);
    pybind11::dict getAllResults();
    void removePredicate(const std::string& name, int arity);

private:
    BumbleBeeDB db_;
    std::map<std::string, std::string> args_;
    std::unordered_map<string, std::unique_ptr<RegisteredObject>> registeredObjects_;

    Schema& getSchema();
    void applyArgs(const std::map<std::string, std::string>& args);
};

} // namespace bumblebee::python