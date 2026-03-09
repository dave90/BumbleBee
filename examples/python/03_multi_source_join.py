"""
03 — Multi-Source Join: CSV Files + DataFrames

Combine file-based data with in-memory DataFrames in a single query.
Run from the examples/ directory:

    cd examples
    python python/03_multi_source_join.py
"""

import bumblebeedb as bb
import pandas as pd

db = bb.db()

# Load employees from CSV via SQL
db.sql("""
    (SELECT ID, NAME, DEPARTMENT_ID, SALARY
     FROM "./data/employees.csv") AS emp
""")

# Create a bonus schedule DataFrame: department_id -> bonus percentage
bonus = pd.DataFrame({
    "dept_id": [1, 2, 3, 4, 5],
    "bonus_pct": [15, 10, 12, 8, 13],
})
db.load_df(bonus, "bonus")

# Join with Datalog: compute bonus amount per employee
db.run("""
    compensation(Name, Salary, BonusPct, BonusAmt) :-
        emp(_, Name, DeptId, Salary),
        bonus(DeptId, BonusPct),
        BonusAmt = Salary * BonusPct / 100.
    compensation(X, Y, Z, W)?
""")

df = db.get_table("compensation", 4).to_df(
    col_names=["name", "salary", "bonus_pct", "bonus_amount"]
)
print("=== Employee Compensation (CSV + DataFrame join) ===")
print(df.sort_values("bonus_amount", ascending=False).to_string(index=False))
