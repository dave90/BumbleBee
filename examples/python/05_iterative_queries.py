"""
05 — Iterative Queries: Interactive OLAP Session

Multiple queries on the same db, inspecting state, and redefining predicates.
Run from the examples/ directory:

    cd examples
    python python/05_iterative_queries.py
"""

import bumblebeedb as bb
import pandas as pd

db = bb.db()

# Step 1: Load product and sales data from CSV
db.sql("""
    (SELECT PRODUCT_ID, PRODUCT_NAME, CATEGORY, PRICE
     FROM "./data/products.csv") AS products
""")
db.sql("""
    (SELECT ORDER_ID, PRODUCT_ID, CUSTOMER_NAME, QUANTITY, UNIT_PRICE
     FROM "./data/sales.csv") AS sales
""")

# Step 2: Inspect loaded predicates
print("=== Loaded Predicates ===")
for name, arity in sorted(db.get_output_predicates()):
    n = len(db.get_table(name, arity))
    print(f"  {name}/{arity}  ({n} rows)")
print()

# Step 3: Revenue by category via SQL
db.sql("""
    (SELECT V4, SUM(V4 * V5) AS REV
     FROM sales
     GROUP BY V4) AS rev_by_customer
""")

df1 = db.get_table("rev_by_customer", 2).to_df(col_names=["customer", "revenue"])
print("=== Revenue by Customer ===")
print(df1.sort_values("revenue", ascending=False).to_string(index=False))
print()

# Step 4: Add Datalog facts and combine with SQL results
db.run("""
    vip("Alice").
    vip("Dan").
    vip("Eve").
    vip(X)?
""")

# Combine: find VIP customers and their total spend
db.run("""
    line_total(C, T) :- sales(_, _, C, Q, P), T = Q * P.
    vip_spend(C, S) :- vip(C), S = #sum{T : line_total(C, T)}.
    vip_spend(X, Y)?
""")

df2 = db.get_table("vip_spend", 2).to_df(col_names=["customer", "total_spend"])
print("=== VIP Customer Spend ===")
print(df2.sort_values("total_spend", ascending=False).to_string(index=False))
print()

# Step 5: Remove a predicate and redefine
db.remove_table("rev_by_customer", 2)
print("After removing rev_by_customer:")
for name, arity in sorted(db.get_output_predicates()):
    print(f"  {name}/{arity}")
