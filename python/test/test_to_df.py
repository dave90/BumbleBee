import pytest
import pandas as pd
import numpy as np
import bumblebee
from pathlib import Path
from conftest import DATA_DIR, _sql

_PROJECT_ROOT = Path(__file__).resolve().parents[2]
PARQUET_DIR = _PROJECT_ROOT / "test" / "unit" / "bumblebee" / "function" / "data" / "input" / "parquet"


class TestToDf:
    """Tests for PredicateTable.to_df()."""

    def test_basic_int(self):
        """Integer facts produce a DataFrame with correct values and default column names."""
        db = bumblebee.db()
        db.run("fact(1, 10). fact(2, 20). fact(3, 30). fact(X, Y)?")
        df = db.get_table("fact", 2).to_df()
        assert isinstance(df, pd.DataFrame)
        assert list(df.columns) == ["COL_0", "COL_1"]
        assert sorted(df.values.tolist()) == [[1, 10], [2, 20], [3, 30]]

    def test_custom_col_names(self):
        """Custom column names are applied correctly."""
        db = bumblebee.db()
        db.run("pair(1, 2). pair(3, 4). pair(X, Y)?")
        df = db.get_table("pair", 2).to_df(col_names=["a", "b"])
        assert list(df.columns) == ["a", "b"]
        assert sorted(df.values.tolist()) == [[1, 2], [3, 4]]

    def test_wrong_col_names_count_raises(self):
        """Passing wrong number of column names raises ValueError."""
        db = bumblebee.db()
        db.run("v(1). v(X)?")
        with pytest.raises(Exception):
            db.get_table("v", 1).to_df(col_names=["a", "b"])

    def test_single_column(self):
        """Single-column predicate produces a one-column DataFrame."""
        db = bumblebee.db()
        db.run("item(10). item(20). item(30). item(X)?")
        df = db.get_table("item", 1).to_df(col_names=["val"])
        assert list(df.columns) == ["val"]
        assert sorted(df["val"].tolist()) == [10, 20, 30]

    def test_string_column(self):
        """String values are correctly converted."""
        db = bumblebee.db()
        db.run('name("alice"). name("bob"). name(X)?')
        df = db.get_table("name", 1).to_df(col_names=["n"])
        assert sorted(df["n"].tolist()) == ["alice", "bob"]

    def test_float_column(self):
        """Float values are correctly converted."""
        db = bumblebee.db()
        db.run("v(1.5). v(2.75). v(X)?")
        df = db.get_table("v", 1).to_df(col_names=["val"])
        vals = sorted(df["val"].tolist())
        assert abs(vals[0] - 1.5) < 1e-6
        assert abs(vals[1] - 2.75) < 1e-6

    def test_mixed_types(self):
        """Mixed int and string columns produce correct dtypes."""
        db = bumblebee.db()
        db.run('rec(1, "alice"). rec(2, "bob"). rec(X, Y)?')
        df = db.get_table("rec", 2).to_df(col_names=["id", "name"])
        assert sorted(df.values.tolist(), key=lambda r: r[0]) == [[1, "alice"], [2, "bob"]]

    def test_empty_result(self):
        """Empty predicate produces an empty DataFrame."""
        db = bumblebee.db()
        db.run("src(1). out(X) :- src(X), X > 100. out(X)?")
        df = db.get_table("out", 1).to_df(col_names=["val"])
        assert isinstance(df, pd.DataFrame)
        assert len(df) == 0

    def test_large_dataset(self):
        """DataFrame conversion works for >STANDARD_VECTOR_SIZE rows (batching)."""
        n = 2048
        facts = " ".join(f"row({i})." for i in range(n))
        db = bumblebee.db()
        db.run(facts + " row(X)?")
        df = db.get_table("row", 1).to_df(col_names=["val"])
        assert len(df) == n
        assert sorted(df["val"].tolist()) == list(range(n))

    def test_default_col_names_multi(self):
        """Default column names follow COL_0, COL_1, ... pattern."""
        db = bumblebee.db()
        db.run("t(1, 2, 3). t(X, Y, Z)?")
        df = db.get_table("t", 3).to_df()
        assert list(df.columns) == ["COL_0", "COL_1", "COL_2"]

    def test_sql_result_to_df(self):
        """to_df works on SQL query results."""
        csv = DATA_DIR / "customers-1.csv"
        db = bumblebee.db()
        db.sql(f"""
            (SELECT FIRST_NAME, LAST_NAME
            FROM "{csv}"
            WHERE COUNTRY = 'Norway') AS norw
        """)
        df = db.get_table("norw", 2).to_df(col_names=["first", "last"])
        assert list(df.columns) == ["first", "last"]
        assert df.values.tolist() == [["Heather", "Callahan"]]

    def test_loaded_df_roundtrip(self):
        """Load a pandas DataFrame, query it, and convert back to DataFrame."""
        input_df = pd.DataFrame({"a": [1, 2, 3], "b": [10, 20, 30]})
        db = bumblebee.db()
        db.load_df(input_df, "src")
        db.run("out(X, Y) :- src(X, Y), X > 1. out(X, Y)?")
        result = db.get_table("out", 2).to_df(col_names=["a", "b"])
        expected = pd.DataFrame({"a": [2, 3], "b": [20, 30]})
        result_sorted = result.sort_values("a").reset_index(drop=True)
        pd.testing.assert_frame_equal(result_sorted, expected)

    def test_distinct_predicate(self):
        """to_df works correctly with distinct predicates (hash table scan path)."""
        db = bumblebee.db({"-d": ""})
        db.run("v(1). v(1). v(2). v(3). v(X)?")
        df = db.get_table("v", 1).to_df(col_names=["val"])
        assert sorted(df["val"].tolist()) == [1, 2, 3]

    # ---- Parquet type tests ----

    def test_decimal_from_parquet(self):
        """Decimal columns from parquet produce float64 in DataFrame."""
        pq = DATA_DIR / "data_decimal.parquet"
        db = bumblebee.db()
        db.sql(f'(SELECT COL1, COL4 FROM "{pq}" WHERE COL1 <= 3) AS t')
        df = db.get_table("t", 2).to_df(col_names=["price", "label"])
        assert df["price"].dtype == np.float64
        assert sorted(df.values.tolist()) == [[1.0, "row_1"], [2.0, "row_2"], [3.0, "row_3"]]

    def test_decimal_sum_from_parquet(self):
        """Decimal SUM aggregate produces correct float64 value."""
        pq = DATA_DIR / "data_decimal.parquet"
        db = bumblebee.db()
        db.sql(f'(SELECT SUM(COL1) AS TOTAL FROM "{pq}") AS t')
        df = db.get_table("t", 1).to_df(col_names=["total"])
        assert df["total"].dtype == np.float64
        # COL1 has values 1..100 with scale=2, sum of raw = 5050
        assert abs(df["total"].iloc[0] - 5050.0) < 1e-2

    def test_date_from_parquet(self):
        """Date columns from parquet produce datetime64[ns] in DataFrame."""
        pq = PARQUET_DIR / "tpch_lineitem.parquet"
        db = bumblebee.db()
        db.sql(f'(SELECT L_SHIPDATE FROM "{pq}" WHERE L_ORDERKEY = 1) AS t')
        df = db.get_table("t", 1).to_df(col_names=["shipdate"])
        assert df["shipdate"].dtype == np.dtype("datetime64[ns]")
        assert len(df) == 6
        assert df["shipdate"].notna().all()

    def test_date_filter_from_parquet(self):
        """Date filter and projection produce correct results."""
        pq = PARQUET_DIR / "tpch_lineitem.parquet"
        db = bumblebee.db()
        db.sql(f"""(SELECT L_ORDERKEY, L_SHIPDATE FROM "{pq}"
                    WHERE L_ORDERKEY = 1) AS t""")
        df = db.get_table("t", 2).to_df(col_names=["orderkey", "shipdate"])
        assert df["shipdate"].dtype == np.dtype("datetime64[ns]")
        assert (df["orderkey"] == 1).all()

    def test_timestamp_from_parquet(self):
        """Timestamp columns from parquet produce datetime64[ns] in DataFrame."""
        pq = PARQUET_DIR / "mini_hits.parquet"
        db = bumblebee.db()
        db.sql(f'(SELECT EVENTTIME FROM "{pq}" WHERE COUNTERID = 76221) AS t')
        df = db.get_table("t", 1).to_df(col_names=["ts"])
        assert df["ts"].dtype == np.dtype("datetime64[ns]")
        assert len(df) == 2048
        assert df["ts"].notna().all()

    def test_mixed_parquet_types(self):
        """Multiple column types (int, decimal, string, date) in one DataFrame."""
        pq = PARQUET_DIR / "tpch_lineitem.parquet"
        db = bumblebee.db()
        db.sql(f"""(SELECT L_ORDERKEY, L_QUANTITY, L_RETURNFLAG, L_SHIPDATE
                    FROM "{pq}" WHERE L_ORDERKEY = 1) AS t""")
        df = db.get_table("t", 4).to_df(
            col_names=["orderkey", "qty", "flag", "shipdate"]
        )
        assert len(df) == 6
        assert df["orderkey"].dtype == np.int64
        assert df["qty"].dtype == np.float64
        assert df["flag"].dtype == object  # string
        assert df["shipdate"].dtype == np.dtype("datetime64[ns]")

    def test_load_query_export_reload(self):
        """Load a DataFrame, query, export to_df, recreate db, reload, query again — results must match."""
        input_df = pd.DataFrame({
            "id": [1, 2, 3, 4, 5],
            "name": ["alice", "bob", "carol", "dave", "eve"],
            "score": [85, 92, 78, 95, 88],
        })

        # First run
        db = bumblebee.db()
        db.load_df(input_df, "students")
        db.run("top(N, S) :- students(_, N, S), S >= 90. top(X, Y)?")
        df1 = db.get_table("top", 2).to_df(col_names=["name", "score"])
        df1 = df1.sort_values("name").reset_index(drop=True)

        assert len(df1) == 2
        assert df1.values.tolist() == [["bob", 92], ["dave", 95]]

        # Second run — fresh db, same input
        db = bumblebee.db()
        db.load_df(input_df, "students")
        db.run("top(N, S) :- students(_, N, S), S >= 90. top(X, Y)?")
        df2 = db.get_table("top", 2).to_df(col_names=["name", "score"])
        df2 = df2.sort_values("name").reset_index(drop=True)

        pd.testing.assert_frame_equal(df1, df2)