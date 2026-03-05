#pragma once
#include <pybind11/pybind11.h>
#include <optional>
#include <string>

namespace bumblebee {
class PredicateTables;
} // namespace bumblebee

namespace bumblebee::python {

class PyPredicateTable {
public:
    PyPredicateTable(std::string name, int arity, bumblebee::PredicateTables* pt);

    pybind11::list tuples() const;
    const std::string& getName() const;
    int getArity() const;

private:
    std::string name_;
    int arity_;
    bumblebee::PredicateTables* pt_;
    mutable std::optional<pybind11::list> cached_;
};

} // namespace bumblebee::python