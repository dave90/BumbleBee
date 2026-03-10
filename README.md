<p align="center">
  <img src="./logo/bumblebee4.png"  width=50% />
</p>

# BumbleBee DB

**A high-performance Datalog-based analytics engine — with SQL support — from Python.**

BumbleBee DB is a lightweight, in-memory analytics engine powered by **Datalog** — a declarative logic language that makes recursive queries, graph analysis, and complex joins natural to express. It also supports **SQL** as an alternative query language, so you can mix and match both in the same session. Query CSV files, Parquet files, and pandas DataFrames through a simple Python API. Built in C++ with push-based execution, columnar storage, and multithreading, it delivers serious performance on a single machine.

## Quick Start

### Install

```bash
pip install bumblebeedb
```

> **Platform support:** Pre-built wheels are available for **Linux x86_64** and **macOS ARM (Apple Silicon)**. If your platform is not supported, you can still use BumbleBee DB in [Google Colab](https://colab.research.google.com/drive/1EWIS0OxGdTE99p14ElXXHESbLb2bgrb7?usp=sharing).

### Your first query

```python
import bumblebeedb as bb

db = bb.db()

# Query a CSV file with SQL — alias names the output predicate
db.sql("""
    SELECT DEPARTMENT_ID, COUNT(*) AS CNT, SUM(SALARY) AS TOTAL
    FROM "examples/data/employees.csv"
    GROUP BY DEPARTMENT_ID
""", alias="dept_stats")

# Get results as a pandas DataFrame
# 3 is the arity (number of columns) of the dept_stats predicate
df = db.get_table("dept_stats", 3).to_df(
    col_names=["dept_id", "count", "total_salary"]
)
print(df)
```

### Recursive Datalog — something SQL can't easily do

BumbleBee supports recursive Datalog rules, enabling graph analysis, transitive closure, and hierarchical queries with a clean, declarative syntax:

```python
import bumblebeedb as bb
import pandas as pd

hierarchy = pd.DataFrame({
    "manager": ["alice", "alice", "bob", "bob", "carol", "dave"],
    "report":  ["bob",   "carol", "dave", "eve", "frank", "grace"],
})

db = bb.db()

# Load a pandas DataFrame as a predicate that can be queried
db.load_df(hierarchy, "manages")

# Recursive Datalog: compute all direct and indirect reports
db.run("""
    reports_to(M, R) :- manages(M, R).
    reports_to(M, R) :- manages(M, X), reports_to(X, R).
    reports_to(X, Y)?
""")

df = db.get_table("reports_to", 2).to_df(col_names=["manager", "report"])
print("reports_to:")
print(df.sort_values(["manager", "report"]).to_string(index=False))
print()

# You can also run SQL on top of Datalog results — count reports per manager
db.sql("""
    SELECT V1, COUNT(*) AS CNT
    FROM reports_to
    GROUP BY V1
""", alias="report_count")

df2 = db.get_table("report_count", 2).to_df(col_names=["manager", "num_reports"])
print("num_reports:")
print(df2.sort_values("num_reports", ascending=False).to_string(index=False))
print()
```

## Features

- **Python client library** — `pip install bumblebeedb`, query and get DataFrames back
- **Dual query languages** — SQL and Datalog, including recursive Datalog
- **High-performance engine** — push-based execution, columnar storage, multithreading
- **Read and write CSV and Parquet** — import data from and export results to CSV and Parquet files
- **DataFrame interop** — load any pandas DataFrame as input via `db.load_df()`, meaning you can connect to virtually any data source (databases, APIs, Excel, in-memory structures) before handing it off for analysis
- **Command-line interface** — run queries directly from the terminal

## Python API

| Method | Description |
|--------|-------------|
| `db = bb.db(args={})` | Create a new engine instance (only one per session). Optional `args` dict for CLI flags, e.g. `{"-t": "4", "-d": ""}` |
| `db.run(program)` | Run a Datalog program |
| `db.sql(query, alias="")` | Run a SQL query. If `alias` is provided, wraps the query as `(query) AS alias` |
| `db.run_file(filepath)` | Run a program from a file |
| `db.load_df(df, alias)` | Load a pandas DataFrame as a predicate. Alias must start with a lowercase letter |
| `db.explain(program)` | Return the generated Datalog rules as a string without executing |
| `db.get_output_predicates()` | List all output predicates as `(name, arity)` tuples |
| `db.get_table(name, arity=-1)` | Get a result table. Arity is optional |
| `table.tuples()` | Get results as a list of tuples |
| `table.to_df(col_names=[])` | Get results as a pandas DataFrame. Column names are optional |
| `db.remove_table(name, arity)` | Remove a predicate from the engine |

## Examples

Check out the [`examples/`](./examples) folder for a collection of ready-to-use examples in Python, SQL, and Datalog, covering data imports, aggregations, joins, recursion, exports, and more.

## CLI Quick Start

BumbleBee is also available as a standalone command-line tool.

### Prerequisites

- CMake 3.20+
- C++20-compatible compiler (GCC 13+ or Clang 18+ with libc++)

### Build

```bash
cmake -S . -B cmake-build-release -DCMAKE_BUILD_TYPE=Release
cmake --build cmake-build-release --target BumbleBee -j 8
./cmake-build-release/BumbleBee --help
```

### Run a query

```bash
cd examples

# SQL
../cmake-build-release/BumbleBee -i sql/01_import/basic_csv_import.sql

# Datalog
../cmake-build-release/BumbleBee -i dl/01_import/basic_csv_import.dl -a
```

| Flag | Description |
|------|-------------|
| `-a` | Print all predicates |
| `-t N` | Use N threads (default: all cores) |
| `-r` | Print profiling data |
| `--print-program` | Print the generated Datalog program and exit |

## Optimizer

BumbleBee DB includes a rule-based query optimizer that applies logical rewrites such as filter push-down and column pruning. The current optimizer does **not** reorder joins — the execution order follows the join sequence as written in the query. A cost-based join reordering optimizer is planned as a future enhancement.

## Roadmap

- **Code generation**: cover complex AND and OR clause combinations in filter expressions
- **Predicate table types**: allow users to explicitly declare column types for predicates (currently auto-deduced at runtime)
- **Left and right joins**: support for LEFT JOIN and RIGHT JOIN
- **Sort-merge join**: alternative to hash joins for pre-sorted or range-based workloads
- **NULL handling**: NULL values across all operators, including NULL-safe comparisons and aggregations
- **Cost-based join optimizer**: reorder joins based on cardinality estimates
