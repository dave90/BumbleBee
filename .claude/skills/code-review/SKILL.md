---
name: code-review
description: Comprehensive code review using git diff. Analyzes changes for bugs, optimizations, refactoring opportunities, and test coverage. Use when the user asks to review code, check changes, look for bugs, suggest improvements, review a PR/commit, or verify test coverage.
---

# Code Review Skill for BumbleBee DB

## Description

Performs comprehensive code review of staged/uncommitted changes in the BumbleBee Datalog analytics engine. Analyzes C++ code for bugs, performance issues, code quality, and test coverage gaps.

## Activation

This skill activates when:
- User requests a code review
- User asks to check changes before commit
- User wants to review a diff or PR

## Instructions

When activated, perform the following code review workflow:

### Step 1: Gather Changes

Run `git diff` to retrieve all current uncommitted changes. For staged changes only, use `git diff --cached`.

### Step 2: Analyze Code Changes

For each modified file, check for:

#### Bugs & Correctness
- **Memory safety**: Dangling pointers, use-after-free, buffer overflows
- **Type mismatches**: Using `int` where `idx_t` is expected (especially for indices/counts)
- **Assertion order**: Ensure bounds checks happen before array access
- **Null/empty checks**: Verify containers are checked before access
- **Thread safety**: Race conditions in parallel code, proper mutex usage
- **Resource leaks**: Unclosed files, unreleased memory

#### BumbleBee-Specific Checks
- **Parser changes**: If `aspcore2.l` or `aspcore2.y` modified, verify regenerated files match
- **Aggregate handling**: Multi-aggregate payload/group column consistency
- **Physical operators**: Proper `AtomResultType` return values (FINISHED, NEED_MORE_INPUT, HAVE_MORE_OUTPUT)
- **Hash table operations**: Correct entry/address handling in join and aggregate HTs
- **Vector operations**: Proper selection vector usage, cardinality updates
- **DataChunk lifecycle**: Correct initialization, reference vs copy semantics

#### Performance & Optimization
- **Unnecessary loops**: Code that iterates when a single call suffices
- **Repeated allocations**: Vectors/chunks that could be cached
- **Missing parallelism**: Opportunities for multi-threaded execution
- **Redundant copies**: Data that could be referenced instead of copied

#### Code Quality
- **Duplicated logic**: Similar code blocks that should be extracted to helpers
- **Magic numbers**: Constants that should be defined in Constants.h
- **Naming**: Variable/function names that don't reflect purpose
- **Dead code**: Commented-out code, unreachable branches
- **Include hygiene**: Missing or unnecessary includes

### Step 3: Review Test Coverage

Examine test files in:
- `test/unit/` - Google Test unit tests
- `test/e2e/files/asp/input/` - ASP end-to-end tests
- `test/e2e/files/sql/input/` - SQL end-to-end tests

For each changed feature, verify:
- **Unit test exists**: Core logic has dedicated unit tests
- **E2E coverage**: Integration tests exercise the feature
- **Edge cases covered**:
  - Empty data/relations
  - Large cardinality (>5000 rows for batch testing)
  - Boundary conditions
  - Error cases with expected failures
- **Missing scenarios**: Identify gaps in test coverage

### Step 4: Generate Action Plan

Create/update `plan.md` at the project root with:

```markdown
# Code Review Action Plan

## 1. Bugs to Fix
### 1.1 [Bug Title] (Priority: High/Medium/Low)
**File:** `path/to/file.cpp:line`
**Issue:** Description of the bug
**Action:** How to fix it

## 2. Code Improvements
### 2.1 [Improvement Title] (Priority: High/Medium/Low)
**File:** `path/to/file.cpp`
**Issue:** Description of the issue
**Action:** Suggested refactoring

## 3. Tests to Add
### 3.1 [Test Name] (Priority: High/Medium/Low)
**File:** `test/e2e/files/asp/input/category/test.name`
**Test Case:** Description and example code
**Expected:** Expected output

## 4. Verification Steps
1. Build debug: `cmake --build cmake-build-debug --target BumbleBee -j 8`
2. Run unit tests: `cd cmake-build-debug && ctest -j`
3. Run e2e tests: `cd test/e2e && pytest test.py`

## Priority Summary
| Priority | Item | Type |
|----------|------|------|
| High | ... | Bug/Test/Improvement |
```

## Output Format

Provide a summary to the user with:
1. **Overview**: Brief description of what was reviewed
2. **Key Findings**: Most important issues found (top 3-5)
3. **Test Gaps**: Missing test scenarios
4. **Plan Location**: Confirm `plan.md` has been updated

## Example Usage

User: "Review my changes"
Agent:
1. Runs `git diff`
2. Analyzes each changed file
3. Checks related test files
4. Updates `plan.md` with findings
5. Provides summary to user

## Notes

- Always read files before suggesting edits
- Focus on the actual changes, not unrelated code
- Prioritize optimization and correctness over style
- For parser changes, remind about `./build_parser.sh`
- Reference specific line numbers when reporting issues