"""Cross-engine result normalization for the LDBC benchmark.

The three engines emit results in different shapes:
  - DuckDB  -> CSV (written via COPY ... TO), with a header row     ("sql_csv")
  - BumbleBee -> Datalog facts on stdout, e.g. `result(143,1288...)`  ("datalog")
  - Neo4j   -> cypher-shell `--format plain` CSV, with a header row  ("cypher_plain")

Each is converted to a *canonical multiset of rows* (a sorted list of value
tuples) so that engine outputs can be compared for equality regardless of
formatting, quoting, column header presence, or row order. Numeric values are
rounded to 4 decimals to absorb float ULP differences (same policy as
test/e2e/utils.compare_csv).
"""

import csv
import re

_FACT_RE = re.compile(r'^\s*[A-Za-z_]\w*\s*\((.*)\)\s*\.?\s*$')


def _canon_value(v):
    """Normalize a single scalar: strip quotes/space, round floats to 4dp."""
    v = v.strip()
    if len(v) >= 2 and v[0] == v[-1] and v[0] in ('"', "'"):
        v = v[1:-1]
    v = v.strip()
    try:
        f = float(v)
        # keep integers as integers so 1 and 1.0 compare equal
        if f == int(f):
            return str(int(f))
        return f"{f:.4f}"
    except ValueError:
        return v


def _split_top_level(s):
    """Split a comma-separated arg list, ignoring commas inside quotes/parens."""
    out, depth, cur, quote = [], 0, [], None
    for ch in s:
        if quote:
            cur.append(ch)
            if ch == quote:
                quote = None
        elif ch in ('"', "'"):
            quote = ch
            cur.append(ch)
        elif ch in '([{':
            depth += 1
            cur.append(ch)
        elif ch in ')]}':
            depth -= 1
            cur.append(ch)
        elif ch == ',' and depth == 0:
            out.append(''.join(cur))
            cur = []
        else:
            cur.append(ch)
    if cur:
        out.append(''.join(cur))
    return out


def _rows_from_datalog(path):
    rows = []
    with open(path, 'r', errors='replace') as f:
        for line in f:
            m = _FACT_RE.match(line)
            if not m:
                continue
            args = m.group(1)
            if args.strip() == '':
                rows.append(tuple())
                continue
            rows.append(tuple(_canon_value(a) for a in _split_top_level(args)))
    return rows


def _rows_from_csv(path, has_header):
    rows = []
    with open(path, 'r', newline='', errors='replace') as f:
        reader = csv.reader(f)
        first = True
        for rec in reader:
            if not rec or (len(rec) == 1 and rec[0].strip() == ''):
                continue
            if first and has_header:
                first = False
                continue
            first = False
            rows.append(tuple(_canon_value(c) for c in rec))
    return rows


def to_canonical(path, result_format):
    """Return a sorted list of *distinct* canonical value-tuples for the output.

    Comparison is set-based: every LDBC query here returns distinct rows (DuckDB
    SELECT DISTINCT / unique keys, Cypher RETURN DISTINCT, Datalog set semantics),
    so duplicates are not meaningful. In particular BumbleBee can emit the same
    derived fact more than once (e.g. a recursive projection that fires once per
    level), which is logically the same set."""
    if result_format == 'datalog':
        rows = _rows_from_datalog(path)
    elif result_format == 'cypher_plain':
        # cypher-shell --format plain emits a CSV header line then data rows
        rows = _rows_from_csv(path, has_header=True)
    elif result_format == 'sql_csv':
        rows = _rows_from_csv(path, has_header=True)
    else:
        raise ValueError(f"unknown result_format: {result_format}")
    return sorted(set(rows))


def write_canonical_csv(path, rows):
    """Write canonical rows to a CSV file (no header) for inspection/debugging."""
    with open(path, 'w', newline='') as f:
        w = csv.writer(f)
        for r in rows:
            w.writerow(r)


def canonical_equal(actual_path, actual_format, expected_csv_path):
    """True if the actual engine output matches the DuckDB expected CSV."""
    actual = to_canonical(actual_path, actual_format)
    expected = to_canonical(expected_csv_path, 'sql_csv')
    return actual == expected
