"""
01 — Getting Started: CSV Analytics from Python

Query CSV files with SQL, retrieve results as pandas DataFrames.
Run from the examples/ directory:

    cd examples
    python python/01_getting_started.py
"""

import bumblebeedb as bb
import pandas as pd

# Create a BumbleBee instance
db = bb.db()

# --- Query 1: Department salary statistics ---
db.sql("""
    (SELECT DEPARTMENT_ID, COUNT(*) AS CNT, SUM(SALARY) AS TOTAL, MIN(SALARY) AS LOW, MAX(SALARY) AS HIGH
     FROM "./data/employees.csv"
     GROUP BY DEPARTMENT_ID) AS dept_stats
""")

# Convert to DataFrame and display
df = db.get_table("dept_stats", 5).to_df(
    col_names=["dept_id", "count", "total_salary", "min_salary", "max_salary"]
)
print("=== Department Salary Statistics ===")
print(df.sort_values("dept_id").to_string(index=False))
print()

# --- Query 2: High earners (salary > 85000) ---
db.sql("""
    (SELECT NAME, SALARY
     FROM "./data/employees.csv"
     WHERE SALARY > 85000) AS high_earners
""")

df2 = db.get_table("high_earners", 2).to_df(col_names=["name", "salary"])
print("=== High Earners (salary > 85,000) ===")
print(df2.sort_values("salary", ascending=False).to_string(index=False))
