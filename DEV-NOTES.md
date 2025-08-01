# 🐝 BumbleBee DB — Developer Notes

## 🧩 Components Overview

### `common/`
Core utilities and shared infrastructure:
- Logging
- Generic data structures
- Compilation utilities
- Memory manager
- Constants and shared types

### `third_party/`
External dependencies (e.g., Hyper log log).

### `catalog/`
Handles database and predicate metadata:
- Database instances
- Predicate catalog per DB
- Predicate tables (with support for statistics)
- Caching layer and index management

Each predicate table includes:
- **Permanent data** (shared across runs)
- **Temporary data** (discarded after a run)
- **Cache** for intermediate results
- **Indexes** and **Hash tables**
- **Statistics** via sampling and HyperLogLog

### `parser/`
Parses input strings (Datalog/SQL):
- Tokenizers and grammar (via Flex/Bison)
- Atoms, rules, annotations, and internal program model

### `planner/`
Builds and optimizes query plans:

- **Logical Plan**:
    - Constructs DAG from rules
    - SQL → Datalog rule conversion
    - Rule rewriters (e.g., filter pushdown, projection folding)

- **Physical Plan**:
    - Converts logical plan into executable operators
    - Designed for push-based execution and parallelism
    - Uses statistics to optimize rule generation

### `execution/`
Evaluates physical plans:
- Contains `physical_rule`, `physical_atom`, and `compiled_atom`
- Manages data movement, execution state, filtering, and output
- Encapsulates compiled operators (via scripts/codegen)

### `parallel/`
Handles task parallelism:
- Rule splitting into **tasks** per partition
- Scheduler manages task creation and queuing
- Executor processes task queue
- Sink/head operators are synchronized using locks/mutexes

---

## 🧠 Core Concepts

### Data Model: `DataChunk` and `Vector`
- **DataChunk**: container for columnar data (vectors)
- **Vector**:
    - Holds typed data and selection vector
    - Can be:
        - **Regular**
        - **Constant**
        - **Sequence**
    - Managed by a Data Manager (or memory buffer)

Materialization is deferred and data is copied only when necessary.

---

## ⚙️ Execution Model

### 🔄 High-Level Execution Overview

1. **Parsing Rules**
    - Input rules (written in Datalog or SQL) are parsed into an internal program structure.
    - The parser constructs atoms, rules, and annotations.

2. **Building the Logical DAG**
    - A **Directed Acyclic Graph (DAG)** is created to model dependencies between rules.
    - Connected components within the DAG are identified and ordered for evaluation.

3. **Processing Components**
    - Each connected component is processed in topological order.
    - All rules within a component are prepared for logical and physical planning.

4. **Logical Optimization (Planner Stage)**
    - The planner applies **logical-to-logical transformations**, including:
        - Filter pushdown
        - Atom reordering
        - Rule simplification
    - This phase is **extensible** to support advanced optimizers (e.g., a Cascades-style optimizer).

5. **Physical Plan Generation**
    - Logical rules are converted into one or more **physical rules**:
        - **Sink-based decomposition**: e.g., a join rule is split into build and probe phases.
        - **Rule partitioning**: physical rules are generated for each partition, enabling parallel execution.
    - Each physical rule is assigned a **priority weight**, which influences its scheduling.
   - Recursive rules are handled similarly to non-recursive ones, with a few key differences:
       - In each iteration, rules are optimized into **physical rules**, allowing for adjustments such as a dynamic change in the number of partitions per rule based on delta.
       - The **head atom** stores its results in a **hash table** within the predicate table (to enable efficient deduplication and lookup).
       - If a rule produces **new data** (i.e., data not already present in the hash table), it is **scheduled for execution in the next iteration**.
       - If no new data is generated, the rule is **excluded from future iterations**.
     - This process continues in a loop until **no recursive rule produces new results**, ensuring a **fixed-point** is reached.

6. **Execution Scheduling**
    - The execution engine schedules physical rules using a **priority queue**.
    - Execution is guided by:
        - Rule priority
        - Available resources
        - Dependency resolution

### Physical Rule Structure

Each physical rule includes:
- **Partitioned source atom**
- **Non-partitioned sources** and **filters**
- **Sink operator (head)**: either output or intermediate storage (e.g., join hash table)

#### Operators:
- `ScanOperator`
- `FilteredScan`
- `FilterOperator`
- `HashJoinBuildOperator`
- `HashJoinProbeOperator`
- `OutputOperator`
- `OutputHashOperator`
- `CompiledOperator` *(planned)*
- `AggregateOperator` *(planned)*

Each operator returns one of:
- `FINISHED`
- `NEED_MORE_INPUT`
- `HAS_MORE_OUTPUT`

Operators receive input/output chunks and are executed in a pipeline.

---

## 📚 Catalog Design

- **Multiple DBs**, starting with one default
- Each DB has a **predicate catalog** with:
    - **Permanent and temp tables**
    - **Cache**
    - **Indexes**
    - **Statistics**

Catalog tables hold:
- Base data
- Intermediate results
- Hash tables for joins
- Estimated stats (HyperLogLog, sampling)

---

## 🧠 Optimizer

- Converts rules to **logical DAG**
- Applies logical optimizations
- Generates **physical rules** using statistics
- A single rule may produce multiple physical stages (e.g., build and probe for joins)

---

## 🧵 Parallel Execution

- Each rule is divided into **tasks** by partition
- **Scheduler**:
    - Generates tasks per rule/DAG
    - Queues them for execution
- **Executor**:
    - Fetches and runs tasks
- Physical plans are generated **late** for better optimization

*(Recursive rule handling is planned)*

---

## 🧠 Execution Data Structures

### DataChunk
- Columnar format
- Passed among operators with minimal copying

### Vector
- Holds:
    - Data pointer
    - Selection vector (for filtering)
    - Type metadata
- Can be:
    - Constant
    - Sequence
    - Dicitonary
- Managed by `VectorMemoryManager`
    - Supports type-safe access
    - Handles heap strings

---

## Profiling

To enable the profiling set  **PROFILING** to 1  in _src/include/bumblebee/common/Constants.h_ and compile **BumbleBee**

Then to print the profiling use the option _- r_


## Build parser
```
	flex -o src/parser/aspcore2_lexer.hpp src/parser/aspcore2.l
	bison -y -d -o src/parser/aspcore2_parser.c src/parser/aspcore2.y
	bison -y -o src/parser/aspcore2_parser.hpp src/parser/aspcore2.y
```


## Clion
Sometime clean the cmake Tools -> Cmake -> Reset Cache and Reload

If Clion cannot load the the CMake project then delete the .idea folder

## Build

### Release
```bash
mkdir cmake-build-release
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake build .
```

### Debug
```bash
mkdir cmake-build-debug
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake build .
```

## 🧪 Benchmark Commands

### Run Clingo (ASP Baseline)
```bash
cd benchmarks
python benchmark_runner.py configs/clingo_asp.json 
```

### Run Bumblebee (ASP)

```bash
python benchmark_runner.py configs/bumblebee_asp.json
```

## Unit Test


Run:
```bash
cd cmake-build-debug
ctest -j
```

## End to End (E2E) Test

Run:
```bash
cd test/e2e
pytest test.py
```
