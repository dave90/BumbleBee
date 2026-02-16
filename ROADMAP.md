## TODOLIST
- optimize aggregation do not merge all partitiones
- TPCH add more queries
- block parenthesis in sql ad datalog
- allow '' in sql and datalog
- parquet write
- Specify predicate tables types
- NULL values in operations
- LEFT and RIGHT JOIN
- sql to datalog allow : WHERE total_revenue = (SELECT max(total_revenue) ... )
- SQL to datalog allow CASE WHEN
- more tests for hits
- more tests for tpch

## Next
- Like expression ext atom
- CICD
- Default minimum integer (small int or int 32 bit not tiny as now)
- Test ldbc_snb_interactive_v1_impls https://github.com/ldbc/ldbc_snb_interactive_v1_impls/blob/main/duckdb/queries/interactive-complex-6.sql
- Python embedding (like duckdb) and interoperability with pandas dataframe https://github.com/pybind/pybind11
- Join with sort

## Next of Next

- Planner with statistics better optimizer and rule ordering
- functions
- Perfect hash join
- better profiling

## 0.1
- CSV
- python lib
- sql (basic)
- directives
- profiling
- hash + aggregate
- recursion
- vectorize ,multithread , SIMD and push
- parquet

## 0.2
- sql (full)
- plugin agg functions
- DB (postegress ,mysql, sqlite)
- multi schema
- compiled operators?
- cascade?
- multi join predicates
- list + array?
- ASP?

