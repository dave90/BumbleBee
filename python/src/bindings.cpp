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
          .def("to_df", &PyPredicateTable::toDf,
          py::arg("col_names") = std::vector<std::string>{},
             "Materialize and return the rows as a pandas dataframe.")
        .def_property_readonly("name", &PyPredicateTable::getName,
             "Predicate name.")
        .def_property_readonly("arity", &PyPredicateTable::getArity,
             "Predicate arity (number of columns).")
        .def("__len__", [](const PyPredicateTable& t) {
             return t.tuples().size();
         });

    py::class_<PyBumbleBee>(m, "db",
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
             py::arg("program"),      "Run a Datalog/SQL program given as a string.")
        .def("sql",                   &PyBumbleBee::sql,
             py::arg("program"),
             py::arg("alias") = "",
             "Run a SQL program given as a string (no %@sql prefix needed). "
             "If alias is provided, wraps the query as (query) AS alias.")

        .def("run_file",              &PyBumbleBee::runFile,
             py::arg("filepath"),     "Run a program from a file.")

        .def("get_output_predicates", &PyBumbleBee::getOutputPredicates,
             "Return list of (name, arity) tuples for non-internal predicates.")

        .def("get_table",             &PyBumbleBee::getTable,
             py::arg("name"), py::arg("arity") = -1,
             "Return a PredicateTable for the given predicate name and arity. "
             "Call .tuples() on the result to materialize the rows.")

        .def("load_df",               &PyBumbleBee::loadDataframe,
             py::arg("df"), py::arg("alias"),
             "Load a pandas DataFrame as a predicate with the given alias name. "
             "The alias must start with a lowercase letter [a-z].")

        .def("remove_table",          &PyBumbleBee::removePredicate,
             py::arg("name"), py::arg("arity"),
             "Remove a predicate table by name and arity. "
             "Raises RuntimeError if the predicate does not exist.");

}