"""
02 — DataFrame Analytics: Load DataFrames & Analyze

Showcase load_df + Datalog/SQL queries on in-memory DataFrames.
Run from the examples/ directory:

    cd examples
    python python/02_dataframe_analytics.py
"""

import bumblebeedb as bb
import pandas as pd

# Build a sales DataFrame in Python
sales = pd.DataFrame({
    "product":  ["Laptop", "Phone", "Tablet", "Laptop", "Phone",
                 "Tablet", "Laptop", "Phone", "Tablet", "Laptop"],
    "category": ["Electronics"] * 10,
    "revenue":  [1200, 800, 450, 1350, 750, 500, 1100, 900, 400, 1400],
    "quarter":  ["Q1", "Q1", "Q1", "Q2", "Q2", "Q2", "Q3", "Q3", "Q3", "Q4"],
})

db = bb.db()
db.load_df(sales, "sales")

# --- Datalog: total revenue per product ---
db.run("""
    rev_by_product(P, S) :- S = #sum{R;P : sales(P, _, R, _)}.
    rev_by_product(X, Y)?
""")

df1 = db.get_table("rev_by_product", 2).to_df(col_names=["product", "total_revenue"])
print("=== Revenue by Product (Datalog) ===")
print(df1.sort_values("total_revenue", ascending=False).to_string(index=False))
print()

# --- SQL: revenue per quarter ---
db.sql("""
    (SELECT V4, SUM(V3) AS TOTAL
     FROM sales
     GROUP BY V4) AS quarterly
""")

df2 = db.get_table("quarterly", 2).to_df(col_names=["quarter", "total_revenue"])
print("=== Revenue by Quarter (SQL) ===")
print(df2.sort_values("quarter").to_string(index=False))
print()

# --- SQL: top product by total revenue ---
db.sql("""
    (SELECT V1, SUM(V3) AS TOTAL
     FROM sales
     GROUP BY V1
     ORDER BY TOTAL DESC
     LIMIT 1) AS top_product
""")

df3 = db.get_table("top_product", 2).to_df(col_names=["product", "total_revenue"])
print("=== Top Product ===")
print(df3.to_string(index=False))
