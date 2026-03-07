<p align="center">
  <img src="./logo/bumblebee4.png"  width=50% />
</p>

# BumbleBee DB (VISION)
BumbleBee DB aims to become a lightweight, high-performance In-memory Datalog-based analytics engine that empowers 
data engineers and researchers to analyze massive datasets on a single machine. 
Inspired by the architecture of modern OLAP systems, BumbleBee combines efficient execution models and flexible data sourcing to deliver fast and expressive analytics workflows. 
Whether your data lives in CSV files or Parquet files, BumbleBee lets you define powerful transformations
and queries using either Datalog or SQL, offering full transparency and extensibility for advanced users.

## Features

- Datalog as input language
- High-performance engine with code generation, push-based execution, columnar storage, and multithreading
- Available as a command-line interface (CLI) and a Python client library
- Supports data sources including CSV and Parquet files
- **Connect to any data source via the Python client**: through DataFrame compatibility, you can pass any pandas (or compatible) DataFrame directly as input to BumbleBee queries — meaning you can load data from virtually any source (databases, APIs, Excel files, in-memory structures, etc.) before handing it off for analysis, similar to DuckDB's relation API

## Quick Start

### Prerequisites

- CMake 3.20+
- C++20-compatible compiler (GCC 13+ or Clang 18+ with libc++)
- Ninja or Make

### Build

```bash
# Configure (release)
cmake -S . -B cmake-build-release -DCMAKE_BUILD_TYPE=Release

# Compile
cmake --build cmake-build-release --target BumbleBee -j 8

# Verify
./cmake-build-release/BumbleBee --help
```

### Run your first query

The `examples/` folder contains ready-to-run queries and sample data. For instance, to load and print all rows from a CSV file:

**Datalog** (`examples/dl/01_import/basic_csv_import.dl`):

```datalog
employee(ID, NAME, DEPARTMENT_ID, SALARY, HIRE_DATE) :-
    &read_csv("./data/employees.csv"; auto_detect=1, header=1; ID, NAME, DEPARTMENT_ID, SALARY, HIRE_DATE).

employee(ID, NAME, DEPARTMENT_ID, SALARY, HIRE_DATE)?
```

```bash
cd examples
../cmake-build-release/BumbleBee -i dl/01_import/basic_csv_import.dl
```

**SQL** (`examples/sql/01_import/basic_csv_import.sql`):

```sql
SELECT * FROM "./data/employees.csv"
```

```bash
cd examples
../cmake-build-release/BumbleBee -i sql/01_import/basic_csv_import.sql
```


## Examples

Check out the [`examples/`](./examples) folder for practical examples on how to use BumbleBee DB, including query patterns, data loading, and integration scenarios.

## Optimizer

BumbleBee DB includes a rule-based query optimizer that applies logical rewrites such as filter push-down and column pruning. The current optimizer does **not** reorder joins — the execution order follows the join sequence as written in the query. A cost-based join reordering optimizer is planned as a future enhancement.

## Work in Progress

- **Python client**: adoption of a Python client library to interact with BumbleBee DB programmatically, making it easy to embed analytics queries directly in Python workflows
- **Code generation**:  code generation to cover complex AND and OR clause combinations in filter expressions
- **Predicate table types**: currently all predicate table types are automatically deduced at runtime; future versions will allow users to explicitly declare column types for predicates
- **Left and right joins**: support for LEFT JOIN and RIGHT JOIN in addition to the existing inner join
- **Sort-merge join**: implementation of a sort-merge join operator as an alternative to hash joins, beneficial for pre-sorted or range-based workloads
- **NULL handling**: support for NULL values across all operators, including NULL-safe comparisons and aggregations
- **Cost-based join optimizer**: upgrading the current rule-based optimizer to a cost-based one capable of reordering joins based on cardinality estimates, significantly improving performance on complex multi-join queries

