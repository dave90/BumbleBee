#!/usr/bin/env python3
"""
Local head-to-head benchmark: BumbleBee vs DuckDB on identical synthetic data.

Runs each query file with both engines on this machine (default 4 threads),
reports per-query timings, the gap vs DuckDB, and whether outputs match.
This is the apples-to-apples measurement used to drive optimization, since the
recorded 50-core/real-data CSVs cannot be reproduced here.

Usage:
  bench_local.py [--bb ../cmake-build-release/BumbleBee] [--threads 4]
                 [--tries 2] [--queries q33,q32,...] [--csv]
                 [--baseline file.json] [--out file.json]
"""
import argparse, json, os, re, subprocess, time, sys
from pathlib import Path
from statistics import median

import duckdb
sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
from test.e2e.utils import compare_csv

HERE = Path(__file__).resolve().parent
INPUT = HERE / "files" / "sql" / "input"
DUCK_INPUT = HERE / "files" / "sql" / "duckdb_input"

# Parquet ClickBench queries (the q* set the task targets) + a couple CSV ones.
PARQUET_Q = ["q00","q01","q02","q03","q06","q07","q12","q14","q19","q20","q21",
             "q25","q26","q30","q32","q33","q36"]
CSV_Q = ["q00","q01","q02","q03","q06","q14","q19","q30"]

def read_sql(path):
    lines = path.read_text().splitlines()
    if lines and lines[0].strip().startswith("%@sql"):
        lines = lines[1:]
    return "\n".join(lines).strip()

def run_bb(bb, infile, threads, out_path):
    # Export query result to CSV so we can compare; mirror benchmark_runner.
    sql = infile.read_text()
    export = f'COPY (\n{read_sql(infile)}\n) TO "{out_path}" (single_file=1)'
    tmp = out_path.with_suffix(".bbinput.sql")
    if sql.splitlines()[0].strip().startswith("%@sql"):
        tmp.write_text("%@sql\n" + export)
    else:
        tmp.write_text(export)
    t0 = time.perf_counter()
    r = subprocess.run(f'{bb} -t {threads} -i {tmp}', shell=True,
                       capture_output=True, text=True, timeout=600)
    dt = time.perf_counter() - t0
    tmp.unlink(missing_ok=True)
    if r.returncode != 0:
        return dt, None, (r.stderr or r.stdout)[-500:]
    return dt, out_path, None

def run_duck(con, infile, threads, out_path):
    con.execute(f"PRAGMA threads={threads}")
    q = read_sql(infile)
    t0 = time.perf_counter()
    con.execute(f'COPY ({q}) TO \'{out_path}\' (HEADER, DELIMITER \',\')')
    dt = time.perf_counter() - t0
    return dt, out_path

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--bb", default=str(HERE.parent / "cmake-build-release" / "BumbleBee"))
    ap.add_argument("--threads", type=int, default=4)
    ap.add_argument("--tries", type=int, default=2)
    ap.add_argument("--queries", default="")
    ap.add_argument("--csv", action="store_true", help="run CSV variants instead of parquet")
    ap.add_argument("--baseline", default="")
    ap.add_argument("--out", default="")
    args = ap.parse_args()

    suffix = ".sql" if args.csv else ".parquet.sql"
    qlist = (args.queries.split(",") if args.queries
             else (CSV_Q if args.csv else PARQUET_Q))

    outdir = HERE / "files" / "sql" / "_local"
    outdir.mkdir(parents=True, exist_ok=True)

    baseline = {}
    if args.baseline and Path(args.baseline).exists():
        baseline = json.load(open(args.baseline))

    con = duckdb.connect()
    results = {}
    print(f"{'query':<14}{'BB(s)':>9}{'Duck(s)':>9}{'gap%':>8}{'vsBase':>9}  match")
    print("-"*64)
    for q in qlist:
        infile = INPUT / f"{q}{suffix}"
        duckfile = DUCK_INPUT / f"{q}{suffix}"
        if not infile.exists() or not duckfile.exists():
            continue
        # DuckDB (uses the duckdb_input variant: '' literals, real column case)
        dts_d = []
        dpath = outdir / f"{q}.duck.csv"
        for _ in range(args.tries):
            dt, _ = run_duck(con, duckfile, args.threads, dpath)
            dts_d.append(dt)
        # BumbleBee
        dts_b = []
        bpath = outdir / f"{q}.bb.csv"
        err = None
        for _ in range(args.tries):
            dt, op, err = run_bb(args.bb, infile, args.threads, bpath)
            dts_b.append(dt)
            if err: break
        bb = median(dts_b)
        dk = median(dts_d)
        try:
            match = "ERR" if err else ("match" if compare_csv(str(bpath), str(dpath)) else "MISMATCH")
        except Exception as e:
            match = f"CMP?({e})"[:20]
        gap = (bb - dk) / dk * 100 if dk > 0 else 0
        base = baseline.get(q)
        vsbase = f"{(bb-base)/base*100:+.0f}%" if base else "-"
        results[q] = bb
        flag = "" if err is None else "  <<<"+ (err.replace(chr(10)," ")[:120])
        print(f"{q:<14}{bb:>9.3f}{dk:>9.3f}{gap:>7.0f}%{vsbase:>9}  {match}{flag}")
    print("-"*64)
    tot_bb = sum(results.values())
    print(f"{'TOTAL BB':<14}{tot_bb:>9.3f}")
    if args.out:
        json.dump(results, open(args.out, "w"), indent=2)
        print(f"saved -> {args.out}")

if __name__ == "__main__":
    main()
