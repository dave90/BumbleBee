#!/usr/bin/env bash
set -euo pipefail

# Usage:
#   ./parquet_to_csv.sh /path/to/parquets /path/to/output_csvs

IN_DIR="${1:-}"
OUT_DIR="${2:-}"

if [[ -z "$IN_DIR" || -z "$OUT_DIR" ]]; then
  echo "Usage: $0 <input_parquet_folder> <output_csv_folder>" >&2
  exit 1
fi

if [[ ! -d "$IN_DIR" ]]; then
  echo "Error: input folder does not exist: $IN_DIR" >&2
  exit 1
fi

mkdir -p "$OUT_DIR"

# If there are no matches, the glob should expand to nothing (not itself)
shopt -s nullglob

parquet_files=("$IN_DIR"/*.parquet)
if (( ${#parquet_files[@]} == 0 )); then
  echo "No .parquet files found in: $IN_DIR" >&2
  exit 0
fi

for parquet_path in "${parquet_files[@]}"; do
  base="$(basename "$parquet_path" .parquet)"
  csv_path="$OUT_DIR/$base.csv"

  # Escape single quotes for SQL string literals
  parquet_sql=${parquet_path//\'/\'\'}
  csv_sql=${csv_path//\'/\'\'}

  echo "Converting: $parquet_path -> $csv_path"

  # Run the COPY statement directly via -c (no heredoc / stdin)
  duckdb -c "COPY (SELECT * FROM read_parquet('$parquet_sql')) TO '$csv_sql' (HEADER, DELIMITER ',');"

  # Optional sanity check: ensure file has content
  if [[ ! -s "$csv_path" ]]; then
    echo "WARNING: CSV is empty: $csv_path" >&2
  fi
done

echo "Done. CSVs written to: $OUT_DIR"
