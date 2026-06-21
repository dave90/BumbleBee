#!/usr/bin/env bash
# LDBC SNB (non-recursive) benchmark: BumbleBee vs DuckDB vs Neo4j.
# Run engines sequentially (no concurrent load during a timed run).
#
# Prerequisites:
#   - Normalized data prepared:  python prepare_ldbc.py --sf 1 --raw-dir <datagen-out>
#   - duckdb on PATH
#   - For Neo4j: a running Neo4j + cypher-shell on PATH, data loaded:
#       python prepare_ldbc.py --sf 1 --neo4j-load --neo4j-import-dir <neo4j>/import
#     and NEO4J_URI / NEO4J_USER / NEO4J_PASSWORD exported if not the defaults.
set -e
cd "$(dirname "$0")"

# Default the Neo4j password to the documented Homebrew-setup value so a run can't
# hammer Neo4j with a wrong password and trip its auth lockout. Override by exporting
# NEO4J_PASSWORD (and NEO4J_URI / NEO4J_USER) before running.
export NEO4J_PASSWORD="${NEO4J_PASSWORD:-bumblebee}"

echo "== DuckDB (reference / expected) =="
python benchmark_runner.py configs/duckdb_ldbc.json

echo "== BumbleBee =="
python benchmark_runner.py configs/bumblebee_ldbc.json

if command -v cypher-shell >/dev/null 2>&1; then
  echo "== Neo4j =="
  python benchmark_runner.py configs/neo4j_ldbc.json
else
  echo "== Neo4j skipped (cypher-shell not found) =="
fi

echo "Done. See results/*_ldbc_benchmark_results_*.csv"