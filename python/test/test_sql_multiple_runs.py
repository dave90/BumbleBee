import bumblebeedb as bb
from conftest import _rows, DATA_DIR


class TestSQLMultipleRuns:
    """Run several SQL queries on the same db instance and verify
    that each result predicate is independently accessible and that
    the default ``query`` predicate accumulates across runs."""

    def test_two_aliases_independent(self):
        """Two runs with distinct aliases produce two independent predicates."""
        csv = DATA_DIR / "customers-1.csv"
        li  = DATA_DIR / "tpch" / "lineitem_2.csv"
        db = bb.db()

        db.sql(f'SELECT COUNT(*) AS CNT FROM "{csv}"', alias="customer_count")
        db.sql(f'SELECT COUNT(*) AS CNT FROM "{li}"',  alias="lineitem_count")

        assert _rows(db, "customer_count", 1) == [(99,)]
        assert _rows(db, "lineitem_count", 1) == [(99,)]

    def test_three_aliases_mixed_sources(self):
        """Three runs mixing CSV and parquet each land in their own predicate."""
        csv = DATA_DIR / "customers-1.csv"
        li  = DATA_DIR / "tpch" / "lineitem_2.csv"
        pq  = DATA_DIR / "data_decimal.parquet"
        db = bb.db()

        db.sql(f'SELECT COUNT(*) AS CNT FROM "{csv}"', alias="csv_count")
        db.sql(f'SELECT COUNT(*) AS CNT FROM "{li}"',  alias="li_count")
        db.sql(f'SELECT COUNT(*) AS CNT FROM "{pq}"',  alias="pq_count")

        assert _rows(db, "csv_count", 1) == [(99,)]
        assert _rows(db, "li_count",  1) == [(99,)]
        assert _rows(db, "pq_count",  1) == [(100,)]

    def test_default_query_predicate_accumulates(self):
        """Three runs without alias with overwrite=False accumulate in ``query``."""
        csv = DATA_DIR / "customers-1.csv"
        db = bb.db()

        # overwrite=False → results accumulate in the default 'query' predicate
        db.sql(f"SELECT FIRST_NAME, LAST_NAME FROM \"{csv}\" WHERE COUNTRY = 'Andorra'", overwrite=False)
        db.sql(f"SELECT FIRST_NAME, LAST_NAME FROM \"{csv}\" WHERE COUNTRY = 'Aruba'", overwrite=False)
        db.sql(f"SELECT FIRST_NAME, LAST_NAME FROM \"{csv}\" WHERE COUNTRY = 'Belize'", overwrite=False)

        assert _rows(db, "query", 2) == [
            ("Aimee",    "Hodge"),
            ("Kristina", "Ferrell"),
            ("Larry",    "Newton"),
        ]

    def test_reuse_alias_overwrites(self):
        """Reusing an alias with default overwrite=True replaces previous result."""
        csv = DATA_DIR / "customers-1.csv"
        db = bb.db()

        db.sql(f"SELECT FIRST_NAME, LAST_NAME FROM \"{csv}\" WHERE COUNTRY = 'Andorra'", alias="q1")
        db.sql(f"SELECT FIRST_NAME, LAST_NAME FROM \"{csv}\" WHERE COUNTRY = 'Aruba'", alias="q1")

        assert _rows(db, "q1", 2) == [("Aimee", "Hodge")]

    def test_reuse_default_predicate_overwrites(self):
        """Reusing default predicate with overwrite=True keeps only last result."""
        csv = DATA_DIR / "customers-1.csv"
        db = bb.db()

        db.sql(f"SELECT FIRST_NAME, LAST_NAME FROM \"{csv}\" WHERE COUNTRY = 'Andorra'")
        db.sql(f"SELECT FIRST_NAME, LAST_NAME FROM \"{csv}\" WHERE COUNTRY = 'Aruba'")

        assert _rows(db, "query", 2) == [("Aimee", "Hodge")]

    def test_reuse_alias_no_overwrite(self):
        """With overwrite=False, results accumulate as before."""
        csv = DATA_DIR / "customers-1.csv"
        db = bb.db()

        db.sql(f"SELECT FIRST_NAME, LAST_NAME FROM \"{csv}\" WHERE COUNTRY = 'Andorra'", alias="q1", overwrite=False)
        db.sql(f"SELECT FIRST_NAME, LAST_NAME FROM \"{csv}\" WHERE COUNTRY = 'Aruba'", alias="q1", overwrite=False)

        assert _rows(db, "q1", 2) == [
            ("Aimee",    "Hodge"),
            ("Kristina", "Ferrell"),
        ]

    def test_four_aggregates_different_aliases(self):
        """Four aggregate queries on the same CSV, each stored under its own alias."""
        li = DATA_DIR / "tpch" / "lineitem_2.csv"
        db = bb.db()

        db.sql(f'SELECT COUNT(*) AS CNT       FROM "{li}"', alias="total_rows")
        db.sql(f'SELECT MIN(L_QUANTITY) AS V  FROM "{li}"', alias="min_qty")
        db.sql(f'SELECT MAX(L_QUANTITY) AS V  FROM "{li}"', alias="max_qty")
        db.sql(f'SELECT SUM(L_QUANTITY) AS V  FROM "{li}"', alias="sum_qty")

        assert _rows(db, "total_rows", 1) == [(99,)]
        assert _rows(db, "min_qty",    1) == [(1.0,)]
        assert _rows(db, "max_qty",    1) == [(50.0,)]
        [(total,)] = _rows(db, "sum_qty", 1)
        assert abs(total - 2624.0) < 1e-6

    def test_two_group_by_queries_different_aliases(self):
        """Two GROUP BY queries on the same table stored under different aliases."""
        li = DATA_DIR / "tpch" / "lineitem_2.csv"
        db = bb.db()

        db.sql(
            f'SELECT L_RETURNFLAG, COUNT(*) AS CNT FROM "{li}" GROUP BY L_RETURNFLAG',
            alias="flag_counts",
        )
        db.sql(
            f'SELECT L_RETURNFLAG, SUM(L_QUANTITY) AS TOTAL FROM "{li}" GROUP BY L_RETURNFLAG',
            alias="flag_sums",
        )

        assert _rows(db, "flag_counts", 2) == [("A", 24), ("N", 59), ("R", 16)]
        assert sorted(_rows(db, "flag_sums", 2)) == [("A", 651.0), ("N", 1564.0), ("R", 409.0)]

    def test_csv_then_parquet_filter(self):
        """A CSV filter run followed by a parquet filter run; both results accessible."""
        csv = DATA_DIR / "customers-1.csv"
        pq  = DATA_DIR / "data_decimal.parquet"
        db = bb.db()

        db.sql(f"SELECT FIRST_NAME, LAST_NAME FROM \"{csv}\" WHERE COUNTRY = 'Norway'",
               alias="norway")
        db.sql(f'SELECT COL4, COL1 FROM "{pq}" WHERE COL1 >= 98',
               alias="high_rows")

        assert _rows(db, "norway",    2) == [("Heather", "Callahan")]
        assert _rows(db, "high_rows", 2) == [
            ("row_100", 100.0),
            ("row_98",   98.0),
            ("row_99",   99.0),
        ]

    # --- IN / NOT IN with constants ---

    def test_in_constants_and_not_in_constants(self):
        """IN and NOT IN with constant lists produce complementary results."""
        csv = DATA_DIR / "customers-1.csv"
        db = bb.db()

        db.sql(
            f"SELECT FIRST_NAME, COUNTRY FROM \"{csv}\""
            f" WHERE COUNTRY IN ('Norway', 'Andorra', 'Aruba')",
            alias="in_result",
        )
        db.sql(
            f"SELECT COUNT(*) AS CNT FROM \"{csv}\""
            f" WHERE COUNTRY NOT IN ('Norway', 'Andorra', 'Aruba')",
            alias="not_in_result",
        )

        assert _rows(db, "in_result", 2) == [
            ("Aimee",    "Aruba"),
            ("Heather",  "Norway"),
            ("Kristina", "Andorra"),
        ]
        assert _rows(db, "not_in_result", 1) == [(96,)]

    def test_in_and_not_in_combined_in_same_where(self):
        """IN and NOT IN filters combined in the same WHERE clause."""
        li = DATA_DIR / "tpch" / "lineitem_2.csv"
        db = bb.db()

        db.sql(
            f"SELECT L_RETURNFLAG, COUNT(*) AS CNT FROM \"{li}\""
            f" WHERE L_RETURNFLAG IN ('A', 'R') AND L_LINESTATUS NOT IN ('O')"
            f" GROUP BY L_RETURNFLAG",
            alias="filtered_flags",
        )
        db.sql(
            f"SELECT COUNT(*) AS CNT FROM \"{li}\" WHERE L_RETURNFLAG NOT IN ('A', 'R')",
            alias="only_n",
        )

        assert _rows(db, "filtered_flags", 2) == [("A", 24), ("R", 16)]
        assert _rows(db, "only_n", 1) == [(59,)]

    # --- IN / NOT IN with subquery ---

    def test_in_subquery_no_agg(self):
        """IN with a non-aggregate subquery filtered by a constant."""
        csv = DATA_DIR / "customers-1.csv"
        db = bb.db()

        db.sql(
            f'SELECT FIRST_NAME, LAST_NAME FROM "{csv}" AS c'
            f' WHERE c.COUNTRY IN'
            f' (SELECT c2.COUNTRY FROM "{csv}" AS c2 WHERE c2.COUNTRY = \'Norway\')',
            alias="norway_sq",
        )
        db.sql(
            f"SELECT COUNT(*) AS CNT FROM \"{csv}\""
            f" WHERE COUNTRY IN ('Norway', 'Andorra', 'Aruba')",
            alias="three_countries",
        )

        assert _rows(db, "norway_sq",       2) == [("Heather", "Callahan")]
        assert _rows(db, "three_countries", 1) == [(3,)]

    def test_in_subquery_with_agg_and_not_in_subquery_with_agg(self):
        """IN / NOT IN whose subquery contains an aggregate (SELECT MAX(...))."""
        li = DATA_DIR / "tpch" / "lineitem_2.csv"
        db = bb.db()

        db.sql(
            f'SELECT COUNT(*) AS CNT FROM "{li}"'
            f' WHERE L_QUANTITY IN (SELECT MAX(L_QUANTITY) FROM "{li}")',
            alias="has_max",
        )
        db.sql(
            f'SELECT COUNT(*) AS CNT FROM "{li}"'
            f' WHERE L_QUANTITY NOT IN (SELECT MAX(L_QUANTITY) FROM "{li}")',
            alias="no_max",
        )

        assert _rows(db, "has_max", 1) == [(1,)]
        assert _rows(db, "no_max",  1) == [(98,)]

    # --- Aggregate queries ---

    def test_three_independent_aggregate_predicates(self):
        """Three aggregate queries (COUNT, SUM, MIN+MAX) each in its own predicate."""
        li = DATA_DIR / "tpch" / "lineitem_2.csv"
        db = bb.db()

        db.sql(f'SELECT COUNT(*) AS CNT FROM "{li}"', alias="row_count")
        db.sql(f'SELECT SUM(L_QUANTITY) AS TOTAL FROM "{li}"', alias="qty_sum")
        db.sql(f'SELECT MIN(L_QUANTITY) AS LO, MAX(L_QUANTITY) AS HI FROM "{li}"',
               alias="qty_range")

        assert _rows(db, "row_count", 1) == [(99,)]
        [(s,)] = _rows(db, "qty_sum", 1)
        assert abs(s - 2624.0) < 1e-6
        assert _rows(db, "qty_range", 2) == [(1.0, 50.0)]

    def test_aggregate_then_in_parquet(self):
        """Aggregate on parquet followed by IN filter on same parquet file."""
        pq = DATA_DIR / "data_decimal.parquet"
        db = bb.db()

        db.sql(
            f'SELECT COUNT(*) AS CNT FROM "{pq}"'
            f' WHERE COL1 NOT IN (SELECT MAX(COL1) FROM "{pq}")',
            alias="pq_no_max",
        )
        db.sql(
            f"SELECT COL4, COL1 FROM \"{pq}\" WHERE COL4 IN ('row_1', 'row_5', 'row_10')",
            alias="pq_in",
        )

        assert _rows(db, "pq_no_max", 1) == [(99,)]
        assert sorted(_rows(db, "pq_in", 2)) == [
            ("row_1",  1.0),
            ("row_10", 10.0),
            ("row_5",  5.0),
        ]

    # --- Datalog + SQL mixed runs ---

    def test_datalog_like_predicate_from_two_runs(self):
        """2 runs: Datalog facts queried via SQL predicate FROM with explicit arity + LIKE."""
        db = bb.db()

        db.run('product("apple", "fruit"). product("apricot", "fruit"). '
               'product("banana", "fruit"). product("bean", "veggie"). '
               'product("avocado", "fruit"). product("broccoli", "veggie"). product(X,Y)?')
        db.sql("SELECT COL_0 FROM product/2 WHERE COL_0 LIKE 'a%'", alias="a_products")

        assert sorted(_rows(db, "a_products", 1)) == [
            ("apple",), ("apricot",), ("avocado",),
        ]

    def test_datalog_predicate_no_arity_two_runs(self):
        """2 runs: SQL FROM without arity (auto-lookup from schema) + equality filter."""
        db = bb.db()

        db.run('city("Paris"). city("London"). city("Berlin"). city("Rome"). city(X)?')
        db.sql("SELECT COL_0 FROM city WHERE COL_0 = 'Paris'", alias="paris_only")

        assert _rows(db, "paris_only", 1) == [("Paris",)]

    def test_datalog_predicate_custom_cols_three_runs(self):
        """3 runs: Datalog facts, SQL FROM pred(X,Y) with custom column names, GROUP BY."""
        db = bb.db()

        db.run('person("Alice", "Smith"). person("Bob", "Jones"). '
               'person("Carol", "Smith"). person("Dave", "Jones"). '
               'person("Eve", "Brown"). person(X,Y)?')
        db.sql("SELECT FNAME, LNAME FROM person(FNAME, LNAME) WHERE LNAME = 'Smith'",
               alias="smiths")
        db.sql("SELECT COL_1, COUNT(*) AS CNT FROM person/2 GROUP BY COL_1",
               alias="last_name_counts")

        assert sorted(_rows(db, "smiths", 2)) == [("Alice", "Smith"), ("Carol", "Smith")]
        assert sorted(_rows(db, "last_name_counts", 2)) == [
            ("Brown", 1), ("Jones", 2), ("Smith", 2),
        ]

    def test_datalog_aggregate_like_three_runs(self):
        """3 runs: Datalog facts, custom column filter, LIKE aggregate."""
        db = bb.db()

        db.run('score("Alice", 85). score("Bob", 92). score("Carol", 78). '
               'score("Dave", 88). score("Eve", 91). score(X,Y)?')
        db.sql("SELECT STUDENT, GRADE FROM score(STUDENT, GRADE) WHERE GRADE > 88",
               alias="top_scores")
        db.sql("SELECT COUNT(*) AS CNT FROM score/2 WHERE COL_0 LIKE '%e'",
               alias="names_end_e")

        assert sorted(_rows(db, "top_scores", 2)) == [("Bob", 92), ("Eve", 91)]
        assert _rows(db, "names_end_e", 1) == [(3,)]

    def test_datalog_csv_parquet_four_runs(self):
        """4 runs: Datalog predicate query (auto-arity), CSV filter, Parquet LIKE."""
        csv = DATA_DIR / "customers-1.csv"
        pq  = DATA_DIR / "data_decimal.parquet"
        db = bb.db()

        db.run('score("Alice", 85). score("Bob", 92). score("Carol", 78). '
               'score("Dave", 88). score("Eve", 91). score(X,Y)?')
        db.sql("SELECT COL_0, COL_1 FROM score WHERE COL_1 > 88",        alias="top_scorers")
        db.sql(f'SELECT COUNT(*) AS CNT FROM "{csv}" WHERE COUNTRY = \'Norway\'',
               alias="norway_count")
        db.sql(f'SELECT COUNT(*) AS CNT FROM "{pq}" WHERE COL4 LIKE \'row_5%\'',
               alias="pq_5x_count")

        assert sorted(_rows(db, "top_scorers", 2)) == [("Bob", 92), ("Eve", 91)]
        assert _rows(db, "norway_count", 1) == [(1,)]
        assert _rows(db, "pq_5x_count",  1) == [(11,)]

    def test_all_features_five_runs(self):
        """5 runs: all three predicate FROM syntaxes + CSV GROUP BY + CSV NOT IN."""
        csv = DATA_DIR / "customers-1.csv"
        li  = DATA_DIR / "tpch" / "lineitem_2.csv"
        db = bb.db()

        db.run('item("apple", "fruit"). item("apricot", "fruit"). item("banana", "fruit"). '
               'item("bean", "veggie"). item("avocado", "fruit"). item("broccoli", "veggie"). '
               'item(X,Y)?')
        db.sql("SELECT COL_0, COL_1 FROM item/2 WHERE COL_0 LIKE 'b%'",
               alias="b_items")
        db.sql("SELECT NAME FROM item(NAME, CAT) WHERE CAT IN ('fruit') AND NAME LIKE 'a%'",
               alias="a_fruits")
        db.sql(f'SELECT L_RETURNFLAG, COUNT(*) AS CNT FROM "{li}" GROUP BY L_RETURNFLAG',
               alias="flag_counts")
        db.sql(
            f"SELECT COUNT(*) AS CNT FROM \"{csv}\""
            f" WHERE COUNTRY NOT IN ('Norway', 'Andorra', 'Aruba', 'Belize')",
            alias="other_count",
        )

        assert sorted(_rows(db, "b_items", 2)) == [
            ("banana", "fruit"), ("bean", "veggie"), ("broccoli", "veggie"),
        ]
        assert sorted(_rows(db, "a_fruits", 1)) == [("apple",), ("apricot",), ("avocado",)]
        assert sorted(_rows(db, "flag_counts", 2)) == [("A", 24), ("N", 59), ("R", 16)]
        assert _rows(db, "other_count", 1) == [(95,)]