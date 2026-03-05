# BumbleBee DB Examples

This folder contains example queries demonstrating BumbleBee DB's features in both SQL and Datalog.

## Running Examples

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
