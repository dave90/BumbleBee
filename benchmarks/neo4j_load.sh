#!/usr/bin/env bash
# Fresh-load the `active` scale factor into Neo4j for the benchmark's timed LOAD step.
# Wipes ALL existing data first (stop -> delete the `neo4j` database files -> start), which
# is heap-free and works at any scale (DETACH DELETE of a big graph OOMs the JVM heap).
# Then copies the normalized CSVs into Neo4j's import dir and runs neo4j_load.cypher.
#
# So Neo4j's benchmark numbers include the cost of getting the data in (wipe + build) —
# parity with DuckDB/BumbleBee, which parse the CSVs inside every query.
set -e
cd "$(dirname "$0")"

URI="${NEO4J_URI:-neo4j://localhost:7687}"
USER="${NEO4J_USER:-neo4j}"
PW="${NEO4J_PASSWORD:-bumblebee}"
IMPORT="${NEO4J_IMPORT_DIR:-$(brew --prefix neo4j)/libexec/import}"
DATA="${NEO4J_DATA:-$(brew --prefix)/var/neo4j/data}"
SRC="${1:-downloads/ldbc/active}"

# heap/page-cache big enough for the load (16 GB machine -> 4G/5G; override via NEO4J_HEAP/_PAGECACHE)
export NEO4J_server_memory_heap_initial__size="${NEO4J_HEAP:-4G}"
export NEO4J_server_memory_heap_max__size="${NEO4J_HEAP:-4G}"
export NEO4J_server_memory_pagecache_size="${NEO4J_PAGECACHE:-5G}"

echo "[neo4j-load] wiping ALL data (stop, delete db files, start)"
neo4j stop >/dev/null 2>&1 || true
rm -rf "$DATA/databases/neo4j" "$DATA/transactions/neo4j"
neo4j start >/dev/null
echo "[neo4j-load] waiting for Neo4j to accept queries"
until cypher-shell -a "$URI" -u "$USER" -p "$PW" "RETURN 1;" >/dev/null 2>&1; do sleep 2; done

echo "[neo4j-load] copying CSVs from $SRC -> $IMPORT"
cp "$SRC"/person.csv "$SRC"/knows.csv "$SRC"/message.csv "$SRC"/place.csv "$IMPORT/"

echo "[neo4j-load] loading graph"
cypher-shell -a "$URI" -u "$USER" -p "$PW" -f neo4j_load.cypher

echo "[neo4j-load] done"