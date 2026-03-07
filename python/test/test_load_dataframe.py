import pytest
import pandas as pd
import bumblebee
from conftest import _rows


class TestLoadDataframe:
    """Tests for db.load_df(df, alias)."""

    def test_basic_int_columns(self):
        """Load a simple integer DataFrame and query it."""
        df = pd.DataFrame({"a": [1, 2, 3], "b": [10, 20, 30]})
        db = bumblebee.db()
        db.load_df(df, "nums")
        db.run("out(X, Y) :- nums(X, Y). out(X, Y)?")
        assert sorted(_rows(db, "out", 2)) == [(1, 10), (2, 20), (3, 30)]

    def test_string_columns(self):
        """Load a DataFrame with string columns."""
        df = pd.DataFrame({"name": ["alice", "bob", "carol"]})
        db = bumblebee.db()
        db.load_df(df, "people")
        db.run("out(X) :- people(X). out(X)?")
        assert sorted(_rows(db, "out", 1)) == [("alice",), ("bob",), ("carol",)]

    def test_mixed_types(self):
        """Load a DataFrame with mixed int and string columns."""
        df = pd.DataFrame({"id": [1, 2], "name": ["alice", "bob"]})
        db = bumblebee.db()
        db.load_df(df, "mixed")
        db.run("out(X, Y) :- mixed(X, Y). out(X, Y)?")
        assert sorted(_rows(db, "out", 2)) == [(1, "alice"), (2, "bob")]

    def test_float_columns(self):
        """Load a DataFrame with float columns."""
        df = pd.DataFrame({"val": [1.5, 2.75, 3.0]})
        db = bumblebee.db()
        db.load_df(df, "floats")
        db.run("out(X) :- floats(X). out(X)?")
        rows = sorted(_rows(db, "out", 1))
        assert rows == [(1.5,), (2.75,), (3.0,)]

    def test_filter_on_loaded_df(self):
        """Apply a Datalog filter on a loaded DataFrame."""
        df = pd.DataFrame({"x": [1, 2, 3, 4, 5], "y": [10, 20, 30, 40, 50]})
        db = bumblebee.db()
        db.load_df(df, "data")
        db.run("big(X, Y) :- data(X, Y), X > 3. big(X, Y)?")
        assert sorted(_rows(db, "big", 2)) == [(4, 40), (5, 50)]

    def test_join_with_datalog_facts(self):
        """Join a loaded DataFrame with Datalog facts."""
        df = pd.DataFrame({"id": [1, 2, 3], "name": ["alice", "bob", "carol"]})
        db = bumblebee.db()
        db.load_df(df, "emp")
        db.run('score(1, 85). score(2, 92). score(3, 78). score(X, Y)?')
        db.run("result(N, S) :- emp(ID, N), score(ID, S). result(X, Y)?")
        assert sorted(_rows(db, "result", 2)) == [("alice", 85), ("bob", 92), ("carol", 78)]

    def test_sql_on_loaded_df(self):
        """Query a loaded DataFrame using SQL."""
        df = pd.DataFrame({"id": [1, 2, 3], "name": ["alice", "bob", "carol"]})
        db = bumblebee.db()
        db.load_df(df, "emp")
        db.sql("SELECT V1, V2 FROM emp WHERE V1 > 1", alias="result")
        assert sorted(_rows(db, "result", 2)) == [(2, "bob"), (3, "carol")]

    def test_large_dataframe(self):
        """Load a large DataFrame (>STANDARD_VECTOR_SIZE rows) to test batching."""
        n = 2048
        df = pd.DataFrame({"val": list(range(n))})
        db = bumblebee.db()
        db.load_df(df, "big")
        db.run("out(X) :- big(X). out(X)?")
        rows = sorted(_rows(db, "out", 1))
        assert len(rows) == n
        assert rows[0] == (0,)
        assert rows[-1] == (n - 1,)

    def test_duplicate_alias_raises(self):
        """Loading a second DataFrame with the same alias should raise."""
        df = pd.DataFrame({"a": [1]})
        db = bumblebee.db()
        db.load_df(df, "dup")
        with pytest.raises(RuntimeError, match="already exist"):
            db.load_df(df, "dup")

    def test_invalid_alias_raises(self):
        """Alias must start with lowercase letter."""
        df = pd.DataFrame({"a": [1]})
        db = bumblebee.db()
        with pytest.raises(RuntimeError, match="lower case"):
            db.load_df(df, "Upper")

    def test_empty_alias_raises(self):
        """Empty alias should raise."""
        df = pd.DataFrame({"a": [1]})
        db = bumblebee.db()
        with pytest.raises(RuntimeError, match="lower case"):
            db.load_df(df, "")

    def test_two_dataframes_join(self):
        """Load two DataFrames and join them with Datalog."""
        employees = pd.DataFrame({"id": [1, 2, 3], "name": ["alice", "bob", "carol"]})
        salaries = pd.DataFrame({"id": [1, 2, 3], "salary": [50000, 60000, 55000]})
        db = bumblebee.db()
        db.load_df(employees, "emp")
        db.load_df(salaries, "sal")
        db.run("rich(N, S) :- emp(ID, N), sal(ID, S), S > 55000. rich(X, Y)?")
        db.run("total(N, S) :- emp(ID, N), sal(ID, S). total(X, Y)?")
        assert sorted(_rows(db, "rich", 2)) == [("bob", 60000)]
        assert sorted(_rows(db, "total", 2)) == [
            ("alice", 50000), ("bob", 60000), ("carol", 55000),
        ]

    def test_three_dataframes_sql_and_datalog(self):
        """Load three DataFrames, query with both SQL and Datalog."""
        products = pd.DataFrame({"pid": [1, 2, 3], "name": ["widget", "gadget", "gizmo"]})
        orders = pd.DataFrame({"oid": [10, 11, 12, 13], "pid": [1, 2, 1, 3], "qty": [5, 3, 2, 7]})
        customers = pd.DataFrame({"oid": [10, 11, 12, 13], "cname": ["alice", "bob", "carol", "alice"]})
        db = bumblebee.db()
        db.load_df(products, "products")
        db.load_df(orders, "orders")
        db.load_df(customers, "customers")
        # Datalog: join all three to get customer-product pairs
        db.run("bought(C, P) :- customers(O, C), orders(O, PID, _), products(PID, P). bought(X, Y)?")
        assert sorted(_rows(db, "bought", 2)) == [
            ("alice", "gizmo"), ("alice", "widget"),
            ("bob", "gadget"), ("carol", "widget"),
        ]
        # SQL: aggregate total quantity per product
        db.sql("SELECT V2, SUM(V3) AS TOTAL FROM orders GROUP BY V2", alias="qty_by_product")
        assert sorted(_rows(db, "qty_by_product", 2)) == [(1, 7), (2, 3), (3, 7)]

    def test_two_dataframes_multiple_sql_queries(self):
        """Load two DataFrames and run multiple SQL queries on them."""
        dept = pd.DataFrame({"did": [1, 2, 3], "dname": ["eng", "sales", "hr"]})
        staff = pd.DataFrame({
            "sid": [1, 2, 3, 4, 5],
            "did": [1, 1, 2, 2, 3],
            "score": [90, 85, 70, 95, 80],
        })
        db = bumblebee.db()
        db.load_df(dept, "dept")
        db.load_df(staff, "staff")
        db.sql("SELECT COUNT(*) AS CNT FROM staff", alias="total_staff")
        db.sql("SELECT V2, COUNT(*) AS CNT FROM staff GROUP BY V2", alias="staff_per_dept")
        db.sql("SELECT V1, V2 FROM dept WHERE V2 = 'eng'", alias="eng_dept")
        assert _rows(db, "total_staff", 1) == [(5,)]
        assert sorted(_rows(db, "staff_per_dept", 2)) == [(1, 2), (2, 2), (3, 1)]
        assert _rows(db, "eng_dept", 2) == [(1, "eng")]