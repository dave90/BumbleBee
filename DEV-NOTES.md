
## Components
- common
    - log
    - data structures
    - compilation
    - memory manager

- third_party
    - arrow
- catalog
    - db
    - predicate table (with statistics)

- parser
    - parser
    - rule
        - atoms
    - annotations
- optimizer
    - logical plan
        - dag
        - rewriter
            - push filter
            - projection
            ...
    - pyshical plan
- execution (query evaluation)
    - pyshical rule
        - pyshical atoms
        - output atom
        - hash_join_atom
        - compiled_atom 
            - compiled using a script
    - states
- parallel
    - scheduler
    - task
    - task execution

Commons: contains common utility and data structures. Define also constants and common type used.

third_party: the third party components (like arrow).

catalog: contains the catalog of the db. initially only the predicates table ad db. A datalog program is mapped to a db. In the future will contains also views 

parsers: will parse the input string and contains the class of the datalog program (atoms rule etc.)

optimizer: manage the creation of the plans (logical and physical). Logical plan will create the dag of the rules (with sql will convert sql to rules) and rewrites the rules with the rule rewriters. Physical plan given the logic plan will convert rules to pushical rule for the push data chunk execution and parallelism (with states). Will convert rules only components by components (to have more statistics).

execution: contains the physical class for the execution.  Filters are putted inside the physical atom. contains also the compiled atom execution

parallel: manage the execution of the rules in parallel. Each rule body can be execution as a pipeline in parallel. For the head mutex are needed calling the sink. 

### Common
Data structure for data chunk. Data chunks contains vectors (column storage). Each vector can be created by other vector 
without copy data (cow). These chunks are passed among operators without copying (copy only when necessary). Materialize data
only when needed (output, join table etc.)


### Executor
Physical Rule contains: one partitioned source (the atom that source the data). Other Sources (not partitioned)  and filters (aggregators ?)
Then the sink operator: the head. The head can be output head (output data), or intermediate data. intermediate data are stored in cache in catalog.

- Physical Rule
  - Source
  - body: List[Operators]
  - head: Operator
- Physical Operator
  - Scan Operator
  - Filtered Scan
  - Filter operator
  - Compiled Operator (in the future)
  - Hash Join Build Operator
  - Hash Join Probe Operator
  - Aggregate ???

Each column of a data chunk is a variable
Each operator can return 3 state : FINISH, NEED MORE INPUT, HAVE MORE OUTPUT.
State , State Global and State Local ? maybe not
Each operator receive input and output chunk

### Catalog
Multiple DBs, at least one (the default). Each DB contains a predicates catalogs  that contains predicate table (or tables). Each table can be permanent data
so is shared among runs and temp table (so temporary data of the run, at the end will be deleted). Also each predicate table contains
a cache (to store cached data) that can be reused during a run. The predicate table contains also the index and htables

- DBs
    - Preidcate Catalogs
      - Predicate Table
        - permanent data
        - temp data
        - cache
        - index

### Optimizer
Given a set of logical rules create a DAG. For each DAG do the Logical to Logical optimization.
The optimizare is responsible also to generate the Physical rules. From a Rule multiple physical rules can be generate.
For example for a join a rule need to be split (build and probe phase). 


### Parallel
Each rule is splitted in Task. Each task can be executed in parallel. Each task is an execution of the rule
for a single partition. A list of the task is putted in a queue by the scheduler. The scheduler is responsible for 
the creation of the task and insertion in the quque. This process is done for each DAG created by the optimizer. before the
execution the rules are transformed in physical rules (generate the physical rule in late stage to have more statistics)
The executor take a task from the ququq and execute it.
For the recursive rules: 


## Future works

- SIMD
- Python client library
- Accepts SQL and ASP input languages
- Multi Way Join
- Optimizer (CASCADE) and Adaptive Optimizer
- Multi Workers
- Write to disk
- Server mode

