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

#include "include/PyBumbleBee.hpp"
#include "bumblebee/catalog/PredicateTables.hpp"
#include "bumblebee/catalog/Schema.hpp"
#include "bumblebee/common/StringUtils.hpp"
#include <vector>

#include "include/PandasScan.hpp"

namespace py = pybind11;

namespace bumblebee::python {

// ---------------------------------------------------------------------------
// PyBumbleBee
// ---------------------------------------------------------------------------

PyBumbleBee::PyBumbleBee(const std::map<std::string, std::string>& args)
    : db_(std::make_unique<Schema>("default")), args_(args) {
    applyArgs(args_);

    // register python functions
    PandasScanFunc::registerFunction(db_.context_.functionRegister_);
}

void PyBumbleBee::applyArgs(const std::map<std::string, std::string>& args) {
    std::vector<std::string> argv_strings;
    argv_strings.emplace_back("BumbleBeeDB");
    // append -p to print the error
    argv_strings.push_back("-p");
    for (auto& [flag, value] : args) {
        argv_strings.push_back(flag);
        if (!value.empty()) {
            argv_strings.push_back(value);
        }
    }

    std::vector<char*> argv_ptrs;
    argv_ptrs.reserve(argv_strings.size());
    for (auto& s : argv_strings) {
        argv_ptrs.push_back(const_cast<char*>(s.c_str()));
    }

    db_.parseArgs(static_cast<int>(argv_ptrs.size()), argv_ptrs.data());
}

void PyBumbleBee::run(const std::string& program) {
    db_.runFromInputString(program);
    getSchema().deleteInternalPredicates();
}

void PyBumbleBee::sql(const std::string& program, const std::string& alias) {
    string query = alias.empty() ? program : "(" + program + ") AS " + alias;
    string sqlProgram = "%@sql\n" + query;
    db_.runFromInputString(sqlProgram);
    getSchema().deleteInternalPredicates();
}

void PyBumbleBee::runFile(const std::string& filepath) {
        std::map<std::string, std::string> combined = args_;
        combined["-i"] = filepath;
        applyArgs(combined);
        db_.run();
        getSchema().deleteInternalPredicates();
}

std::string PyBumbleBee::explain(const std::string& program) {
    return db_.explainFromInputString(program);
}

void PyBumbleBee::loadDataframe(pybind11::object df, const std::string &alias) {

    if (alias.empty() || alias[0] < 'a' || alias[0] > 'z') {
        throw std::runtime_error("Alias must start with lower case character [a-z]");
    }
    if (registeredObjects_.contains(alias)) {
        throw std::runtime_error("Alias dataframe " + alias + " already exist");
    }
    vector<PandasColumnBindData> bind;
    vector<LogicalType> types;
    vector<string> names;
    VectorConversion::bindPandas(df, bind, types, names);

    registeredObjects_[alias] = std::make_unique<RegisteredObject>(df);
    auto pointer = reinterpret_cast<uint64_t>(df.ptr());

    // Normalize column names to valid Datalog variables (uppercase, no spaces)
    // Pandas scan binds by position, so renaming duplicates is safe
    vector<string> varNames;
    std::unordered_map<string, int> seen;
    for (auto& name : names) {
        string normalized = StringUtils::normalizeColumnName(name);
        int count = seen[normalized]++;
        if (count > 0) {
            normalized += "_" + std::to_string(count);
        }
        varNames.push_back(std::move(normalized));
    }

    // Build rule: alias(COL1, COL2, ...) :- &pandas_scan(pointer;;COL1, COL2, ...).
    string vars;
    for (idx_t i = 0; i < varNames.size(); i++) {
        if (i > 0) vars += ", ";
        vars += varNames[i];
    }

    string rule = alias + "(" + vars + ") :- &pandas_scan(" +
                  std::to_string(pointer) + ";;" + vars + "). " +
                  alias + "(" + vars + ")?";
    {
        // This is what enables multithreading. Without releasing the GIL before run(), worker threads that
        // need the GIL (for bind) would deadlock.
        pybind11::gil_scoped_release release;
        run(rule);
    }
}

Schema& PyBumbleBee::getSchema() {
    return db_.getSchema();
}

py::list PyBumbleBee::getOutputPredicates() {
    py::list result;
    for (auto* pred : getSchema().getPredicates()) {
        if (pred->getName()[0] != '#') {
            result.append(py::make_tuple(pred->getName(), pred->getArity()));
        }
    }
    return result;
}

PyPredicateTable PyBumbleBee::getTable(const std::string& name, int arity) {
    for (auto* pred : getSchema().getPredicates()) {
        if (pred->getName()[0] == '#') continue;
        if (pred->getName() == name && (arity == -1 || pred->getArity() == arity)) {
            return PyPredicateTable(std::string(pred->getName()), pred->getArity(),
                                   getSchema().getPredicateTable(pred).get());
        }
    }
    throw std::runtime_error("Predicate not found: " + name +
                             (arity == -1 ? "" : "/" + std::to_string(arity)));
}

py::dict PyBumbleBee::getAllResults() {
    py::dict results;
    for (auto* pred : getSchema().getPredicates()) {
        if (pred->getName()[0] == '#') continue;
        std::string key = std::string(pred->getName()) + "/" + std::to_string(pred->getArity());
        PyPredicateTable pt(std::string(pred->getName()), pred->getArity(),
                            getSchema().getPredicateTable(pred).get());
        results[py::str(key)] = pt.tuples();
    }
    return results;
}

void PyBumbleBee::removePredicate(const std::string& name, int arity) {
    if (arity < 0) {
        throw std::invalid_argument("arity must be >= 0");
    }
    // Check the predicate exists before deleting
    bool found = false;
    for (auto* pred : getSchema().getPredicates()) {
        if (pred->getName() == name && pred->getArity() == static_cast<unsigned>(arity)) {
            found = true;
            break;
        }
    }
    if (!found) {
        throw std::runtime_error("Predicate not found: " + name + "/" + std::to_string(arity));
    }
    getSchema().deletePredicate(name.c_str(), static_cast<unsigned>(arity));
    registeredObjects_.erase(name);
}

} // namespace bumblebee::python