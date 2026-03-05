#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "include/PyBumbleBee.hpp"
#include "include/PyPredicateTable.hpp"

namespace py = pybind11;
using namespace bumblebee::python;

PYBIND11_MODULE(bumblebee, m) {
    m.doc() = "BumbleBee Datalog analytics engine Python bindings";

    py::class_<PyPredicateTable>(m, "PredicateTable",
        "Represents the result of a predicate query.\n\n"
        "Call tuples() to materialize and retrieve the rows."
    )
        .def("tuples", &PyPredicateTable::tuples,
             "Materialize and return the rows as a list of tuples.")
        .def_property_readonly("name", &PyPredicateTable::getName,
             "Predicate name.")
        .def_property_readonly("arity", &PyPredicateTable::getArity,
             "Predicate arity (number of columns).")
        .def("__len__", [](const PyPredicateTable& t) {
             return t.tuples().size();
         });

    py::class_<PyBumbleBee>(m, "BumbleBeeDB",
        "BumbleBee Datalog/SQL analytics engine.\n\n"
        "Instantiate with an optional dict of CLI flags, e.g. "
        "{'-t': '4', '-d': '', '-a': ''}.\n"
        "Call run(program_str) or run_file(path), then retrieve results "
        "via get_all_results(), get_table(name, arity), or get_output_predicates()."
    )
        .def(py::init<const std::map<std::string, std::string>&>(),
             py::arg("args") = std::map<std::string, std::string>{},
             R"doc(
Create a BumbleBeeDB instance.

args: dict mapping CLI flags to values, e.g. {"-t": "4", "-d": "", "-a": ""}
Boolean flags (like --distinct, --print-all) use an empty string as value.
)doc")
        .def("run",                   &PyBumbleBee::run,
             py::arg("program"),      "Run a Datalog/ASP/SQL program given as a string.")

        .def("run_file",              &PyBumbleBee::runFile,
             py::arg("filepath"),     "Run a program from a file.")

        .def("get_output_predicates", &PyBumbleBee::getOutputPredicates,
             "Return list of (name, arity) tuples for non-internal predicates.")

        .def("get_table",             &PyBumbleBee::getTable,
             py::arg("name"), py::arg("arity") = -1,
             "Return a PredicateTable for the given predicate name and arity. "
             "Call .tuples() on the result to materialize the rows.");

}