"""Benchmark: BumbleBee DB vs pandas on the ClickBench `hits.parquet` dataset.

For each query we run the same logical computation through BumbleBee (SQL over
the parquet file) and through pandas, reporting elapsed wall-clock time and the
speedup.

Fairness measures
-----------------
* Column projection: pandas reads only the columns each query needs, matching
  BumbleBee's projection push-down into the parquet reader.
* Parquet read is inside the timed region for BOTH engines.
* Warm-up + hot runs: each query runs once untimed (to warm the OS page cache
  equally for both engines), then `N_RUNS` timed runs; the fastest is reported.
* Result-shape sanity: result row counts must match across engines (a mismatch
  is flagged), so a "win" can't come from computing the wrong thing.

Robustness
----------
Each engine's work for each query runs in its OWN subprocess with a per-query
timeout (`BENCH_TIMEOUT`, default 300 s). A query that hangs, exceeds the
timeout, or is OOM-killed by the OS is reported as `timeout` / `killed` and the
benchmark CONTINUES — one heavy query (e.g. pandas materializing the full 100M
-row URL string column) can no longer take down the whole run.

Caveat (documented): BumbleBee runs multi-threaded by default, pandas is
single-threaded. That's a real engine property, shown in the header.

Run:
    python3 test_bench.py
    BENCH_PARQUET=test/e2e/files/parquet/mini_hits.parquet python3 test_bench.py  # quick smoke
    BENCH_RUNS=3 BENCH_TIMEOUT=600 python3 test_bench.py
"""

import os
import time
import multiprocessing as mp

import pandas as pd

PARQUET = os.environ.get(
    "BENCH_PARQUET",
    "/Users/davidefusca/git/BumbleBee/benchmarks/downloads/hits.parquet",
)
N_RUNS = int(os.environ.get("BENCH_RUNS", "1"))
TIMEOUT = float(os.environ.get("BENCH_TIMEOUT", "300"))  # seconds per engine per query


def _result_rows(x):
    """Result-shape signal: len() of a df/series, else 1 for a scalar/tuple."""
    if isinstance(x, (pd.DataFrame, pd.Series)):
        return len(x)
    return 1


P = PARQUET
QUERIES = [
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
    {
        "name": "q20_url_like_google",
        "sql": f"SELECT COUNT(*) AS c FROM \"{P}\" WHERE URL LIKE '%google%'",
        "columns": ["URL"],
        "pandas": lambda df: int(df["URL"].str.contains("google", regex=False, na=False).sum()),
    },
    {
        "name": "q25_searchphrase_sorted",
        "sql": f'SELECT SEARCHPHRASE FROM "{P}" WHERE SEARCHPHRASE <> \'\' '
               f'ORDER BY SEARCHPHRASE LIMIT 10',
        "columns": ["SearchPhrase"],
        "pandas": lambda df: (df.loc[df["SearchPhrase"] != "", "SearchPhrase"]
                              .sort_values().head(10).reset_index(drop=True)),
    },
]
QMAP = {q["name"]: q for q in QUERIES}


def _best_time(call):
    call()  # warm-up (untimed): warms OS page cache + lazy init
    best = float("inf")
    res = None
    for _ in range(N_RUNS):
        start = time.perf_counter()
        res = call()
        best = min(best, time.perf_counter() - start)
    return best, res


def _engine_child(kind, qname, queue):
    """Runs in a subprocess. Puts (elapsed_seconds, result_rows) on the queue."""
    q = QMAP[qname]
    if kind == "bb":
        import bumblebeedb as bb
        db = bb.db()

        def call():
            db.sql(q["sql"], alias="result")
            df = db.get_table("result").to_df()
            db.remove_table("result", len(df.columns))
            return df
    else:  # pandas
        def call():
            df = pd.read_parquet(PARQUET, columns=q["columns"])
            return q["pandas"](df)

    elapsed, res = _best_time(call)
    queue.put((elapsed, _result_rows(res)))


def run_isolated(kind, qname):
    """Run one engine for one query in its own process, bounded by TIMEOUT.
    Returns (elapsed, rows) or (None, reason)."""
    ctx = mp.get_context("spawn")
    queue = ctx.Queue()
    p = ctx.Process(target=_engine_child, args=(kind, qname, queue))
    p.start()
    p.join(TIMEOUT)
    if p.is_alive():
        p.terminate()
        p.join()
        return None, f"timeout(>{int(TIMEOUT)}s)"
    if p.exitcode != 0:
        # negative exitcode => killed by signal (e.g. -9 = OOM kill)
        return None, f"killed(exit={p.exitcode})"
    try:
        return queue.get_nowait()
    except Exception:
        return None, "no-result"


def main():
    print(f"Dataset : {PARQUET}")
    print(f"Runs    : warm-up + best of {N_RUNS}   (per-query timeout {int(TIMEOUT)}s)")
    print(f"Threads : BumbleBee = default (hardware concurrency); pandas = 1\n")

    header = f"{'query':<26}{'bumblebee (s)':>15}{'pandas (s)':>13}{'speedup':>11}{'rows':>11}"
    print(header)
    print("-" * len(header))

    for q in QUERIES:
        name = q["name"]
        bb_t, bb_info = run_isolated("bb", name)
        pd_t, pd_info = run_isolated("pd", name)

        bb_cell = f"{bb_t:.3f}" if bb_t is not None else bb_info
        pd_cell = f"{pd_t:.3f}" if pd_t is not None else pd_info

        if bb_t is not None and pd_t is not None and bb_t > 0:
            speed = f"{pd_t / bb_t:.2f}x"
            rows = f"{bb_info}" if bb_info == pd_info else f"{bb_info}!={pd_info}"
        else:
            speed = "-"
            rows = "-"
        print(f"{name:<26}{bb_cell:>15}{pd_cell:>13}{speed:>11}{rows:>11}")


if __name__ == "__main__":
    main()