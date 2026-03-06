#pragma once
#include <pybind11/pybind11.h>
#include "bumblebee/BumbleBeeDB.hpp"
#include "bumblebee/catalog/Schema.hpp"
#include "PyPredicateTable.hpp"
#include <map>
#include <string>
#include <vector>

namespace bumblebee::python {

class PyBumbleBee {
public:
    explicit PyBumbleBee(const std::map<std::string, std::string>& args = {});

    void run(const std::string& program);
    void sql(const std::string& program, const std::string& alias = "");
    void runFile(const std::string& filepath);

    pybind11::list getOutputPredicates();
    PyPredicateTable getTable(const std::string& name, int arity = -1);
    pybind11::dict getAllResults();

private:
    BumbleBeeDB db_;
    std::map<std::string, std::string> args_;

    bumblebee::Schema& getSchema();
    void applyArgs(const std::map<std::string, std::string>& args);
};

} // namespace bumblebee::python