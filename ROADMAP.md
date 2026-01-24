## TODOLIST
- block parenthesis in sql ad datalog
- check vector getType and logical types
- parquet
- Query a partition
- Specify predicate tables types
- Refactor aggregates and avoid distinct
- Decimal date and timestamp in vector operation (logical type switch instead of physical)
- NUL values in operations

## Next
- CICD
- Default minimum integer (small int or int 32 bit not tiny as now)
- Perfect hash join
- Test ldbc_snb_interactive_v1_impls https://github.com/ldbc/ldbc_snb_interactive_v1_impls/blob/main/duckdb/queries/interactive-complex-6.sql

## Next of Next

- Planner with statistics better optimizer and rule ordering
- functions
- DATE / TIMESTAMP
- Specific table types
- Python embedding (like duckdb) and interoperability with pandas dataframe https://github.com/pybind/pybind11

## 0.1
- CSV
- server mode ???
- store data multi session
- python lib
- sql (basic)
- directives
- profiling
- hash + aggregate
- recursion
- vectorize ,multithread , SIMD and push

## 0.2
- parquet
- sql (full)
- plugin agg functions
- date/timestamp
- DB (postegress ,mysql, sqlite)
- multi schema
- compiled operators
- cascade?
- multi join predicates?
- list + array?
- ASP?

## v1
- Master and slave architecture?
- Serialize/ Deserialize data to disk