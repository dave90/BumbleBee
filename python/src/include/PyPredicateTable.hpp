#pragma once
#include <pybind11/pybind11.h>
#include <optional>
#include <string>

#include "bumblebee/common/TypeDefs.hpp"
#include "bumblebee/common/Vector.hpp"

namespace bumblebee {
class PredicateTables;
} // namespace bumblebee

namespace bumblebee::python {

class PyPredicateTable {
public:
    PyPredicateTable(std::string name, int arity, bumblebee::PredicateTables* pt);

    pybind11::list tuples() const;
    pybind11::object toDf(const vector<string>& names = {}) const;
    const std::string& getName() const;
    int getArity() const;

private:
    pybind11::dict fetchNumpyInternal(const vector<string>& names) const;


    std::string name_;
    int arity_;
    bumblebee::PredicateTables* pt_;
};

} // namespace bumblebee::python