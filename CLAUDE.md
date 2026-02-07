# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

BumbleBee DB is a high-performance Datalog-based analytics engine written in C++20. It supports both Datalog and SQL as input languages, with push-based execution, columnar storage, and multithreading. Data sources include CSV, Parquet, and ASP files.

## Build Commands

```bash
# Release build
cmake --build cmake-build-release --target BumbleBee -j 8 --verbose
./cmake-build-release/BumbleBee --help

# Debug build (includes AddressSanitizer)
cmake --build cmake-build-debug --target BumbleBee -j 8 --verbose
./cmake-build-debug/BumbleBee --help
```

## Running the CLI

```bash
./cmake-build-release/BumbleBee -i <input_file> [options]
# Options:
#   -t, --threads <N>     Number of threads (default: hardware concurrency)
#   -a, --print-all       Print all predicates
#   -r, --print-profiling Print profiling data
#   -d, --distinct        Set all predicates as distinct
#   --print-program       Print the Datalog program and exit
```

## Testing

```bash
# Unit tests (Google Test)
cd cmake-build-debug && ctest -j

# Run a single unit test
cd cmake-build-debug && ctest -R <test_name>

# E2E tests (requires pytest, duckdb, clingo)
cd test/e2e && pytest test.py

# Generate ASP expected output for E2E tests
cd test/e2e && python gen_asp_test.py
```

## Parser Regeneration

When modifying `aspcore2.l` or `aspcore2.y`:
```bash
flex -o src/parser/aspcore2_lexer.hpp src/parser/aspcore2.l
bison -y -d -o src/parser/aspcore2_parser.c src/parser/aspcore2.y
bison -y -o src/parser/aspcore2_parser.hpp src/parser/aspcore2.y
```

Or
```bash
./build_parser.sh
```

### Key Data Structures

- **DataChunk**: Container for columnar data (multiple Vectors)
- **Vector**: Typed column with selection vector; can be Regular, Constant, Sequence, or Dictionary

### Source Layout

- `src/catalog/` - Database/predicate metadata, tables with statistics, caching, indexes
- `src/common/` - Utilities, memory management, data structures, CSV/Parquet readers
- `src/execution/` - Physical operators (scan, filter, join, aggregate, output)
- `src/function/` - Built-in functions and predicates (read_csv, read_parquet, aggregates)
- `src/parser/` - Flex/Bison grammar and AST construction
- `src/planner/` - Logical DAG, rewriters, physical plan generation
- `src/parallel/` - Task scheduling and thread pool execution
- `src/storage/` - Storage management (block manager, buffer manager, statistics, etc.)

### Recursion Handling

Recursive rules are processed iteratively until a fixed-point is reached. Each iteration re-optimizes physical rules. Head predicates store results in hash tables for deduplication. Processing stops when no new data is produced.

## Code Conventions

### SQL to Datalog Conversion (`SqlToDatalog.cpp`)
- SQL queries are translated to Datalog rules via `SqlToDatalog::sqlToDatalog()`
- Variable names follow pattern `COLUMN.#p_N` where N is the table alias index

### External Atoms
- Format: `&func_name(input_values;named_param1=val1,named_param2=val2;term1,term2,...)`
- Sections separated by `;`: inputs, named parameters (comma-separated), terms
- File readers (`&read_csv`, `&read_parquet`) support `columns_mapping` parameter for variable-to-column translation

### Anonymous Variables
- The underscore `_` represents anonymous/don't-care variables
- The optimizer may introduce `_` when rewriting rules to remove unused variables

### Aggregation
- `#ID` variable used in aggregates for duplicate handling (avoids distinct calculation)
- `&gen_id` atom generates unique IDs when needed

#### Implicit Groups (default)
- Format: `VAR = #agg{agg_terms:agg_body_atoms}`
- **Groups**: determined by variables **shared** between rule body and aggregate body

Example:
```datalog
result(X,Y,S) :- a(X,Y), S = #sum{Z:b(X,Y,Z)}.
```
- Rule body: `a(X,Y)` → variables X, Y
- Aggregate body: `b(X,Y,Z)` → variables X, Y, Z
- Shared variables: X, Y → **these are the groups**
- Z is local to aggregate, S is the sum of Z values

#### Explicit Groups
- Format: `VAR = #agg{agg_terms;group_terms:agg_body_atoms}`
- **Groups**: explicitly specified after semicolon, not inferred from shared variables
- Useful when there are no prior atoms in the rule body (aggregate is the only source)

Example:
```datalog
c(Y,K) :- K = #sum{X;Y:a(Y,X)}.
```
- No prior body atoms - groups must be explicit
- `Y` is the explicit group variable
- `X` is summed for each distinct `Y`

Cross product example:
```datalog
c(V,Y,K) :- b(V), K = #sum{X;Y:a(Y,X)}.
```
- `b(V)` provides input rows
- Aggregate scans all groups from `a(Y,X)`
- Output is cross product: each `V` combined with each `(Y,K)` pair

## AI Development Workflow

### Before Making Changes
- **Always describe the plan first**: Before modifying any code, provide a clear description of what changes will be made and why. Wait for user approval before proceeding.
Before making any changes to the code, first provide a concise, high-level plan outlining:

- What will be changed
- Why those changes are needed
- How the work will be broken down into phases or steps. 
- Phases of work, where each phase can include more detailed notes about the specific implementations to be carried out

If there are any uncertainties, assumptions, or missing details, list them at the end as open questions.

Do not modify any code until the user has reviewed and approved the plan.

### After Making Changes
- **Compile in debug mode** to verify the code compiles correctly:
- Fix any compilation errors before proceeding.

### Testing Changes
- Extend (if exist) the unit testing class to cover the new implementation in folder `test/unit/`
- **Create e2e test files** in the appropriate folder:
  - ASP tests: `test/e2e/files/asp/input/<category>/<name>`
  - SQL tests: `test/e2e/files/sql/input/<name>.sql`
- **Create expected output files** in the corresponding expected folder:
  - ASP: `test/e2e/files/asp/expected/<category>/<name>`
  - SQL: `test/e2e/files/sql/expected/<name>.csv`
- **Run and verify** the test produces correct output:
  ```bash
  ./cmake-build-debug/BumbleBee -i test/e2e/files/asp/input/<category>/<name> -a
  ```
- **Run all the e2e test**:
  -  `cd test/e2e && pytest test.py`
  -  `cd test/e2e && pytest test.py -k "test_sql"` to run only the sql tests
  -  `cd test/e2e && pytest test.py -k "test_asp"` to run only the asp tests
- **Use `-p -r` flags** for detailed debugging:
  - `-p`: Print debug logs
  - `-r`: Print profiling/timing data for each rule
- **Use `--print-program`** to see generated Datalog rules
- **Test edge cases**: Include tests for large cardinality (>5000 rows to test multiple batches), cross products, filters, and various combinations and use python code to help you with the creation of expected output.

### Performance Testing
- After verifying correctness in debug, build and test in release:
  ```bash
  cd cmake-build-release && cmake --build .
  time ./cmake-build-release/BumbleBee -i test/e2e/files/asp/input/<category>/<name>
  ```

## Debugging Tips

- Use `--print-program` flag to see generated Datalog rules before execution
- Use `-p` flag to enable debug logging
- When investigating bugs or unexpected behavior, use LOG_DEBUG statements to log relevant data during execution. This function follows the same formatting rules as printf, so you can use formatted strings to include variable values and contextual information. 
  - These logs are meant only for debugging purposes.
  - Once the issue is resolved, remove any LOG_DEBUG statements that were added during troubleshooting to keep the codebase clean.
