#include "include/PyBumbleBee.hpp"
#include "bumblebee/catalog/PredicateTables.hpp"
#include <vector>

namespace py = pybind11;

namespace bumblebee::python {

// ---------------------------------------------------------------------------
// PyBumbleBee
// ---------------------------------------------------------------------------

PyBumbleBee::PyBumbleBee(const std::map<std::string, std::string>& args)
    : args_(args) {
    applyArgs(args_);
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

void PyBumbleBee::runFile(const std::string& filepath) {
        std::map<std::string, std::string> combined = args_;
        combined["-i"] = filepath;
        applyArgs(combined);
        db_.run();
        getSchema().deleteInternalPredicates();
}

bumblebee::Schema& PyBumbleBee::getSchema() {
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

} // namespace bumblebee::python