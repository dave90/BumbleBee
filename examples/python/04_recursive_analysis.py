"""
04 — Recursive Analysis: Graph Reachability with Datalog

Compute transitive closure on a graph — something SQL can't do easily.
Run from the examples/ directory:

    cd examples
    python python/04_recursive_analysis.py
"""

import bumblebeedb as bb
import pandas as pd

# Define an org hierarchy: (manager, report)
hierarchy = pd.DataFrame({
    "manager": ["alice", "alice", "bob", "bob", "carol", "dave"],
    "report":  ["bob",   "carol", "dave", "eve", "frank", "grace"],
})

db = bb.db()
db.load_df(hierarchy, "manages")

# Recursive Datalog: compute all direct and indirect reports
db.run("""
    reports_to(M, R) :- manages(M, R).
    reports_to(M, R) :- manages(M, X), reports_to(X, R).
    reports_to(X, Y)?
""")

df = db.get_table("reports_to", 2).to_df(col_names=["manager", "report"])
print("=== All Reports (direct + indirect) ===")
print(df.sort_values(["manager", "report"]).to_string(index=False))
print()

# Who does alice manage (directly or indirectly)?
db.run("""
    alice_team(R) :- reports_to("alice", R).
    alice_team(X)?
""")

team = db.get_table("alice_team", 1).to_df(col_names=["member"])
print("=== Alice's Full Team ===")
print(team.sort_values("member").to_string(index=False))
