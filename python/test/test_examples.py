"""
Tests mirroring the examples/python/ scripts.

Each test class corresponds to one example script and validates
the same queries produce expected results.
"""

import pytest
import pandas as pd
from pathlib import Path

import bumblebeedb as bb
from conftest import _rows, _sql

EXAMPLES_DATA = Path(__file__).resolve().parents[2] / "examples" / "data"


class TestGettingStarted:
    """Tests for 01_getting_started.py — CSV analytics."""

    def test_dept_stats(self):
        csv = str(EXAMPLES_DATA / "employees.csv")
        db = bb.db()
        db.sql(f"""
            (SELECT DEPARTMENT_ID, COUNT(*) AS CNT, SUM(SALARY) AS TOTAL,
                    MIN(SALARY) AS LOW, MAX(SALARY) AS HIGH
             FROM "{csv}"
             GROUP BY DEPARTMENT_ID) AS dept_stats
        """)
        rows = sorted(_rows(db, "dept_stats", 5))
        assert len(rows) == 5
        # Dept 1: Alice(95k), Bob(88k), Dan(105k), Jack(92k), Olivia(110k) -> 5, 490000, 88000, 110000
        assert rows[0] == (1, 5, 490000, 88000, 110000)

    def test_high_earners(self):
        csv = str(EXAMPLES_DATA / "employees.csv")
        db = bb.db()
        db.sql(f"""
            (SELECT NAME, SALARY
             FROM "{csv}"
             WHERE SALARY > 85000) AS high_earners
        """)
        rows = sorted(_rows(db, "high_earners", 2))
        names = [r[0] for r in rows]
        assert "Alice Johnson" in names
        assert "Olivia Clark" in names
        assert len(rows) == 6  # 95k, 88k, 105k, 85k, 92k, 91k, 110k -> 7 above 85k... let me check
        # >85000: Alice(95k), Bob(88k), Dan(105k), Ivy(85k NO), Jack(92k), Nick(91k), Olivia(110k) = 6
        assert all(r[1] > 85000 for r in rows)


class TestDataFrameAnalytics:
    """Tests for 02_dataframe_analytics.py — load_df + queries."""

    def _make_db(self):
        sales = pd.DataFrame({
            "product":  ["Laptop", "Phone", "Tablet", "Laptop", "Phone",
                         "Tablet", "Laptop", "Phone", "Tablet", "Laptop"],
            "category": ["Electronics"] * 10,
            "revenue":  [1200, 800, 450, 1350, 750, 500, 1100, 900, 400, 1400],
            "quarter":  ["Q1", "Q1", "Q1", "Q2", "Q2", "Q2", "Q3", "Q3", "Q3", "Q4"],
        })
        db = bb.db()
        db.load_df(sales, "sales")
        return db

    def test_revenue_by_product(self):
        db = self._make_db()
        db.run("""
            rev_by_product(P, S) :- S = #sum{R;P : sales(P, _, R, _)}.
            rev_by_product(X, Y)?
        """)
        rows = sorted(_rows(db, "rev_by_product", 2))
        # Laptop: 1200+1350+1100+1400=5050, Phone: 800+750+900=2450, Tablet: 450+500+400=1350
        assert rows == [("Laptop", 5050), ("Phone", 2450), ("Tablet", 1350)]

    def test_revenue_by_quarter(self):
        db = self._make_db()
        db.sql("""
            (SELECT COL_3, SUM(COL_2) AS TOTAL
             FROM sales
             GROUP BY COL_3) AS quarterly
        """)
        rows = sorted(_rows(db, "quarterly", 2))
        # Q1: 1200+800+450=2450, Q2: 1350+750+500=2600, Q3: 1100+900+400=2400, Q4: 1400
        assert rows == [("Q1", 2450), ("Q2", 2600), ("Q3", 2400), ("Q4", 1400)]

    def test_top_product(self):
        db = self._make_db()
        db.sql("""
            (SELECT COL_0, SUM(COL_2) AS TOTAL
             FROM sales
             GROUP BY COL_0
             ORDER BY TOTAL DESC
             LIMIT 1) AS top_product
        """)
        rows = _rows(db, "top_product", 2)
        assert rows == [("Laptop", 5050)]


class TestMultiSourceJoin:
    """Tests for 03_multi_source_join.py — CSV + DataFrame join."""

    def test_compensation(self):
        csv = str(EXAMPLES_DATA / "employees.csv")
        db = bb.db()
        db.sql(f"""
            (SELECT ID, NAME, DEPARTMENT_ID, SALARY
             FROM "{csv}") AS emp
        """)
        bonus = pd.DataFrame({
            "dept_id": [1, 2, 3, 4, 5],
            "bonus_pct": [15, 10, 12, 8, 13],
        })
        db.load_df(bonus, "bonus")
        db.run("""
            compensation(Name, Salary, BonusPct, BonusAmt) :-
                emp(_, Name, DeptId, Salary),
                bonus(DeptId, BonusPct),
                BonusAmt = Salary * BonusPct / 100.
            compensation(X, Y, Z, W)?
        """)
        rows = sorted(_rows(db, "compensation", 4))
        assert len(rows) == 15  # all 15 employees
        # Check Alice Johnson: dept 1 -> 15% bonus on 95000 = 14250
        alice = [r for r in rows if r[0] == "Alice Johnson"][0]
        assert alice == ("Alice Johnson", 95000, 15, 14250)


class TestRecursiveAnalysis:
    """Tests for 04_recursive_analysis.py — graph reachability."""

    def _make_db(self):
        hierarchy = pd.DataFrame({
            "manager": ["alice", "alice", "bob", "bob", "carol", "dave"],
            "report":  ["bob",   "carol", "dave", "eve", "frank", "grace"],
        })
        db = bb.db()
        db.load_df(hierarchy, "manages")
        return db

    def test_transitive_closure(self):
        db = self._make_db()
        db.run("""
            reports_to(M, R) :- manages(M, R).
            reports_to(M, R) :- manages(M, X), reports_to(X, R).
            reports_to(X, Y)?
        """)
        rows = sorted(_rows(db, "reports_to", 2))
        # Direct: alice->bob, alice->carol, bob->dave, bob->eve, carol->frank, dave->grace
        # Indirect: alice->dave (via bob), alice->eve (via bob), alice->frank (via carol),
        #           alice->grace (via bob->dave), bob->grace (via dave)
        assert ("alice", "bob") in rows
        assert ("alice", "grace") in rows  # alice -> bob -> dave -> grace
        assert ("bob", "grace") in rows    # bob -> dave -> grace
        assert len(rows) == 11

    def test_alice_team(self):
        db = self._make_db()
        db.run("""
            reports_to(M, R) :- manages(M, R).
            reports_to(M, R) :- manages(M, X), reports_to(X, R).
            alice_team(R) :- reports_to("alice", R).
            alice_team(X)?
        """)
        rows = sorted(_rows(db, "alice_team", 1))
        # alice manages everyone directly or indirectly
        assert rows == [("bob",), ("carol",), ("dave",), ("eve",), ("frank",), ("grace",)]


class TestIterativeQueries:
    """Tests for 05_iterative_queries.py — interactive session."""

    def test_load_and_inspect(self):
        csv_products = str(EXAMPLES_DATA / "products.csv")
        csv_sales = str(EXAMPLES_DATA / "sales.csv")
        db = bb.db()
        db.sql(f"""
            (SELECT PRODUCT_ID, PRODUCT_NAME, CATEGORY, PRICE
             FROM "{csv_products}") AS products
        """)
        db.sql(f"""
            (SELECT ORDER_ID, PRODUCT_ID, CUSTOMER_NAME, QUANTITY, UNIT_PRICE
             FROM "{csv_sales}") AS sales
        """)
        preds = dict(db.get_output_predicates())
        assert "products" in preds
        assert "sales" in preds
        assert len(db.get_table("products", 4)) == 10
        assert len(db.get_table("sales", 5)) == 20

    def test_vip_spend(self):
        csv_sales = str(EXAMPLES_DATA / "sales.csv")
        db = bb.db()
        db.sql(f"""
            (SELECT ORDER_ID, PRODUCT_ID, CUSTOMER_NAME, QUANTITY, UNIT_PRICE
             FROM "{csv_sales}") AS sales
        """)
        db.run("""
            vip("Alice").
            vip("Dan").
            vip("Eve").
            vip(X)?
        """)
        db.run("""
            line_total(C, T) :- sales(_, _, C, Q, P), T = Q * P.
            vip_spend(C, S) :- vip(C), S = #sum{T : line_total(C, T)}.
            vip_spend(X, Y)?
        """)
        rows = sorted(_rows(db, "vip_spend", 2))
        assert len(rows) == 3
        names = [r[0] for r in rows]
        assert "Alice" in names
        assert "Dan" in names
        assert "Eve" in names
        # All should have positive spend
        assert all(r[1] > 0 for r in rows)

    def test_remove_table(self):
        db = bb.db()
        db.run("temp(1, 2). temp(3, 4). temp(X, Y)?")
        assert len(db.get_table("temp", 2)) == 2
        db.remove_table("temp", 2)
        preds = dict(db.get_output_predicates())
        assert "temp" not in preds


class TestPredicateTableColumns:
    """Tests that SQL queries on predicate tables use COL_0, COL_1, ... naming,
    consistent with to_df() default column names."""

    def test_sql_where_with_default_columns(self):
        """COL_0, COL_1 should work in SQL WHERE on predicate tables."""
        hierarchy = pd.DataFrame({
            "manager": ["alice", "alice", "bob"],
            "report":  ["bob",   "carol", "dave"],
        })
        db = bb.db()
        db.load_df(hierarchy, "manages")
        db.sql("""
            (SELECT *
             FROM manages
             WHERE COL_0 = 'alice') AS alice_manages
        """)
        rows = sorted(_rows(db, "alice_manages", 2))
        assert rows == [("alice", "bob"), ("alice", "carol")]

    def test_sql_select_with_default_columns(self):
        """COL_0, COL_1 should work in SQL SELECT on predicate tables."""
        data = pd.DataFrame({
            "name": ["alice", "bob"],
            "score": [90, 85],
        })
        db = bb.db()
        db.load_df(data, "scores")
        db.sql("""
            (SELECT COL_0, COL_1
             FROM scores
             WHERE COL_1 > 87) AS high_scores
        """)
        rows = _rows(db, "high_scores", 2)
        assert rows == [("alice", 90)]

    def test_to_df_columns_match_sql_columns(self):
        """to_df() default column names should match SQL column names."""
        data = pd.DataFrame({
            "a": [1, 2, 3],
            "b": [4, 5, 6],
        })
        db = bb.db()
        db.load_df(data, "tbl")
        df = db.get_table("tbl", 2).to_df()
        assert list(df.columns) == ["COL_0", "COL_1"]

    def test_sql_group_by_with_default_columns(self):
        """COL_N columns should work in GROUP BY on predicate tables."""
        data = pd.DataFrame({
            "category": ["A", "A", "B", "B"],
            "value": [10, 20, 30, 40],
        })
        db = bb.db()
        db.load_df(data, "items")
        db.sql("""
            (SELECT COL_0, SUM(COL_1) AS TOTAL
             FROM items
             GROUP BY COL_0) AS grouped
        """)
        rows = sorted(_rows(db, "grouped", 2))
        assert rows == [("A", 30), ("B", 70)]

    def test_explicit_columns_still_work(self):
        """FROM pred(COL1, COL2) with explicit names should still work."""
        data = pd.DataFrame({
            "x": [1, 2],
            "y": [3, 4],
        })
        db = bb.db()
        db.load_df(data, "tbl")
        db.sql("""
            (SELECT MY_A
             FROM tbl(MY_A, MY_B)
             WHERE MY_B > 3) AS filtered
        """)
        rows = _rows(db, "filtered", 1)
        assert rows == [(2,)]


class TestExplain:
    """Tests for explain() — returns generated Datalog rules without executing."""

    def test_explain_datalog(self):
        db = bb.db()
        result = db.explain("a(1,2). b(X,Y) :- a(X,Y). b(X,Y)?")
        assert "b(X,Y) :- a(X,Y)." in result

    def test_explain_sql(self):
        csv = str(EXAMPLES_DATA / "employees.csv")
        db = bb.db()
        result = db.explain(f'%@sql\n(SELECT NAME, SALARY FROM "{csv}") AS emp')
        # SQL should be translated to Datalog rules
        assert "emp" in result
        assert "read_csv" in result or ":-" in result
