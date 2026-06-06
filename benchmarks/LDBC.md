# LDBC SNB benchmark — BumbleBee vs DuckDB vs Neo4j

Compares BumbleBee (Datalog), DuckDB (SQL), and Neo4j (Cypher) on a set of LDBC
Social Network Benchmark *Interactive v1* read queries — six non-recursive plus two
**recursive** (reachability / transitive-closure) — over the same graph dataset.
Results land in `results/*_ldbc_benchmark_results_*.csv` (same format as the other
suites; `delta_vs_cmp` is each engine vs DuckDB).

## Queries

| Query | Description | Recursion |
|-------|-------------|-----------|
| IS1 | Profile of a person | none (single-table lookup) |
| IS3 | Friends of a person (1-hop) | none |
| IS7 | Replies to a message + does replier know the author | none (+ knows-flag) |
| IC2 | 20 most recent messages of a person's friends | none |
| IC3 | Friends/FoF outside two countries who posted in both | bounded 2-hop join |
| IC8 | 20 most recent replies to a person's messages | none |
| **REACH** | Persons reachable from a person via `KNOWS` within ≤3 hops | **recursive** |
| **RCHAIN** | All messages that transitively reply to a message (`REPLY_OF`) | **recursive** |

The recursive queries use **transitive closure / reachability**, which each engine
expresses natively: BumbleBee recursive Datalog, DuckDB `WITH RECURSIVE`, Neo4j
variable-length paths (`*1..3` / `*`). Shortest-path-*length* queries (IC13/IC14) are
still excluded — BumbleBee has no min-over-recursion, so those are not fairly
comparable; reachability (does a path exist / which nodes are reachable) is. REACH
excludes the source node so the reachable set matches Neo4j's no-relationship-reuse
semantics; RCHAIN walks a reply tree (acyclic), so it terminates unbounded.

### Per-engine query language
BumbleBee's SQL subset lacks `LEFT JOIN`, `CASE WHEN`, `HAVING`, and string/date
functions, so **BumbleBee queries are written in Datalog** (`input_bumblebee/*.dl`)
— implicit joins, grouped `#count`, `#order_by`/`#limit`, and `not` (to emulate
`LEFT JOIN`/`NOT EXISTS`). DuckDB uses SQL (`input_duckdb/*.sql`), Neo4j uses
Cypher (`input_neo4j/*.cypher`). Cosmetic projections needing unsupported features
(free-text content, the IS7 "knows" boolean) are reduced to the **comparable core**
(ids + counts + a 0/1 flag) so all three outputs match value-for-value.

Parameters are baked in as literals (see `files/ldbc/params.json`) so every engine
runs identical bindings. For a real dataset, pick valid ids/dates and re-substitute.

## Data

All engines read one canonical, normalized CSV set built by `prepare_ldbc.py`:

    downloads/ldbc/sf<N>/{person,knows,message,place}.csv

mirroring the upstream LDBC DuckDB loader: `message = post ⊔ comment`, `knows` is
materialized **both directions**, temporal columns are **epoch-second integers**
and birthday is `YYYYMMDD` (so ordering/equality is identical across engines, with
no timestamp-format drift). Schema is documented in `params.json`.

```bash
# 1. Download the official SF1 dataset from LDBC's R2 host, extract, and normalize.
#    (Needs curl + zstd: `brew install zstd`.)
python prepare_ldbc.py --sf 1 --download
#    ...or normalize a dataset you already have (an extracted social_network/ folder):
#    python prepare_ldbc.py --sf 1 --raw-dir /path/to/social_network

# 2. Bake valid query literals (a real personId/messageId/countries) into the query
#    files + params.json, picked from the loaded data:
python prepare_ldbc.py --sf 1 --pick-params

# 3. (Neo4j only) load the normalized CSVs into a running Neo4j:
python prepare_ldbc.py --sf 1 --neo4j-load --neo4j-import-dir /path/to/neo4j/import
```

`--download` fetches `social_network-sf<N>-CsvCompositeMergeForeign-LongDateFormatter.tar.zst`
directly from the LDBC Cloudflare R2 host (`datasets.ldbcouncil.org/snb-interactive-v1/`),
extracts it, and normalizes. Date parsing is robust to both the LongDateFormatter
(epoch-millisecond) and StringDateFormatter (ISO) datagen variants. Other scale
factors: `--sf 3`, `--sf 10`, etc. (subject to availability on the host).

> **Query parameters.** The committed query literals (`personId=100`, …) exist only in
> the smoke dataset, *not* in real SF1. **`--pick-params`** rewrites every query file +
> `params.json` with a valid `personId` (a message author who has friends), `messageId`
> (a message that has replies), and the two most-posted countries — chosen from the
> loaded data, so IS1/IS3/IS7/IC2/IC8 return non-empty results. It reads the current
> literals from `params.json`, so it's idempotent and works for any dataset (including
> re-running on the smoke set). Without it, queries on non-smoke data return empty.

## Scale factors (bigger input data)

Every standard LDBC SNB Interactive v1 scale factor is available on the R2 host; pass
`--sf N` to `prepare_ldbc.py`:

| SF | Download (compressed) | ~Persons | ~Messages |
|----|----------------------:|---------:|----------:|
| 1 (default) | 0.21 GB | ~10k  | ~3M   |
| 3           | 0.67 GB | ~27k  | ~9M   |
| 10          | 2.33 GB | ~73k  | ~30M  |
| 30          | 7.32 GB | ~180k | ~90M  |
| 100         | 24.4 GB | ~500k | ~300M |
| 300         | 72.9 GB | —     | ~900M |
| 1000        | 244 GB  | —     | ~3B   |

```bash
python prepare_ldbc.py --sf 10 --download        # download + normalize a bigger dataset
python prepare_ldbc.py --sf 10 --pick-params     # rebind query literals to ids in SF10
python prepare_ldbc.py --sf 10 --neo4j-load --neo4j-import-dir "$(brew --prefix neo4j)/libexec/import"
```

The query files read a stable path **`downloads/ldbc/active/`**, and `prepare_ldbc.py`
points `active → sf<N>` at whichever scale factor you prepare — so switching SF needs no
edits to the query files. (DuckDB `read_csv_auto` and BumbleBee `&read_csv` read through
`active/`; Neo4j queries the loaded graph.)

**Caveats when scaling up:**
- **Performance scales with size.** BumbleBee re-parses the CSVs on every run, so the
  sub-2s SF1 timings grow roughly linearly (SF10's `message.csv` is ~1.5 GB). DuckDB
  stays fast; the Neo4j *load* gets much slower (SF10 ≈ 30M message nodes — tens of
  minutes). Bigger data makes the engine differences more visible, but "all queries
  under 2s" is an SF1 property. **SF3 or SF10** is a good heavier-but-manageable run.
- Disk: extracted raw data + normalized CSVs ≈ 3–4× the compressed download.

## Neo4j setup (Homebrew, no Docker)

```bash
brew install neo4j
neo4j-admin dbms set-initial-password bumblebee     # >=8 chars, before first start
neo4j start                                         # http://localhost:7474, bolt :7687
cypher-shell -a neo4j://localhost:7687 -u neo4j -p bumblebee "RETURN 1;"   # verify

# load the LDBC graph (import dir is under the formula's libexec on Homebrew):
NEO4J_PASSWORD=bumblebee python prepare_ldbc.py --sf 1 --neo4j-load \
    --neo4j-import-dir "$(brew --prefix neo4j)/libexec/import"
```

## Running

```bash
export NEO4J_PASSWORD=bumblebee
./run_ldbc.sh                 # duckdb (reference) -> bumblebee -> neo4j (if cypher-shell present)
# or individually:
python benchmark_runner.py configs/duckdb_ldbc.json     # writes files/ldbc/expected/*.csv
python benchmark_runner.py configs/bumblebee_ldbc.json  # compares vs expected (BumbleBee runs with -d)
python benchmark_runner.py configs/neo4j_ldbc.json      # compares vs expected
```

Neo4j connection is read from `NEO4J_URI` / `NEO4J_USER` / `NEO4J_PASSWORD`
(defaults `neo4j://localhost:7687` / `neo4j` / `bumblebee`). BumbleBee runs with `-d`
(distinct) so multi-path derivations don't duplicate rows under `#limit`; comparison is
set-based. All 8 queries report `output_match = match` for both BumbleBee and Neo4j vs DuckDB.

### Neo4j load is counted in the benchmark (timed LOAD step)
DuckDB and BumbleBee parse the CSVs inside *every* query, so their timings include the
data load. To keep Neo4j comparable, `configs/neo4j_ldbc.json` runs a first **`LOAD`**
test (`bash neo4j_load.sh`, `num_tries:1`) that **wipes all data and rebuilds the graph**
from `downloads/ldbc/active/`, timed — so the Neo4j results CSV shows a `LOAD` row (its
`avg` = seconds to get the data in) alongside the per-query rows. You never load Neo4j
manually for the benchmark. The wipe is heap-free (stop → delete the `neo4j` database
files → restart), so it works at any scale; `neo4j_load.sh` also raises the JVM heap /
page-cache (`NEO4J_HEAP`/`NEO4J_PAGECACHE`, default 4G/5G) so large scale factors don't
OOM. Example (SF10, 16 GB machine): LOAD ≈ 762s, then all 8 queries match (IC3 ≈ 10s).

## How comparison works

DuckDB is the reference: `configs/duckdb_ldbc.json` writes one CSV per query to
`files/ldbc/expected/`. The BumbleBee and Neo4j configs set `result_format`
(`datalog` / `cypher_plain`); `ldbc_normalize.py` converts each engine's raw output
to a canonical multiset of value-tuples (quotes stripped, floats rounded, header
dropped) and compares it to the expected CSV — set-equality, so row order doesn't
matter (`LIMIT` queries still select the same top-N because each query's `ORDER BY`
has a unique tiebreaker). `output_match` in the results CSV is `match`/`mismatch`.

## Smoke test (no datagen needed)

A tiny synthetic dataset with known answers is committed under `files/ldbc/sample/`.
Copy it into the data path to verify the query translations and the harness without
generating real LDBC data:

```bash
mkdir -p downloads/ldbc/sf1 && cp files/ldbc/sample/*.csv downloads/ldbc/sf1/
ln -sfn sf1 downloads/ldbc/active                        # queries read downloads/ldbc/active/
python prepare_ldbc.py --sf 1 --pick-params              # bind literals to the smoke ids
python benchmark_runner.py configs/duckdb_ldbc.json      # build expected
python benchmark_runner.py configs/bumblebee_ldbc.json   # expect all 8 -> output_match=match
```

(`--pick-params` is needed here because the query files may carry literals from a
previous real-data run; it re-binds them to ids that exist in whatever is loaded.)

`prepare_ldbc.py` overwrites `downloads/ldbc/sf1/` with real data when you run it.