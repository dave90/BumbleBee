"""
06 — NULL Handling: missing values round-trip through Python

Shows how NULL (missing data) flows between pandas and BumbleBee:
  - load_df: pandas None / pd.NA / NaN become NULL.
  - Aggregates skip NULL inputs (#sum, #count, ...).
  - IS NULL / IS NOT NULL select rows by missingness.
  - to_df: NULLs come back as pd.NA (nullable dtype); tuples(): as None.

Run from the examples/ directory:

    cd examples
    python python/06_null_handling.py
"""

import bumbledl as bb
import pandas as pd

# A survey where some respondents did not give a score (missing values).
# Use pandas' nullable Int64 dtype so None is a true NA, not a float NaN.
survey = pd.DataFrame({
    "name":  ["Alice", "Bob", "Carol", "Dan", "Eve"],
    "score": pd.array([90, None, 75, None, 88], dtype="Int64"),
})

db = bb.db()
db.load_df(survey, "scores")

# --- Aggregates skip NULLs ---
db.run("""
    total(S)    :- S = #sum{V : scores(_, V)}.
    recorded(C) :- C = #count{V : scores(_, V)}.
    total(X)?
    recorded(X)?
""")

total = db.get_table("total", 1).tuples()[0][0]
recorded = db.get_table("recorded", 1).tuples()[0][0]
print("=== Aggregates skip NULL ===")
print(f"sum of recorded scores : {total}")   # 90 + 75 + 88 = 253
print(f"number recorded        : {recorded}/{len(survey)} rows")  # 3 of 5
print()

# --- IS NULL / IS NOT NULL ---
db.run("""
    missing(N) :- scores(N, V), V IS NULL.
    answered(N, V) :- scores(N, V), V IS NOT NULL.
    missing(X)?
    answered(X, Y)?
""")

missing = sorted(r[0] for r in db.get_table("missing", 1).tuples())
print("=== IS NULL / IS NOT NULL ===")
print(f"missing a score : {missing}")  # ['Bob', 'Dan']
print()

# --- NULLs round-trip back to pandas ---
df_out = db.get_table("scores", 2).to_df(col_names=["name", "score"])
df_out = df_out.sort_values("name").reset_index(drop=True)
print("=== to_df() — NULLs become pd.NA (nullable dtype) ===")
print(f"score dtype: {df_out['score'].dtype}")  # Int64 (nullable)
print(df_out.to_string(index=False))
print()

# tuples() surfaces NULL positions as Python None
rows = sorted(db.get_table("scores", 2).tuples(), key=lambda r: r[0])
print("=== tuples() — NULL is None ===")
print(rows)  # [('Alice', 90), ('Bob', None), ('Carol', 75), ('Dan', None), ('Eve', 88)]
