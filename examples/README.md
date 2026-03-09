# BumbleBee DB Examples

This folder contains example queries demonstrating BumbleBee DB's features in both SQL and Datalog.


## Python Examples (`python/`)

Use BumbleBee as a Python library: load DataFrames, run SQL/Datalog, and get results back as DataFrames.

**Prerequisites:**

Install the `bumblebeedb` package from PyPI:
```bash
pip install bumblebeedb
```

Or install from source (from the project root):
```bash
pip install .
```

**Running:** All scripts use relative paths to `./data/`, so run from the `examples/` directory:
```bash
cd examples
python python/01_getting_started.py
```

| Script | Description |
|--------|-------------|
| `01_getting_started.py` | Query CSV files with SQL, retrieve results as pandas DataFrames |
| `02_dataframe_analytics.py` | Load pandas DataFrames and run Datalog/SQL aggregations |
| `03_multi_source_join.py` | Join CSV file data with in-memory DataFrames |
| `04_recursive_analysis.py` | Graph reachability with recursive Datalog rules |
| `05_iterative_queries.py` | Interactive session: multiple queries, inspect state, redefine predicates |



## Running SQL/DL Examples

**Prerequisites:** Build BumbleBee first (from the project root):
```bash
cmake -S . -B cmake-build-release -DCMAKE_BUILD_TYPE=Release
cmake --build cmake-build-release --target BumbleBee -j 8
```

All examples use **relative paths** to `./data/`, so run BumbleBee from the `examples/` directory:

```bash
cd examples

# SQL examples
../cmake-build-release/BumbleBee -i sql/01_import/basic_csv_import.sql

# Datalog examples (-a prints all predicates)
../cmake-build-release/BumbleBee -i dl/01_import/basic_csv_import.dl -a

# Export examples write files to ./output/
../cmake-build-release/BumbleBee -i sql/02_export/export_csv_basic.sql
../cmake-build-release/BumbleBee -i dl/02_export/export_csv.dl
```

### Useful Flags

| Flag | Description |
|------|-------------|
| `-a` | Print all predicates (recommended for Datalog) |
| `-t N` | Use N threads (default: all cores) |
| `-r` | Print profiling data |
| `--print-program` | Print the generated Datalog program and exit |


## SQL Examples (`sql/`)

SQL files start with `%@sql` to indicate SQL mode.

| Folder | Description |
|--------|-------------|
| `01_import/` | Import data from CSV and Parquet files |
| `02_export/` | Export query results to CSV files using `COPY ... TO` |
| `03_aggregations/` | COUNT, SUM, AVG, MIN, MAX and GROUP BY |
| `04_order_by/` | ORDER BY with LIMIT (ORDER BY requires LIMIT) |
| `05_in_clause/` | IN and NOT IN with constants and subqueries |
| `06_subqueries/` | Scalar subqueries in WHERE clauses |
| `07_like/` | LIKE pattern matching with `%` wildcards |

## Datalog Examples (`dl/`)

| Folder | Description |
|--------|-------------|
| `01_import/` | Read CSV and Parquet files with `&read_csv` / `&read_parquet` |
| `02_export/` | Write results to CSV/Parquet with `&write_csv` / `&write_parquet` |
| `03_aggregations/` | Aggregates (`#sum`, `#count`, `#min`, `#max`), multi-aggregates, explicit groups |
| `04_recursion/` | Recursive rules: transitive closure, graph reachability |
| `05_order_by/` | `#order_by` and `#limit` directives |
| `06_like/` | Filter strings with `&like` pattern matching |

## Sample Data (`data/`)

| File | Description |
|------|-------------|
| `employees.csv` | 15 employees with departments, salaries, and hire dates |
| `departments.csv` | 5 departments with locations and budgets |
| `sales.csv` | 20 sales transactions with products and customers |
| `products.csv` | 10 products with categories and prices |
| `nations.parquet` | 25 nations from the TPC-H benchmark (Parquet format) |
