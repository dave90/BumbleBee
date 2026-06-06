#!/usr/bin/env python3
"""
Generates agg.null.all_null_large — Phase 6 e2e test exercising the case where
a group has *every* row NULL across multiple STANDARD_VECTOR_SIZE batches,
while another group has typed values so the column's type can be inferred.

Shape:
  - Group 1: 2000 rows with non-NULL values (spans >1 batch). Verifies
    the SUM/AVG/MIN/MAX/COUNT update path with a large non-null group.
  - Group 99: 1500 rows all NULL in col 1 (also spans >1 batch). Verifies
    the empty-state propagation through combine() and finalize-to-NULL
    even when the empty state is updated thousands of times with NULL inputs.

Run from the project root:
    python3 test/e2e/generators/asp/null/gen_agg_null_all_null_large.py \\
        > test/e2e/files/asp/input/null/agg.null.all_null_large
Then regenerate the expected:
    ./cmake-build-debug/BumbleBee -t 1 -d \\
        -i test/e2e/files/asp/input/null/agg.null.all_null_large \\
        > test/e2e/files/asp/expected/agg.null.all_null_large

NOTE: generators live in `test/e2e/generators/` (sibling to `files/`) so the
e2e test discovery (which rglobs `files/asp/input/`) does not pick them up.
"""
import sys


def main() -> None:
    out: list[str] = []
    # Group 1: 2000 non-NULL rows. value = 100000 + i for unique values.
    for i in range(2000):
        out.append(f"data(1,{100000 + i}).")
    # Group 99: 1500 all-NULL rows.
    for i in range(1500):
        out.append(f"data(99,NULL).")

    out.append(
        "g_all(G, S, A, MN, MX, C) :- data(G, _),"
        " S  = #sum  {V : data(G, V)},"
        " A  = #avg  {V : data(G, V)},"
        " MN = #min  {V : data(G, V)},"
        " MX = #max  {V : data(G, V)},"
        " C  = #count{V : data(G, V)}."
    )
    out.append("g_all(G, S, A, MN, MX, C)?")
    sys.stdout.write("\n".join(out) + "\n")


if __name__ == "__main__":
    main()