#!/usr/bin/env python3
"""
Generates agg.null.large — Phase 6 e2e large multi-batch + multi-thread test.

Shape:
  - 5 "normal" groups, 1200 rows each (each group spans > STANDARD_VECTOR_SIZE = 1024).
  - Sparse NULL pattern: every 7th row of each normal group has NULL in col 1.
  - 1 all-NULL group (99) with 1100 rows.
  - One query computes SUM/AVG/MIN/MAX/COUNT per group in a single multi-aggregate atom.

Run from the project root:
    python3 test/e2e/generators/asp/null/gen_agg_null_large.py \\
        > test/e2e/files/asp/input/null/agg.null.large
Then regenerate the expected file:
    ./cmake-build-debug/BumbleBee -t 1 -d \\
        -i test/e2e/files/asp/input/null/agg.null.large \\
        > test/e2e/files/asp/expected/agg.null.large

NOTE: generators live in `test/e2e/generators/` (sibling to `files/`) so the
e2e test discovery (which rglobs `files/asp/input/`) does not pick them up.
"""
import sys


def main() -> None:
    out: list[str] = []

    # 5 normal groups, 1200 rows each. value = group_id * 100000 + row_index so each
    # value is unique and easy to verify by hand if the test ever fails.
    for g in (1, 2, 3, 4, 5):
        for i in range(1200):
            if i % 7 == 0:
                out.append(f"input({g},NULL).")
            else:
                v = g * 100000 + i
                out.append(f"input({g},{v}).")

    # All-NULL group: 1100 rows, every row's col 1 is NULL.
    for _ in range(1100):
        out.append("input(99,NULL).")

    # Multi-aggregate query: SUM/AVG/MIN/MAX/COUNT in one atom over each group.
    out.append(
        "g_all(G, S, A, MN, MX, C) :- input(G, _),"
        " S  = #sum  {V : input(G, V)},"
        " A  = #avg  {V : input(G, V)},"
        " MN = #min  {V : input(G, V)},"
        " MX = #max  {V : input(G, V)},"
        " C  = #count{V : input(G, V)}."
    )
    out.append("g_all(G, S, A, MN, MX, C)?")

    sys.stdout.write("\n".join(out) + "\n")


if __name__ == "__main__":
    main()