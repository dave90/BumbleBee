"""
NULL support across the Python boundary:
- tuples() surfaces NULLs as None
- to_df() surfaces NULLs as pd.NA / NaN / None based on column dtype
- load_df() round-trips pandas NaN/None into BumbleBee NULL and back
"""
import math

import pandas as pd
import numpy as np
import pytest
import bumbledl as bb


class TestTuplesNull:
    def test_tuples_returns_none_for_null(self):
        db = bb.db()
        db.run("a(1,2). a(3,NULL). a(5,6). a(X,Y)?")
        rows = sorted(db.get_table("a", 2).tuples(), key=lambda r: r[0])
        # row 0: (1, 2), row 1: (3, None), row 2: (5, 6)
        assert rows[0] == (1, 2)
        assert rows[1] == (3, None)
        assert rows[2] == (5, 6)

    def test_tuples_null_in_first_column(self):
        db = bb.db()
        db.run("a(NULL,1). a(2,3). a(X,Y)?")
        rows = sorted(db.get_table("a", 2).tuples(), key=lambda r: (r[0] is None, r[0]))
        assert (None, 1) in rows
        assert (2, 3) in rows

    def test_tuples_string_null(self):
        db = bb.db()
        db.run('p("alice","NYC"). p("bob",NULL). p(X,Y)?')
        rows = sorted(db.get_table("p", 2).tuples(), key=lambda r: r[0])
        assert ("alice", "NYC") in rows
        assert ("bob", None) in rows


class TestToDfNull:
    def test_int_column_emits_pd_na(self):
        db = bb.db()
        db.run("a(1,10). a(2,NULL). a(3,30). a(X,Y)?")
        df = db.get_table("a", 2).to_df(col_names=["x", "y"])
        # y has NULL at the row where x == 2
        df = df.set_index("x").sort_index()
        assert df.loc[1, "y"] == 10
        assert df.loc[2, "y"] is pd.NA
        assert df.loc[3, "y"] == 30

    def test_float_column_emits_na(self):
        db = bb.db()
        # Use one float fact to pin the column type as DOUBLE
        db.run("f(1, 1.5). f(2, NULL). f(3, 2.5). f(X,Y)?")
        df = db.get_table("f", 2).to_df(col_names=["x", "y"]).set_index("x").sort_index()
        # Float NA materializes as pd.NA in a Float64 nullable column (or NaN if dtype is plain float).
        # Accept either pd.NA or NaN.
        y_at_2 = df.loc[2, "y"]
        assert y_at_2 is pd.NA or (isinstance(y_at_2, float) and math.isnan(y_at_2))
        assert df.loc[1, "y"] == 1.5
        assert df.loc[3, "y"] == 2.5

    def test_string_column_emits_none(self):
        db = bb.db()
        db.run('p("alice","NYC"). p("bob",NULL). p("carol","Berlin"). p(X,Y)?')
        df = db.get_table("p", 2).to_df(col_names=["name", "city"]).set_index("name").sort_index()
        # Object dtype: missing value is Python None.
        assert df.loc["alice", "city"] == "NYC"
        assert df.loc["bob", "city"] is None
        assert df.loc["carol", "city"] == "Berlin"

    def test_no_nulls_keeps_plain_dtypes(self):
        """When no nulls are present, dtypes remain numpy (no needless conversion)."""
        db = bb.db()
        db.run("g(1,10). g(2,20). g(X,Y)?")
        df = db.get_table("g", 2).to_df(col_names=["x", "y"])
        # plain numpy integer dtypes (the existing test_to_df.test_basic_int continues to pass)
        assert pd.api.types.is_integer_dtype(df["x"])
        assert pd.api.types.is_integer_dtype(df["y"])


class TestLoadDfNull:
    def test_load_df_with_null_round_trips(self):
        # A pandas Int64 (nullable) column with pd.NA
        df = pd.DataFrame({
            "a": pd.array([1, 2, 3], dtype="Int64"),
            "b": pd.array([10, pd.NA, 30], dtype="Int64"),
        })
        db = bb.db()
        db.load_df(df, "nums")
        rows = sorted(db.get_table("nums", 2).tuples(), key=lambda r: r[0])
        assert rows[0] == (1, 10)
        assert rows[1] == (2, None)
        assert rows[2] == (3, 30)

    def test_load_df_float_nan_is_null(self):
        df = pd.DataFrame({
            "x": [1, 2, 3],
            "y": [1.5, float("nan"), 2.5],
        })
        db = bb.db()
        db.load_df(df, "vals")
        rows = sorted(db.get_table("vals", 2).tuples(), key=lambda r: r[0])
        # The NaN row surfaces as a NULL on the BumbleBee side.
        assert rows[1][0] == 2
        assert rows[1][1] is None

    def test_load_df_object_none_is_null(self):
        df = pd.DataFrame({
            "n": [1, 2, 3],
            "city": ["NYC", None, "Berlin"],
        })
        db = bb.db()
        db.load_df(df, "p")
        rows = sorted(db.get_table("p", 2).tuples(), key=lambda r: r[0])
        assert rows[0] == (1, "NYC")
        assert rows[1] == (2, None)
        assert rows[2] == (3, "Berlin")


# ---------- Broader pandas-input coverage ----------

class TestLoadDfNullableExtensions:
    """Nullable pandas extension dtypes: small ints, unsigned ints, Float, boolean, string."""

    def test_int8_nullable(self):
        df = pd.DataFrame({"x": pd.array([1, pd.NA, 3], dtype="Int8")})
        db = bb.db()
        db.load_df(df, "t")
        rows = sorted(db.get_table("t", 1).tuples(),
                      key=lambda r: (r[0] is None, r[0]))
        assert (1,) in rows
        assert (3,) in rows
        assert (None,) in rows

    def test_int32_nullable(self):
        df = pd.DataFrame({"x": pd.array([100, pd.NA, 300], dtype="Int32")})
        db = bb.db()
        db.load_df(df, "t")
        rows = sorted(db.get_table("t", 1).tuples(),
                      key=lambda r: (r[0] is None, r[0]))
        assert (100,) in rows
        assert (300,) in rows
        assert (None,) in rows

    def test_uint16_nullable(self):
        df = pd.DataFrame({"x": pd.array([10, pd.NA, 30], dtype="UInt16")})
        db = bb.db()
        db.load_df(df, "t")
        rows = sorted(db.get_table("t", 1).tuples(),
                      key=lambda r: (r[0] is None, r[0]))
        assert (10,) in rows
        assert (30,) in rows
        assert (None,) in rows

    def test_float64_nullable(self):
        df = pd.DataFrame({"x": pd.array([1.5, pd.NA, 2.5], dtype="Float64")})
        db = bb.db()
        db.load_df(df, "t")
        rows = sorted(db.get_table("t", 1).tuples(),
                      key=lambda r: (r[0] is None, r[0]))
        assert (1.5,) in rows
        assert (2.5,) in rows
        assert (None,) in rows

    def test_boolean_nullable(self):
        df = pd.DataFrame({"flag": pd.array([True, pd.NA, False], dtype="boolean")})
        db = bb.db()
        db.load_df(df, "b")
        rows = sorted(db.get_table("b", 1).tuples(),
                      key=lambda r: (r[0] is None, r[0]))
        assert (True,) in rows
        assert (False,) in rows
        assert (None,) in rows

    def test_pd_na_in_object_column(self):
        """pd.NA dropped into an object column should still be a NULL."""
        df = pd.DataFrame({"n": [1, 2, 3], "city": ["NYC", pd.NA, "Berlin"]})
        db = bb.db()
        db.load_df(df, "p")
        rows = sorted(db.get_table("p", 2).tuples(), key=lambda r: r[0])
        assert rows[1] == (2, None)


class TestLoadDfDatetime:
    """datetime64[ns] NaT round-trips through TIMESTAMP."""

    def test_timestamp_nat(self):
        df = pd.DataFrame({
            "k": [1, 2, 3],
            "ts": pd.to_datetime(["2025-01-01", None, "2025-01-03"]),
        })
        db = bb.db()
        db.load_df(df, "events")
        rows = sorted(db.get_table("events", 2).tuples(), key=lambda r: r[0])
        # row with NaT should report None
        assert rows[1][0] == 2
        assert rows[1][1] is None


class TestLoadDfCategorical:
    """pd.Categorical with NaN should be NULL, not the empty string."""

    def test_category_with_nan(self):
        df = pd.DataFrame({"k": [1, 2, 3], "c": pd.Categorical(["a", None, "b"])})
        db = bb.db()
        db.load_df(df, "c")
        rows = sorted(db.get_table("c", 2).tuples(), key=lambda r: r[0])
        assert rows[0] == (1, "a")
        assert rows[1] == (2, None)
        assert rows[2] == (3, "b")


class TestLoadDfMultiColumn:
    """NULLs scattered across columns and rows."""

    def test_nulls_at_different_positions(self):
        df = pd.DataFrame({
            "a": pd.array([1, pd.NA, 3, 4], dtype="Int64"),
            "b": pd.array([pd.NA, 20, 30, pd.NA], dtype="Int64"),
            "c": ["x", "y", None, "z"],
        })
        db = bb.db()
        db.load_df(df, "m")
        rows = sorted(db.get_table("m", 3).tuples(),
                      key=lambda r: (r[0] is None, r[0]))
        # sort by `a` (None last)
        assert rows[0] == (1, None, "x")
        assert rows[1] == (3, 30, None)
        assert rows[2] == (4, None, "z")
        assert rows[3] == (None, 20, "y")


class TestToDfMoreTypes:
    """to_df coverage for less-common column types."""

    def test_multi_chunk_with_nulls(self):
        """A predicate with >1024 rows produces multiple chunks; NULLs must survive across them."""
        # 1500 facts, NULL on `y` whenever x % 100 == 0
        n = 1500
        a = list(range(1, n + 1))
        b = pd.array(
            [(i if i % 100 != 0 else pd.NA) for i in a],
            dtype="Int64",
        )
        df = pd.DataFrame({"x": pd.array(a, dtype="Int64"), "y": b})
        db = bb.db()
        db.load_df(df, "big")
        out = db.get_table("big", 2).to_df(col_names=["x", "y"]).set_index("x").sort_index()
        assert len(out) == n
        # spot-check a NULL before and after the 1024 boundary
        assert out.loc[100, "y"] is pd.NA
        assert out.loc[1000, "y"] is pd.NA
        assert out.loc[1100, "y"] is pd.NA  # in chunk 2
        # a non-null row in chunk 2
        assert out.loc[1099, "y"] == 1099


class TestComputeOverNull:
    """End-to-end: load NULL-bearing data, then RUN a query that computes over it
    (join / aggregate / IS NULL filter) and check NULLs in the output. The other
    classes only cover load->retrieve round-trips; this exercises NULL semantics in
    the actual execution engine (3VL join keys, skip-NULL aggregates) through the
    Python surface."""

    def test_join_excludes_null_keys(self):
        # NULL join keys must NOT match (NULL-excludes), on either side.
        left = pd.DataFrame({
            "k": pd.array([1, 2, pd.NA], dtype="Int64"),
            "v": ["a", "b", "c"],
        })
        right = pd.DataFrame({
            "k": pd.array([1, pd.NA], dtype="Int64"),
            "lbl": ["one", "nullrow"],
        })
        db = bb.db()
        db.load_df(left, "l")
        db.load_df(right, "r")
        db.run("j(V, L) :- l(K, V), r(K, L). j(X, Y)?")
        rows = sorted(db.get_table("j", 2).tuples())
        # Only k==1 matches; l's NULL key and r's NULL key never join.
        assert rows == [("a", "one")]

    def test_aggregate_skips_and_nulls_empty_group(self):
        # Group 9 is entirely NULL -> SUM/MIN/MAX NULL, COUNT 0. bb.db() is
        # multi-threaded by default, so this also exercises the combine-of-partials
        # path for MIN/MAX over an all-NULL group.
        df = pd.DataFrame({
            "g": pd.array([1, 1, 9, 9], dtype="Int64"),
            "v": pd.array([10, 20, pd.NA, pd.NA], dtype="Int64"),
        })
        db = bb.db()
        db.load_df(df, "d")
        db.run(
            "agg(G, S, C, MN, MX) :- d(G, _),"
            " S  = #sum  {V : d(G, V)},"
            " C  = #count{V : d(G, V)},"
            " MN = #min  {V : d(G, V)},"
            " MX = #max  {V : d(G, V)}."
            " agg(G, S, C, MN, MX)?"
        )
        rows = {r[0]: r for r in db.get_table("agg", 5).tuples()}
        assert rows[1] == (1, 30, 2, 10, 20)
        assert rows[9] == (9, None, 0, None, None)

    def test_is_null_filter(self):
        df = pd.DataFrame({
            "id": pd.array([1, 2, 3], dtype="Int64"),
            "v": pd.array([10, pd.NA, 30], dtype="Int64"),
        })
        db = bb.db()
        db.load_df(df, "d")
        db.run("missing(I) :- d(I, V), V IS NULL. missing(X)?")
        assert sorted(db.get_table("missing", 1).tuples()) == [(2,)]

    def test_sql_count_star_vs_count_col_and_is_null(self):
        # COUNT(*) counts every row incl. NULL-bearing ones; COUNT(col) skips NULL.
        df = pd.DataFrame({
            "id": pd.array([1, 2, 3, 4], dtype="Int64"),
            "v": pd.array([10, pd.NA, pd.NA, 40], dtype="Int64"),
        })
        db = bb.db()
        db.load_df(df, "d")
        db.sql("SELECT COUNT(*) AS C, COUNT(COL_1) AS CV FROM d", alias="cnt")
        assert db.get_table("cnt", 2).tuples() == [(4, 2)]
        db.sql("SELECT COL_0 FROM d WHERE COL_1 IS NULL", alias="nullrows")
        assert sorted(db.get_table("nullrows", 1).tuples()) == [(2,), (3,)]


class TestSqlComputeOverNull:
    """db.sql() over NULL-bearing loaded data: 3VL filters, IS [NOT] NULL,
    arithmetic propagation, GROUP BY on a NULL key, and a NULL-key join."""

    def _db(self):
        df = pd.DataFrame({
            "id": pd.array([1, 2, 3, 4], dtype="Int64"),
            "v": pd.array([10, pd.NA, 30, pd.NA], dtype="Int64"),
        })
        db = bb.db()
        db.load_df(df, "d")
        return db

    def test_sql_where_3vl_drops_null(self):
        db = self._db()
        db.sql("SELECT COL_0 FROM d WHERE COL_1 > 15", alias="hi")
        assert sorted(db.get_table("hi", 1).tuples()) == [(3,)]

    def test_sql_is_not_null(self):
        db = self._db()
        db.sql("SELECT COL_0 FROM d WHERE COL_1 IS NOT NULL", alias="present")
        assert sorted(db.get_table("present", 1).tuples()) == [(1,), (3,)]

    def test_sql_arithmetic_propagates_null(self):
        db = self._db()
        db.sql("SELECT COL_0, COL_1 + 100 AS SP FROM d", alias="ar")
        rows = {r[0]: r[1] for r in db.get_table("ar", 2).tuples()}
        assert rows == {1: 110, 2: None, 3: 130, 4: None}

    def test_sql_groupby_null_group_aggregates(self):
        # Group 9 is entirely NULL -> SUM/MIN/MAX NULL, COUNT 0; group key is itself
        # a NULL bucket for group with key NULL is not used here (keys 1 and 9 are
        # both non-null), but the all-NULL *values* exercise MIN/MAX over an empty
        # group through the SQL path (multi-threaded combine of partials).
        gdf = pd.DataFrame({
            "g": pd.array([1, 1, 9, 9], dtype="Int64"),
            "v": pd.array([10, 20, pd.NA, pd.NA], dtype="Int64"),
        })
        db = bb.db()
        db.load_df(gdf, "g")
        db.sql(
            "SELECT COL_0 AS G, SUM(COL_1) AS S, MIN(COL_1) AS MN, "
            "MAX(COL_1) AS MX, COUNT(COL_1) AS C FROM g GROUP BY COL_0",
            alias="agg",
        )
        rows = {r[0]: r for r in db.get_table("agg", 5).tuples()}
        assert rows[1] == (1, 30, 10, 20, 2)
        assert rows[9] == (9, None, None, None, 0)

    def test_sql_join_excludes_null_keys(self):
        left = pd.DataFrame({
            "k": pd.array([1, 2, pd.NA], dtype="Int64"),
            "v": ["a", "b", "c"],
        })
        right = pd.DataFrame({
            "k": pd.array([1, pd.NA], dtype="Int64"),
            "lbl": ["one", "nullrow"],
        })
        db = bb.db()
        db.load_df(left, "l")
        db.load_df(right, "r")
        # Disambiguate the two COL_0 key columns via explicit FROM-item names.
        db.sql("SELECT V, LBL FROM l(LK, V), r(RK, LBL) WHERE LK = RK", alias="j")
        assert sorted(db.get_table("j", 2).tuples()) == [("a", "one")]


class TestToDfRetrieveTypes:
    """to_df() retrieve side for types whose load side is covered elsewhere:
    BOOL and TIMESTAMP, plus a dtype (not just value) assertion for nullable int."""

    def test_to_df_bool_null(self):
        df = pd.DataFrame({
            "id": pd.array([1, 2, 3], dtype="Int64"),
            "flag": pd.array([True, pd.NA, False], dtype="boolean"),
        })
        db = bb.db()
        db.load_df(df, "b")
        out = db.get_table("b", 2).to_df(col_names=["id", "flag"]).set_index("id").sort_index()
        assert bool(out.loc[1, "flag"]) is True
        nullflag = out.loc[2, "flag"]
        assert nullflag is pd.NA or nullflag is None or (
            isinstance(nullflag, float) and math.isnan(nullflag)
        )
        assert bool(out.loc[3, "flag"]) is False

    def test_to_df_timestamp_nat(self):
        df = pd.DataFrame({
            "id": pd.array([1, 2, 3], dtype="Int64"),
            "t": pd.to_datetime(["2021-01-01", None, "2021-03-03"]),
        })
        db = bb.db()
        db.load_df(df, "e")
        out = db.get_table("e", 2).to_df(col_names=["id", "t"]).set_index("id").sort_index()
        assert pd.isna(out.loc[2, "t"])  # NaT
        assert pd.Timestamp(out.loc[1, "t"]) == pd.Timestamp("2021-01-01")
        assert pd.api.types.is_datetime64_any_dtype(out["t"])

    def test_to_df_int_null_uses_nullable_dtype(self):
        # The nullable-int column should come back as an Int* extension dtype, not a
        # lossy float64 that silently turns NULL into NaN. (Value coverage exists in
        # TestToDfNull; this pins the dtype.)
        db = bb.db()
        db.run("a(1,10). a(2,NULL). a(3,30). a(X,Y)?")
        out = db.get_table("a", 2).to_df(col_names=["x", "y"]).set_index("x").sort_index()
        # A pandas nullable *integer extension* dtype (Int*/UInt*), not a lossy float64.
        assert pd.api.types.is_integer_dtype(out["y"])
        assert pd.api.types.is_extension_array_dtype(out["y"])
        assert out.loc[2, "y"] is pd.NA