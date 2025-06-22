
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
- planner
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
planner: manage the creation of the plans (logical and pyshical). Logical plan will create the dag of the rules (with sql will convert sql to rules) and rewrites the rules with the rule rewriters. Physical plan given the logic plan will convert rules to pushical rule for the push data chunk execution and parallelism (with states). Will convert rules only components by components (to have more statistics).

execution: contains the pyshical class for the execution.  Filters are putted inside the pyshical atom. contains also the compiled atom execution

parallel: manage the execution of the rules in parallel. Each rule body can be execution as a pipeline in parallel. For the head mutex are needed calling the sink. 


## Future works

- SIMD
- Python client library
- Accepts SQL and ASP input languages
- Multi Way Join
- Rule rewriter
- Multi Workers
- Write to disk


## To test and practice
- Compiling during runtime
- CMkae
- Arrow (reading csv parquer) and sending
- 