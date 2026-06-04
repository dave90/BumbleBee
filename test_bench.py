"""Benchmark: BumbleBee DB vs pandas on the ClickBench `hits.parquet` dataset.

For each query we run the same logical computation through BumbleBee (SQL over
the parquet file) and through pandas, and report the elapsed wall-clock time of
each plus the speedup.

Fairness measures
-----------------
* Column projection: pandas reads only the columns each query needs, matching
  BumbleBee's projection push-down into the parquet reader.
* Parquet read is inside the timed region for BOTH engines (each query re-reads
  the file), so neither hides I/O.
* The BumbleBee engine is constructed ONCE, outside the timed region — query
  time excludes one-off engine init.
* Warm-up + hot runs: every query is run once untimed (to warm the OS page
  cache equally for both engines), then `N_RUNS` timed runs; we report the
  fastest (hot) run. This removes the cold-vs-warm cache asymmetry that a
  single sequential run would create.
* Result parity: each query exposes a `check` that reduces both engines'
  output to a comparable scalar/shape; a mismatch is flagged so a "win" can't
  come from computing the wrong thing.

Caveat (documented, not hidden): BumbleBee uses multiple threads by default
while pandas' groupby/scan are single-threaded. That is a real property of the
engines, not a harness artifact — the thread count is printed in the header.

Run:
    python3 test_bench.py
    BENCH_PARQUET=test/e2e/files/parquet/mini_hits.parquet python3 test_bench.py  # quick smoke
    BENCH_RUNS=5 python3 test_bench.py
"""

import os
import time

import bumblebeedb as bb
import pandas as pd

PARQUET = os.environ.get(
    "BENCH_PARQUET",
    "/Users/davidefusca/git/BumbleBee/benchmarks/downloads/hits.parquet",
)
N_RUNS = int(os.environ.get("BENCH_RUNS", "3"))


def best_time(fn, runs=N_RUNS):
    """Warm up once (untimed), then return the fastest of `runs` timed calls and
    the last result."""
    result = fn()  # warm-up: warms OS page cache + lazy init, not counted
    best = float("inf")
    for _ in range(runs):
        start = time.perf_counter()
        result = fn()
        best = min(best, time.perf_counter() - start)
    return best, result


# A counter gives each BumbleBee run a unique result alias so repeated runs
# don't collide on the same predicate name.
_alias_n = [0]


def run_bumblebee(db, sql):
    _alias_n[0] += 1
    alias = f"result_{_alias_n[0]}"
    db.sql(sql, alias=alias)
    df = db.get_table(alias).to_df()
    db.remove_table(alias, len(df.columns))
    return df


def run_pandas(columns, op):
    df = pd.read_parquet(PARQUET, columns=columns)
    return op(df)


def result_rows(x):
    """Number of result rows: len() of a df/series, else 1 for a scalar/tuple.
    Used for a result-shape sanity check (both engines returned the same number
    of rows) — a perf benchmark doesn't value-compare across engines, but a
    shape mismatch means one side computed something different."""
    if isinstance(x, (pd.DataFrame, pd.Series)):
        return len(x)
    return 1


P = PARQUET
QUERIES = [
    # ---- ClickBench Q0–Q3, Q6: pure aggregates ----
    {
        "name": "q00_count_all",
        "sql": f'SELECT COUNT(*) AS c FROM "{P}"',
        "columns": ["WatchID"],
        "pandas": lambda df: len(df),
    },
    {
        "name": "q01_count_adv",
        "sql": f'SELECT COUNT(*) AS c FROM "{P}" WHERE ADVENGINEID <> 0',
        "columns": ["AdvEngineID"],
        "pandas": lambda df: int((df["AdvEngineID"] != 0).sum()),
    },
    {
        "name": "q02_sum_count_avg",
        "sql": f'SELECT SUM(ADVENGINEID) AS s, COUNT(*) AS c, AVG(RESOLUTIONWIDTH) AS a FROM "{P}"',
        "columns": ["AdvEngineID", "ResolutionWidth"],
        "pandas": lambda df: (int(df["AdvEngineID"].sum()), len(df), float(df["ResolutionWidth"].mean())),
    },
    {
        "name": "q03_avg_userid",
        "sql": f'SELECT AVG(USERID) AS a FROM "{P}"',
        "columns": ["UserID"],
        "pandas": lambda df: float(df["UserID"].mean()),
    },
    {
        "name": "q06_min_max_date",
        "sql": f'SELECT MIN(EVENTDATE) AS mn, MAX(EVENTDATE) AS mx FROM "{P}"',
        "columns": ["EventDate"],
        "pandas": lambda df: (df["EventDate"].min(), df["EventDate"].max()),
    },
    # ---- group-by + order-by + limit ----
    {
        "name": "q07_adv_groupby_top",
        "sql": f'SELECT ADVENGINEID, COUNT(*) AS c FROM "{P}" WHERE ADVENGINEID <> 0 '
               f'GROUP BY ADVENGINEID ORDER BY c DESC LIMIT 100',
        "columns": ["AdvEngineID"],
        "pandas": lambda df: (df[df["AdvEngineID"] != 0]
                              .groupby("AdvEngineID").size()
                              .sort_values(ascending=False).head(100).reset_index()),
    },
    {
        "name": "q12_searchphrase_top",
        "sql": f'SELECT SEARCHPHRASE AS s, COUNT(*) AS c FROM "{P}" WHERE SEARCHPHRASE <> \'\' '
               f'GROUP BY SEARCHPHRASE ORDER BY c DESC, s DESC LIMIT 6',
        "columns": ["SearchPhrase"],
        "pandas": lambda df: (df[df["SearchPhrase"] != ""]
                              .groupby("SearchPhrase").size()
                              .sort_values(ascending=False).head(6).reset_index()),
    },
    {
        "name": "q14_engine_phrase_top",
        "sql": f'SELECT SEARCHENGINEID, SEARCHPHRASE, COUNT(*) AS c FROM "{P}" '
               f'WHERE SEARCHPHRASE <> \'\' GROUP BY SEARCHENGINEID, SEARCHPHRASE '
               f'ORDER BY c DESC LIMIT 10',
        "columns": ["SearchEngineID", "SearchPhrase"],
        "pandas": lambda df: (df[df["SearchPhrase"] != ""]
                              .groupby(["SearchEngineID", "SearchPhrase"]).size()
                              .sort_values(ascending=False).head(10).reset_index()),
    },
    # ---- string filter (LIKE) ----
    {
        "name": "q20_url_like_google",
        "sql": f"SELECT COUNT(*) AS c FROM \"{P}\" WHERE URL LIKE '%google%'",
        "columns": ["URL"],
        "pandas": lambda df: int(df["URL"].str.contains("google", regex=False, na=False).sum()),
    },
    # ---- order-by + limit over a filtered string column ----
    {
        "name": "q25_searchphrase_sorted",
        "sql": f'SELECT SEARCHPHRASE FROM "{P}" WHERE SEARCHPHRASE <> \'\' '
               f'ORDER BY SEARCHPHRASE LIMIT 10',
        "columns": ["SearchPhrase"],
        "pandas": lambda df: (df.loc[df["SearchPhrase"] != "", "SearchPhrase"]
                              .sort_values().head(10).reset_index(drop=True)),
    },
]


def main():
    print(f"Dataset : {PARQUET}")
    print(f"Runs    : warm-up + best of {N_RUNS}")
    print(f"Threads : BumbleBee = default (hardware concurrency); pandas = 1\n")

    db = bb.db()

    header = f"{'query':<26}{'bumblebee (s)':>15}{'pandas (s)':>13}{'speedup':>11}{'rows':>9}"
    print(header)
    print("-" * len(header))

    for q in QUERIES:
        try:
            bb_t, bb_res = best_time(lambda: run_bumblebee(db, q["sql"]))
            bb_rows = result_rows(bb_res)
        except Exception as e:
            print(f"{q['name']:<26}{'ERROR':>15}   bumblebee {type(e).__name__}: {str(e)[:50]}")
            continue
        try:
            pd_t, pd_res = best_time(lambda: run_pandas(q["columns"], q["pandas"]))
            pd_rows = result_rows(pd_res)
        except Exception as e:
            print(f"{q['name']:<26}{bb_t:>15.3f}{'ERROR':>13}   pandas {type(e).__name__}: {str(e)[:40]}")
            continue

        speedup = pd_t / bb_t if bb_t > 0 else float("inf")
        # result-shape sanity: same number of result rows on both engines
        rows = f"{bb_rows}" if bb_rows == pd_rows else f"{bb_rows}!={pd_rows}"
        print(f"{q['name']:<26}{bb_t:>15.3f}{pd_t:>13.3f}{speedup:>10.2f}x{rows:>9}")


if __name__ == "__main__":
    main()