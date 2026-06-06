#!/usr/bin/env python3
"""
Generates the large null-bearing reader fixtures used by the SQL e2e tests
`null_csv_large` / `null_parquet_large`.

Why this exists: every other null reader fixture is tiny (<= 10 rows), so the
CSV/Parquet reader's multi-chunk (> STANDARD_VECTOR_SIZE = 1024) and
multi-row-group definition-level decoding *with NULLs* was never exercised
end-to-end. This produces 6000 rows so the data crosses several 1024-row
vector chunks and (for parquet) 3 row groups of 2048, with NULLs deliberately
placed at the row-group boundaries and an all-NULL stretch.

Data (numeric only — keeps CSV empty-field == NULL unambiguous for both
BumbleBee and DuckDB; the string-null reader path is covered by null_data):
  id  : 1..6000, never NULL
  grp : i % 5, but NULL when i % 50 == 0 -> a dedicated NULL group key
  val : i, but NULL when i % 10 == 0, NULL for the all-NULL stretch
        3000 <= i < 3500, and NULL at the row-group boundaries i in {2048,4096}

Run from test/e2e:
    python3 generators/sql/gen_null_large.py
then regenerate expected:
    python3 gen_sql_expected.py

Generators live under test/e2e/generators/ (sibling to files/) so pytest's
input discovery (which rglobs files/sql/input/) never picks them up.
"""
import os
import duckdb

N = 6000
ROW_GROUP_SIZE = 2048
BOUNDARIES = {2048, 4096}

HERE = os.path.dirname(os.path.abspath(__file__))
E2E = os.path.abspath(os.path.join(HERE, "..", ".."))
CSV_PATH = os.path.join(E2E, "files", "csv", "null_large.csv")
PARQUET_PATH = os.path.join(E2E, "files", "parquet", "null_large.parquet")


def build_rows():
    rows = []
    for i in range(N):
        grp = None if i % 50 == 0 else (i % 5)
        val = None if (i % 10 == 0 or 3000 <= i < 3500 or i in BOUNDARIES) else i
        rows.append((i + 1, grp, val))
    return rows


def main():
    rows = build_rows()

    with open(CSV_PATH, "w") as f:
        f.write("id,grp,val\n")
        for id_, grp, val in rows:
            f.write(f"{id_},{'' if grp is None else grp},{'' if val is None else val}\n")

    con = duckdb.connect()
    con.execute("CREATE TABLE t(id BIGINT, grp INTEGER, val BIGINT)")
    con.executemany("INSERT INTO t VALUES (?,?,?)", rows)
    if os.path.exists(PARQUET_PATH):
        os.remove(PARQUET_PATH)
    con.execute(
        f"COPY t TO '{PARQUET_PATH}' (FORMAT parquet, ROW_GROUP_SIZE {ROW_GROUP_SIZE})"
    )
    con.close()
    print(f"wrote {CSV_PATH} and {PARQUET_PATH} ({N} rows)")


if __name__ == "__main__":
    main()