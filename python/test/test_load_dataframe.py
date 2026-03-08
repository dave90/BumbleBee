import pytest
import pandas as pd
import numpy as np
from datetime import date, time, timedelta
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

    # ---- Edge-case pandas types ----

    def test_timestamp_columns(self):
        """Load a DataFrame with datetime64[ns] (timestamp) columns."""
        df = pd.DataFrame({
            "id": [1, 2, 3],
            "ts": pd.to_datetime(["2024-01-15 10:30:00", "2024-06-20 14:00:00", "2024-12-31 23:59:59"]),
        })
        db = bumblebee.db()
        db.load_df(df, "events")
        db.run("out(X, Y) :- events(X, Y). out(X, Y)?")
        rows = sorted(_rows(db, "out", 2))
        assert len(rows) == 3
        # timestamps are stored as TIMESTAMP (microseconds since epoch)
        # and returned as string representation via valueToPython
        assert rows[0][0] == 1
        assert rows[1][0] == 2
        assert rows[2][0] == 3
        # verify all three timestamps were loaded (exact format depends on Timestamp::toString)
        assert len(str(rows[0][1])) > 0
        assert len(str(rows[1][1])) > 0
        assert len(str(rows[2][1])) > 0

    def test_timedelta_columns(self):
        """Load a DataFrame with timedelta64[ns] (interval) columns."""
        df = pd.DataFrame({
            "label": ["short", "medium", "long"],
            "duration": pd.to_timedelta(["1 days", "5 days", "30 days"]),
        })
        db = bumblebee.db()
        db.load_df(df, "durations")
        db.run("out(X, Y) :- durations(X, Y). out(X, Y)?")
        rows = sorted(_rows(db, "out", 2))
        assert len(rows) == 3
        # timedelta is stored as BIGINT (nanoseconds)
        one_day_ns = 86400 * 10**9
        assert rows[1][1] == 5 * one_day_ns  # "medium" = 5 days
        assert rows[0][1] == 30 * one_day_ns  # "long" = 30 days
        assert rows[2][1] == 1 * one_day_ns   # "short" = 1 day

    def test_categorical_string_columns(self):
        """Load a DataFrame with Categorical (enum) string columns.

        Pandas Categorical columns with string categories are converted to
        plain STRING type in BumbleBee — the enum metadata is not preserved,
        category codes are resolved to their string labels during scan.
        """
        df = pd.DataFrame({
            "id": [1, 2, 3, 4],
            "color": pd.Categorical(["red", "green", "blue", "red"]),
        })
        db = bumblebee.db()
        db.load_df(df, "items")
        db.run("out(X, Y) :- items(X, Y). out(X, Y)?")
        rows = sorted(_rows(db, "out", 2))
        # enum values are converted to regular strings
        assert rows == [(1, "red"), (2, "green"), (3, "blue"), (4, "red")]

    def test_categorical_int_columns(self):
        """Load a DataFrame with Categorical integer columns (non-string enum)."""
        df = pd.DataFrame({
            "id": [1, 2, 3],
            "level": pd.Categorical([10, 20, 10]),
        })
        db = bumblebee.db()
        db.load_df(df, "levels")
        db.run("out(X, Y) :- levels(X, Y). out(X, Y)?")
        rows = sorted(_rows(db, "out", 2))
        assert rows == [(1, 10), (2, 20), (3, 10)]

    def test_date_object_columns(self):
        """Load a DataFrame with datetime.date objects (stored as object dtype)."""
        df = pd.DataFrame({
            "id": [1, 2, 3],
            "d": [date(2024, 1, 15), date(2024, 6, 20), date(2024, 12, 31)],
        })
        db = bumblebee.db()
        db.load_df(df, "dates")
        db.run("out(X, Y) :- dates(X, Y). out(X, Y)?")
        rows = sorted(_rows(db, "out", 2))
        assert len(rows) == 3
        # date objects are converted to strings via object path
        assert rows[0] == (1, "2024-01-15")
        assert rows[1] == (2, "2024-06-20")
        assert rows[2] == (3, "2024-12-31")

    def test_time_object_columns(self):
        """Load a DataFrame with datetime.time objects (stored as object dtype)."""
        df = pd.DataFrame({
            "id": [1, 2, 3],
            "t": [time(10, 30, 0), time(14, 0, 0), time(23, 59, 59)],
        })
        db = bumblebee.db()
        db.load_df(df, "times")
        db.run("out(X, Y) :- times(X, Y). out(X, Y)?")
        rows = sorted(_rows(db, "out", 2))
        assert len(rows) == 3
        # time objects are converted to strings via object path
        assert rows[0] == (1, "10:30:00")
        assert rows[1] == (2, "14:00:00")
        assert rows[2] == (3, "23:59:59")