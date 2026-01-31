# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

BumbleBee DB is a high-performance Datalog-based analytics engine written in C++20. It supports both Datalog and SQL as input languages, with push-based execution, columnar storage, and multithreading. Data sources include CSV, Parquet, and ASP files.

## Build Commands

```bash
# Release build
mkdir -p cmake-build-release && cd cmake-build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .

# Debug build (includes AddressSanitizer)
mkdir -p cmake-build-debug && cd cmake-build-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
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

## Benchmarks

```bash
cd benchmarks
python benchmark_runner.py configs/bumblebee_sql.json   # SQL benchmarks
python benchmark_runner.py configs/bumblebee_asp.json   # ASP benchmarks
python benchmark_runner.py configs/clingo_asp.json      # Clingo baseline
```

## Parser Regeneration

When modifying `aspcore2.l` or `aspcore2.y`:
```bash
flex -o src/parser/aspcore2_lexer.hpp src/parser/aspcore2.l
bison -y -d -o src/parser/aspcore2_parser.c src/parser/aspcore2.y
bison -y -o src/parser/aspcore2_parser.hpp src/parser/aspcore2.y
```

## Profiling

Enable profiling by setting `PROFILING` to 1 in `src/include/bumblebee/common/Constants.h`, rebuild, then run with `-r` flag.

## Architecture

### Execution Pipeline

1. **Parsing** (`parser/`): Input (Datalog or SQL) is parsed via Flex/Bison into an internal program structure. SQL is converted to Datalog rules.

2. **Planning** (`planner/`): Rules are organized into a DAG, optimized (filter pushdown, atom reordering), then converted to physical rules. A single logical rule may produce multiple physical stages (e.g., build/probe for joins).

3. **Scheduling** (`parallel/`): Physical rules are split into tasks by partition and queued. The `Scheduler` creates tasks; `TaskExecutor` runs them across threads.

4. **Execution** (`execution/`): Push-based operators process `DataChunk` containers with columnar `Vector` data. Operators return `FINISHED`, `NEED_MORE_INPUT`, or `HAS_MORE_OUTPUT`.

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
- `src/storage/` - Hash tables for joins and aggregation

### Recursion Handling

Recursive rules are processed iteratively until a fixed-point is reached. Each iteration re-optimizes physical rules. Head predicates store results in hash tables for deduplication. Processing stops when no new data is produced.

## Code Conventions

### Common Utilities
- String parsing utilities belong in `StringUtils` (`src/common/StringUtils.cpp`) - avoid duplicating parsing functions across readers
- Shared constants should go in common headers, not be redefined in multiple files

### SQL to Datalog Conversion (`SqlToDatalog.cpp`)
- SQL queries are translated to Datalog rules via `SqlToDatalog::sqlToDatalog()`
- Variable names follow pattern `COLUMN.#p_N` where N is the table alias index
- The `columns_mapping` named parameter maps Datalog variable names to actual column names (format: `VAR1:COL1;VAR2:COL2`)
- First FROM table can use external atom directly; subsequent tables generate auxiliary rules

### External Atoms
- Format: `&func_name(input_values;named_param1=val1,named_param2=val2;term1,term2,...)`
- Sections separated by `;`: inputs, named parameters (comma-separated), terms
- File readers (`&read_csv`, `&read_parquet`) support `columns_mapping` parameter for variable-to-column translation

### Anonymous Variables
- The underscore `_` represents anonymous/don't-care variables
- The optimizer may introduce `_` when rewriting rules to remove unused variables
- File readers and column mapping functions must handle `_` specially (skip validation, use placeholder types)

### Aggregation
- `#ID` variable used in aggregates for duplicate handling (avoids distinct calculation)
- `&gen_id` atom generates unique IDs when needed
- Aggregate atoms format: `VAR = #agg{agg_terms:agg_body_atoms}`

#### Aggregate Semantics
- **Aggregate terms** (`agg_terms`): specifies what to aggregate, NOT the groups
  - `#sum{X:body}` - sum the values of X
  - `#count{X,Y,#ID:body}` - count tuples (with #ID for uniqueness)
- **Groups**: determined by variables **shared** between rule body and aggregate body

Example:
```datalog
result(X,Y,s) :- a(X,Y), s = #sum{Z:b(X,Y,Z)}.
```
- Rule body: `a(X,Y)` → variables X, Y
- Aggregate body: `b(X,Y,Z)` → variables X, Y, Z
- Shared variables: X, Y → **these are the groups**
- Z is local to aggregate, S is the sum of Z values

## AI Development Workflow

### Before Making Changes
- **Always describe the plan first**: Before modifying any code, provide a clear description of what changes will be made and why. Wait for user approval before proceeding.

### After Making Changes
- **Compile in debug mode** to verify the code compiles correctly:
  ```bash
  cd cmake-build-debug && cmake --build .
  ```
- Fix any compilation errors before proceeding.

### Testing Changes
- **Write test programs in `test.1`** (or `test.2`, `test.3`, etc.) at the project root
- **Run the debug build** to test:
  ```bash
  ./cmake-build-debug/BumbleBee -i test.1
  ```
- **Use `-p -r` flags** for detailed output:
  ```bash
  ./cmake-build-debug/BumbleBee -i test.1 -p -r
  ```
  - `-p`: Print debug logs
  - `-r`: Print profiling/timing data for each rule
- **Use `--print-program`** to see the generated Datalog rules:
  ```bash
  ./cmake-build-debug/BumbleBee -i test.1 --print-program
  ```

### Performance Testing
- After verifying correctness in debug, build and test in release:
  ```bash
  cd cmake-build-release && cmake --build .
  time ./cmake-build-release/BumbleBee -i test.1
  ```

## Debugging Tips

- Use `--print-program` flag to see generated Datalog rules before execution
- Use `-p` flag to enable debug logging
- Debug build includes AddressSanitizer - assertion failures show as `SIGABRT`
- Check `Testing/Temporary/LastTest.log` for detailed test failure output
- Run single test with verbose: `ctest -R <test_name> --output-on-failure -V`
