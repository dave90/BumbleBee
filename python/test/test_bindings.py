"""
pytest suite for the bumblebee Python bindings (python/src/bindings.cpp).

Run from the project root:
    pytest python/test/

Or from inside python/test/:
    pytest
"""
import pytest
from pathlib import Path
import bumblebee

DATA_DIR = Path(__file__).parent / "data"

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def _rows(db, name, arity):
    """Return a sorted list of tuples for the given predicate."""
    return sorted(db.get_table(name, arity).tuples())




# ---------------------------------------------------------------------------
# Constructor and basic API
# ---------------------------------------------------------------------------

class TestConstructor:
    def test_no_args(self):
        db = bumblebee.BumbleBeeDB()
        db.run("fact(1).fact(X)?")
        assert _rows(db, "fact", 1) == [(1,)]

    def test_threads_arg(self):
        db = bumblebee.BumbleBeeDB({"-t": "2", "-a":""})
        db.run("fact(42).")
        assert _rows(db, "fact", 1) == [(42,)]

    def test_distinct_arg(self):
        db = bumblebee.BumbleBeeDB({"-d": "", "-a":""})
        db.run("out(1). out(1). out(2).")
        # with -d all predicates are distinct → duplicates removed
        assert sorted(_rows(db, "out", 1)) == [(1,), (2,)]


# ---------------------------------------------------------------------------
# Facts (EDB)
# ---------------------------------------------------------------------------

class TestFacts:
    def test_integer_facts(self):
        db = bumblebee.BumbleBeeDB()
        db.run("p(1,2). p(3,4). p(5,6). p(X,Y)?")
        assert _rows(db, "p", 2) == [(1, 2), (3, 4), (5, 6)]

    def test_string_facts(self):
        db = bumblebee.BumbleBeeDB({"-a":""})
        db.run('name("alice"). name("bob"). name("carol").')
        assert _rows(db, "name", 1) == [("alice",), ("bob",), ("carol",)]

    def test_mixed_types(self):
        db = bumblebee.BumbleBeeDB({"-a":""})
        db.run('row(1, "hello", 3). row(2, "world", 4).')
        assert _rows(db, "row", 3) == [(1, "hello", 3), (2, "world", 4)]

    def test_float_facts(self):
        db = bumblebee.BumbleBeeDB({"-a":""})
        db.run("val(1.5). val(2.75).")
        rows = _rows(db, "val", 1)
        assert rows == [(1.5,), (2.75,)]


    def test_unary_fact(self):
        db = bumblebee.BumbleBeeDB({"-a":""})
        db.run("item(x). item(y). item(z).")
        assert _rows(db, "item", 1) == [("x",), ("y",), ("z",)]


# ---------------------------------------------------------------------------
# Derived rules (IDB)
# ---------------------------------------------------------------------------

class TestRules:
    def test_simple_rule(self):
        db = bumblebee.BumbleBeeDB()
        db.run("""
            b(1). b(2). b(3).
            a(X) :- b(X).
            a(X) ?
        """)
        assert _rows(db, "a", 1) == [(1,), (2,), (3,)]

    def test_filter_rule(self):
        db = bumblebee.BumbleBeeDB()
        db.run("""
            b(1). b(2). b(3). b(4). b(5).
            a(X) :- b(X), X > 2.
            a(X)?
        """)
        assert _rows(db, "a", 1) == [(3,), (4,), (5,)]

    def test_arithmetic_rule(self):
        db = bumblebee.BumbleBeeDB()
        db.run("""
            b(1). b(2). b(3).
            a(X, Y) :- b(X), Y = X + 10.
            a(X,Y)?
        """)
        assert _rows(db, "a", 2) == [(1, 11), (2, 12), (3, 13)]

    def test_join_rule(self):
        db = bumblebee.BumbleBeeDB({"-a":""})
        db.run("""
            p(1, "a"). p(2, "b").
            q("a", 10). q("b", 20).
            r(X, Z) :- p(X, Y), q(Y, Z).
        """)
        assert _rows(db, "r", 2) == [(1, 10), (2, 20)]

    def test_chain_rules(self):
        db = bumblebee.BumbleBeeDB({"-a":""})
        db.run("""
            a(1). a(2). a(3).
            b(X) :- a(X), X >= 2.
            c(X) :- b(X), X >= 3.
        """)
        assert _rows(db, "b", 1) == [(2,), (3,)]
        assert _rows(db, "c", 1) == [(3,)]


# ---------------------------------------------------------------------------
# Recursion
# ---------------------------------------------------------------------------

class TestRecursion:
    def test_transitive_closure(self):
        db = bumblebee.BumbleBeeDB()
        db.run("""
            edge(1,2). edge(2,3). edge(3,4).
            reach(X,Y) :- edge(X,Y).
            reach(X,Z) :- reach(X,Y), edge(Y,Z).
            reach(X,Y)?
        """)
        rows = _rows(db, "reach", 2)
        assert (1, 2) in rows
        assert (1, 3) in rows
        assert (1, 4) in rows
        assert (2, 3) in rows
        assert (2, 4) in rows
        assert (3, 4) in rows

    def test_path_count(self):
        """reach should have exactly 6 tuples for a 4-node chain."""
        db = bumblebee.BumbleBeeDB()
        db.run("""
            edge(1,2). edge(2,3). edge(3,4).
            reach(X,Y) :- edge(X,Y).
            reach(X,Z) :- reach(X,Y), edge(Y,Z).
            reach(X,Y)?
        """)
        assert len(_rows(db, "reach", 2)) == 6


# ---------------------------------------------------------------------------
# Aggregates
# ---------------------------------------------------------------------------

class TestAggregates:
    def test_sum(self):
        db = bumblebee.BumbleBeeDB()
        db.run("""
            val(1). val(2). val(3). val(4). val(5).
            total(S) :- S = #sum{X:val(X)}.
            total(S) ?
        """)
        assert _rows(db, "total", 1) == [(15,)]

    def test_count(self):
        db = bumblebee.BumbleBeeDB()
        db.run("""
            item(a). item(b). item(c).
            n(C) :- C = #count{X:item(X)}.
            n(X)?
        """)
        assert _rows(db, "n", 1) == [(3,)]

    def test_min_max(self):
        db = bumblebee.BumbleBeeDB({"-a":""})
        db.run("""
            val(3). val(1). val(4). val(1). val(5).
            lo(M) :- M = #min{X:val(X)}.
            hi(M) :- M = #max{X:val(X)}.
        """)
        assert _rows(db, "lo", 1) == [(1,)]
        assert _rows(db, "hi", 1) == [(5,)]

    def test_group_sum(self):
        # Use -d so the aggregate result per group is deduplicated.
        # Without distinct, the source fact per group fires the aggregate
        # once per matching row, producing duplicates.
        db = bumblebee.BumbleBeeDB({"-d": "","-a":""})
        db.run("""
            sale(1, 10). sale(1, 20). sale(2, 5). sale(2, 15).
            total(G, S) :- sale(G,_), S = #sum{V:sale(G,V)}.
        """)
        assert _rows(db, "total", 2) == [(1, 30), (2, 20)]


# ---------------------------------------------------------------------------
# API surface
# ---------------------------------------------------------------------------

class TestAPI:
    def test_get_output_predicates_returns_list(self):
        db = bumblebee.BumbleBeeDB({"-a":""})
        db.run("pred_a(1). pred_b(2,3). pred_c(4,5,6).")
        predicates = db.get_output_predicates()
        assert isinstance(predicates, list)
        # Each entry is a (name, arity) tuple.
        # Use subset: the singleton Catalog may retain predicates from
        # previous tests if pytest holds references via tracebacks.
        assert {("pred_a", 1), ("pred_b", 2), ("pred_c", 3)}.issubset(set(predicates))


    def test_get_table_by_name_only(self):
        """get_table without arity should find the predicate by name alone."""
        db = bumblebee.BumbleBeeDB({"-a":""})
        db.run("fact(1). fact(2). fact(3).")
        assert sorted(db.get_table("fact").tuples()) == [(1,), (2,), (3,)]

    def test_get_table_wrong_predicate_raises(self):
        db = bumblebee.BumbleBeeDB()
        db.run("fact(1).")
        with pytest.raises(RuntimeError, match="Predicate not found"):
            db.get_table("nonexistent", 1)

    def test_get_table_wrong_arity_raises(self):
        db = bumblebee.BumbleBeeDB({"-a":""})
        db.run("fact(1).")
        with pytest.raises(RuntimeError, match="Predicate not found"):
            db.get_table("fact", 2)  # fact has arity 1, not 2

    def test_empty_result(self):
        """A rule whose body is never satisfied returns empty."""
        db = bumblebee.BumbleBeeDB({"-a":""})
        db.run("""
            b(1). b(2).
            a(X) :- b(X), X > 100.
        """)
        assert _rows(db, "a", 1) == []

    def test_internal_predicates_not_exposed(self):
        """Aggregate helper predicates (#AGG...) must not appear in results."""
        db = bumblebee.BumbleBeeDB({"-a":""})
        db.run("""
            val(1). val(2). val(3).
            total(S) :- S = #sum{X:val(X)}.
        """)
        predicates = db.get_output_predicates()
        for name, _ in predicates:
            assert not name.startswith("#"), f"Internal predicate exposed: {name}"

    def test_run_twice_different_rules_same_head(self):
        """Second run accumulates into the same head predicate (data not cleared)."""
        db = bumblebee.BumbleBeeDB()
        db.run("base(1). base(2). base(3). out(X) :- base(X), X > 1. out(X)?")
        assert sorted(db.get_table("out").tuples()) == [(2,), (3,)]
        db.run("src(10). src(20). out(X) :- src(X), X < 20. out(X)?")
        # out now contains results from both runs
        assert sorted(db.get_table("out").tuples()) == [(2,), (3,), (10,)]

    def test_recreate_db_clears_previous_results(self):
        """Recreating the db instance clears previous results; second run yields only its own output.
        Note: del is required before reassigning to ensure the old instance (and its catalog)
        is fully destroyed before the new one is constructed."""
        db = bumblebee.BumbleBeeDB()
        db.run("base(1). base(2). base(3). out(X) :- base(X), X > 1. out(X)?")
        assert sorted(db.get_table("out").tuples()) == [(2,), (3,)]
        del db  # drop catalog before constructing new instance
        db = bumblebee.BumbleBeeDB()
        db.run("src(10). src(20). out(X) :- src(X), X < 20. out(X)?")
        assert sorted(db.get_table("out").tuples()) == [(10,)]

    def test_run_twice_returns_new_results(self):
        """A second run accumulates into the same predicate (data is not cleared between runs)."""
        db = bumblebee.BumbleBeeDB()
        db.run("out(1). out(2). out(X)?")
        assert sorted(db.get_table("out").tuples()) == [(1,), (2,)]
        db.run("out(10). out(20). out(30). out(X)?")
        assert sorted(db.get_table("out").tuples()) == [(1,), (2,), (10,), (20,), (30,)]

    def test_docstring_accessible(self):
        assert bumblebee.BumbleBeeDB.__doc__ is not None


# ---------------------------------------------------------------------------
# run_file
# ---------------------------------------------------------------------------

class TestRunFile:
    def test_run_file_basic(self, tmp_path):
        prog = tmp_path / "prog.dl"
        prog.write_text("out(10). out(20). out(30). out(X)?\n")
        db = bumblebee.BumbleBeeDB()
        db.run_file(str(prog))
        assert _rows(db, "out", 1) == [(10,), (20,), (30,)]

    def test_run_file_with_rules(self, tmp_path):
        prog = tmp_path / "prog.dl"
        prog.write_text(
            "num(1). num(2). num(3).\n"
            "big(X) :- num(X), X > 1.\n"
            "big(X)?\n"
        )
        db = bumblebee.BumbleBeeDB()
        db.run_file(str(prog))
        assert _rows(db, "big", 1) == [(2,), (3,)]


# ---------------------------------------------------------------------------
# Type conversion
# ---------------------------------------------------------------------------

class TestTypeConversion:
    def test_int_type(self):
        db = bumblebee.BumbleBeeDB()
        db.run("v(42). v(X)?")
        [(val,)] = _rows(db, "v", 1)
        assert isinstance(val, int)
        assert val == 42

    def test_float_type(self):
        db = bumblebee.BumbleBeeDB()
        db.run("v(3.14). v(X)?")
        [(val,)] = _rows(db, "v", 1)
        assert isinstance(val, float)
        assert abs(val - 3.14) < 1e-6

    def test_string_type(self):
        db = bumblebee.BumbleBeeDB()
        db.run('v("hello"). v(X)?')
        [(val,)] = _rows(db, "v", 1)
        assert isinstance(val, str)
        assert val == "hello"

    def test_negative_int(self):
        db = bumblebee.BumbleBeeDB()
        db.run("""
            base(-5). base(-3). base(0).
            out(X) :- base(X).
            out(X)?
        """)
        assert _rows(db, "out", 1) == [(-5,), (-3,), (0,)]

    def test_large_dataset(self):
        """Verify batched scan works (>STANDARD_VECTOR_SIZE rows)."""
        n = 2048  # > 1024 (typical STANDARD_VECTOR_SIZE)
        facts = " ".join(f"row({i})." for i in range(n))
        db = bumblebee.BumbleBeeDB()
        db.run(facts + " row(X)?")
        rows = _rows(db, "row", 1)
        assert len(rows) == n
        assert rows[0] == (0,)
        assert rows[-1] == (n - 1,)


# ---------------------------------------------------------------------------
# PredicateTable class
# ---------------------------------------------------------------------------

class TestPredicateTable:
    def test_isinstance(self):
        db = bumblebee.BumbleBeeDB()
        db.run("fact(1). fact(2). fact(X)?")
        result = db.get_table("fact", 1)
        assert isinstance(result, bumblebee.PredicateTable)

    def test_tuples_returns_list(self):
        db = bumblebee.BumbleBeeDB()
        db.run("fact(1). fact(2). fact(3). fact(X)?")
        result = db.get_table("fact", 1)
        rows = result.tuples()
        assert isinstance(rows, list)
        assert sorted(rows) == [(1,), (2,), (3,)]

    def test_name_property(self):
        db = bumblebee.BumbleBeeDB()
        db.run("mypred(42). mypred(X)?")
        result = db.get_table("mypred", 1)
        assert result.name == "mypred"

    def test_arity_property(self):
        db = bumblebee.BumbleBeeDB()
        db.run("pair(1, 2). pair(3, 4). pair(X,Y)?")
        result = db.get_table("pair", 2)
        assert result.arity == 2

    def test_len(self):
        db = bumblebee.BumbleBeeDB()
        db.run("item(a). item(b). item(c). item(d). item(X)?")
        result = db.get_table("item", 1)
        assert len(result) == 4

    def test_lazy_materialization(self):
        """tuples() should work correctly when called multiple times."""
        db = bumblebee.BumbleBeeDB()
        db.run("v(10). v(20). v(X)?")
        result = db.get_table("v", 1)
        first = sorted(result.tuples())
        second = sorted(result.tuples())
        assert first == second == [(10,), (20,)]


# Helper to build a SQL program string cleanly.
def _sql(query: str) -> str:
    return "%@sql\n" + query.strip()


# ---------------------------------------------------------------------------
# SQL queries — CSV sources
# ---------------------------------------------------------------------------

class TestSQLCSV:
    """SQL queries whose source data comes from CSV files.

    The result predicate is always named ``query`` (the default).
    All column names are normalised to UPPER_CASE by BumbleBee's CSV reader.
    """

    def test_count_rows(self):
        """COUNT(*) over customers-1.csv returns the row count."""
        csv = DATA_DIR / "customers-1.csv"
        db = bumblebee.BumbleBeeDB()
        db.run(_sql(f"""
            SELECT COUNT(*) AS CNT
            FROM "{csv}"
        """))
        assert _rows(db, "query", 1) == [(99,)]

    def test_where_filter(self):
        """Filter rows by a string equality condition."""
        csv = DATA_DIR / "customers-1.csv"
        db = bumblebee.BumbleBeeDB()
        db.run(_sql(f"""
            SELECT FIRST_NAME, LAST_NAME
            FROM "{csv}"
            WHERE COUNTRY = 'Norway'
        """))
        assert _rows(db, "query", 2) == [("Heather", "Callahan")]

    def test_group_by_count(self):
        """GROUP BY with COUNT(*) produces one row per group."""
        csv = DATA_DIR / "tpch" / "lineitem_2.csv"
        db = bumblebee.BumbleBeeDB()
        db.run(_sql(f"""
            SELECT L_RETURNFLAG, COUNT(*) AS CNT
            FROM "{csv}"
            GROUP BY L_RETURNFLAG
        """))
        rows = _rows(db, "query", 2)
        assert rows == [("A", 24), ("N", 59), ("R", 16)]

    def test_sum_aggregate(self):
        """SUM aggregate over a numeric column."""
        csv = DATA_DIR / "tpch" / "lineitem_2.csv"
        db = bumblebee.BumbleBeeDB()
        db.run(_sql(f"""
            SELECT SUM(L_QUANTITY) AS TOTAL
            FROM "{csv}"
        """))
        [(total,)] = _rows(db, "query", 1)
        assert abs(total - 2624.0) < 1e-6

    def test_user_alias(self):
        """Wrapping the query in (SELECT ...) AS name uses that name as result predicate."""
        csv = DATA_DIR / "tpch" / "lineitem_2.csv"
        db = bumblebee.BumbleBeeDB()
        db.run(_sql(f"""
            (SELECT L_RETURNFLAG, COUNT(*) AS CNT
             FROM "{csv}"
             GROUP BY L_RETURNFLAG) AS stats
        """))
        # predicate is "stats", not "query"
        rows = _rows(db, "stats", 2)
        assert rows == [("A", 24), ("N", 59), ("R", 16)]

    def test_select_multiple_columns(self):
        """SELECT with multiple projected columns returns correct arity."""
        csv = DATA_DIR / "customers-1.csv"
        db = bumblebee.BumbleBeeDB()
        db.run(_sql(f"""
            SELECT FIRST_NAME, LAST_NAME, COUNTRY
            FROM "{csv}"
            WHERE COUNTRY = 'Norway'
        """))
        assert _rows(db, "query", 3) == [("Heather", "Callahan", "Norway")]

    def test_multiple_runs(self):
        """Multiple runs of SQL query."""
        csv = DATA_DIR / "customers-1.csv"
        db = bumblebee.BumbleBeeDB()
        db.run(_sql(f"""(
                SELECT FIRST_NAME, LAST_NAME, COUNTRY
                FROM "{csv}"
                WHERE COUNTRY = 'Norway'
                ) AS query1
            """))
        assert _rows(db, "query1", 3) == [("Heather", "Callahan", "Norway")]
        db.run(_sql(f"""(
                SELECT FIRST_NAME, LAST_NAME, COUNTRY
                FROM "{csv}"
                WHERE COUNTRY = 'Papua New Guinea'
                ) AS query2
        """))
        assert _rows(db, "query2", 3) == [("Alfred", "Hooper", "Papua New Guinea")]


# ---------------------------------------------------------------------------
# Multiple SQL runs on the same db object
# ---------------------------------------------------------------------------

class TestSQLMultipleRuns:
    """Run several SQL queries on the same BumbleBeeDB instance and verify
    that each result predicate is independently accessible and that
    the default ``query`` predicate accumulates across runs."""

    def test_two_aliases_independent(self):
        """Two runs with distinct aliases produce two independent predicates."""
        csv = DATA_DIR / "customers-1.csv"
        li  = DATA_DIR / "tpch" / "lineitem_2.csv"
        db = bumblebee.BumbleBeeDB()

        db.run(_sql(f"""
            (SELECT COUNT(*) AS CNT
             FROM "{csv}") AS customer_count
        """))
        db.run(_sql(f"""
            (SELECT COUNT(*) AS CNT
             FROM "{li}") AS lineitem_count
        """))

        assert _rows(db, "customer_count", 1) == [(99,)]
        assert _rows(db, "lineitem_count", 1) == [(99,)]

    def test_three_aliases_mixed_sources(self):
        """Three runs mixing CSV and parquet each land in their own predicate."""
        csv = DATA_DIR / "customers-1.csv"
        li  = DATA_DIR / "tpch" / "lineitem_2.csv"
        pq  = DATA_DIR / "data_decimal.parquet"
        db = bumblebee.BumbleBeeDB()

        db.run(_sql(f"""
            (SELECT COUNT(*) AS CNT FROM "{csv}") AS csv_count
        """))
        db.run(_sql(f"""
            (SELECT COUNT(*) AS CNT FROM "{li}") AS li_count
        """))
        db.run(_sql(f"""
            (SELECT COUNT(*) AS CNT FROM "{pq}") AS pq_count
        """))

        assert _rows(db, "csv_count", 1) == [(99,)]
        assert _rows(db, "li_count",  1) == [(99,)]
        assert _rows(db, "pq_count",  1) == [(100,)]

    def test_default_query_predicate_accumulates(self):
        """Three runs all use the default ``query`` predicate; rows accumulate."""
        csv = DATA_DIR / "customers-1.csv"
        db = bumblebee.BumbleBeeDB()

        db.run(_sql(f"""
            SELECT FIRST_NAME, LAST_NAME
            FROM "{csv}"
            WHERE COUNTRY = 'Andorra'
        """))
        db.run(_sql(f"""
            SELECT FIRST_NAME, LAST_NAME
            FROM "{csv}"
            WHERE COUNTRY = 'Aruba'
        """))
        db.run(_sql(f"""
            SELECT FIRST_NAME, LAST_NAME
            FROM "{csv}"
            WHERE COUNTRY = 'Belize'
        """))

        # All three single-row countries end up in the same predicate
        assert _rows(db, "query", 2) == [
            ("Aimee",    "Hodge"),
            ("Kristina", "Ferrell"),
            ("Larry",    "Newton"),
        ]

    def test_four_aggregates_different_aliases(self):
        """Four aggregate queries on the same CSV, each stored under its own alias."""
        li = DATA_DIR / "tpch" / "lineitem_2.csv"
        db = bumblebee.BumbleBeeDB()

        db.run(_sql(f"""
            (SELECT COUNT(*) AS CNT FROM "{li}") AS total_rows
        """))
        db.run(_sql(f"""
            (SELECT MIN(L_QUANTITY) AS V FROM "{li}") AS min_qty
        """))
        db.run(_sql(f"""
            (SELECT MAX(L_QUANTITY) AS V FROM "{li}") AS max_qty
        """))
        db.run(_sql(f"""
            (SELECT SUM(L_QUANTITY) AS V FROM "{li}") AS sum_qty
        """))

        assert _rows(db, "total_rows", 1) == [(99,)]
        assert _rows(db, "min_qty",    1) == [(1.0,)]
        assert _rows(db, "max_qty",    1) == [(50.0,)]

        [(total,)] = _rows(db, "sum_qty", 1)
        assert abs(total - 2624.0) < 1e-6

    def test_two_group_by_queries_different_aliases(self):
        """Two GROUP BY queries on the same table stored under different aliases."""
        li = DATA_DIR / "tpch" / "lineitem_2.csv"
        db = bumblebee.BumbleBeeDB()

        db.run(_sql(f"""
            (SELECT L_RETURNFLAG, COUNT(*) AS CNT
             FROM "{li}"
             GROUP BY L_RETURNFLAG) AS flag_counts
        """))
        db.run(_sql(f"""
            (SELECT L_RETURNFLAG, SUM(L_QUANTITY) AS TOTAL
             FROM "{li}"
             GROUP BY L_RETURNFLAG) AS flag_sums
        """))

        assert _rows(db, "flag_counts", 2) == [("A", 24), ("N", 59), ("R", 16)]
        flag_sums = _rows(db, "flag_sums", 2)
        assert sorted(flag_sums) == [("A", 651.0), ("N", 1564.0), ("R", 409.0)]

    def test_csv_then_parquet_filter(self):
        """A CSV filter run followed by a parquet filter run; both results accessible."""
        csv = DATA_DIR / "customers-1.csv"
        pq  = DATA_DIR / "data_decimal.parquet"
        db = bumblebee.BumbleBeeDB()

        db.run(_sql(f"""
            (SELECT FIRST_NAME, LAST_NAME
             FROM "{csv}"
             WHERE COUNTRY = 'Norway') AS norway
        """))
        db.run(_sql(f"""
            (SELECT COL4, COL1
             FROM "{pq}"
             WHERE COL1 >= 98) AS high_rows
        """))

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
        db = bumblebee.BumbleBeeDB()

        db.run(_sql(f"""
            (SELECT FIRST_NAME, COUNTRY
             FROM "{csv}"
             WHERE COUNTRY IN ('Norway', 'Andorra', 'Aruba')) AS in_result
        """))
        db.run(_sql(f"""
            (SELECT COUNT(*) AS CNT
             FROM "{csv}"
             WHERE COUNTRY NOT IN ('Norway', 'Andorra', 'Aruba')) AS not_in_result
        """))

        assert _rows(db, "in_result", 2) == [
            ("Aimee",    "Aruba"),
            ("Heather",  "Norway"),
            ("Kristina", "Andorra"),
        ]
        assert _rows(db, "not_in_result", 1) == [(96,)]

    def test_in_and_not_in_combined_in_same_where(self):
        """IN and NOT IN filters combined in the same WHERE clause."""
        li = DATA_DIR / "tpch" / "lineitem_2.csv"
        db = bumblebee.BumbleBeeDB()

        # flags A and R + linestatus not O → all A and R rows (they are all linestatus F)
        db.run(_sql(f"""
            (SELECT L_RETURNFLAG, COUNT(*) AS CNT
             FROM "{li}"
             WHERE L_RETURNFLAG IN ('A', 'R') AND L_LINESTATUS NOT IN ('O')
             GROUP BY L_RETURNFLAG) AS filtered_flags
        """))
        # complement: only flag N
        db.run(_sql(f"""
            (SELECT COUNT(*) AS CNT
             FROM "{li}"
             WHERE L_RETURNFLAG NOT IN ('A', 'R')) AS only_n
        """))

        assert _rows(db, "filtered_flags", 2) == [("A", 24), ("R", 16)]
        assert _rows(db, "only_n", 1) == [(59,)]

    # --- IN / NOT IN with subquery ---

    def test_in_subquery_no_agg(self):
        """IN with a non-aggregate subquery filtered by a constant."""
        csv = DATA_DIR / "customers-1.csv"
        db = bumblebee.BumbleBeeDB()

        db.run(_sql(f"""
            (SELECT FIRST_NAME, LAST_NAME
             FROM "{csv}" AS c
             WHERE c.COUNTRY IN (
               SELECT c2.COUNTRY
               FROM "{csv}" AS c2
               WHERE c2.COUNTRY = 'Norway'
             )) AS norway_sq
        """))
        # second run: simple IN with constants on the same db
        db.run(_sql(f"""
            (SELECT COUNT(*) AS CNT
             FROM "{csv}"
             WHERE COUNTRY IN ('Norway', 'Andorra', 'Aruba')) AS three_countries
        """))

        assert _rows(db, "norway_sq",      2) == [("Heather", "Callahan")]
        assert _rows(db, "three_countries", 1) == [(3,)]

    def test_in_subquery_with_agg_and_not_in_subquery_with_agg(self):
        """IN / NOT IN whose subquery contains an aggregate (SELECT MAX(...))."""
        li = DATA_DIR / "tpch" / "lineitem_2.csv"
        db = bumblebee.BumbleBeeDB()

        db.run(_sql(f"""
            (SELECT COUNT(*) AS CNT
             FROM "{li}"
             WHERE L_QUANTITY IN (SELECT MAX(L_QUANTITY) FROM "{li}")) AS has_max
        """))
        db.run(_sql(f"""
            (SELECT COUNT(*) AS CNT
             FROM "{li}"
             WHERE L_QUANTITY NOT IN (SELECT MAX(L_QUANTITY) FROM "{li}")) AS no_max
        """))

        # exactly 1 row has the maximum quantity; the rest do not
        assert _rows(db, "has_max", 1) == [(1,)]
        assert _rows(db, "no_max",  1) == [(98,)]

    # --- Aggregate queries ---

    def test_three_independent_aggregate_predicates(self):
        """Three aggregate queries (COUNT, SUM, MIN+MAX) each in its own predicate."""
        li = DATA_DIR / "tpch" / "lineitem_2.csv"
        db = bumblebee.BumbleBeeDB()

        db.run(_sql(f"""
            (SELECT COUNT(*) AS CNT FROM "{li}") AS row_count
        """))
        db.run(_sql(f"""
            (SELECT SUM(L_QUANTITY) AS TOTAL FROM "{li}") AS qty_sum
        """))
        db.run(_sql(f"""
            (SELECT MIN(L_QUANTITY) AS LO, MAX(L_QUANTITY) AS HI
             FROM "{li}") AS qty_range
        """))

        assert _rows(db, "row_count", 1) == [(99,)]
        [(s,)] = _rows(db, "qty_sum", 1)
        assert abs(s - 2624.0) < 1e-6
        # qty_range has arity 2: one row (min, max)
        assert _rows(db, "qty_range", 2) == [(1.0, 50.0)]

    def test_aggregate_then_in_parquet(self):
        """Aggregate on parquet followed by IN filter on same parquet file."""
        pq = DATA_DIR / "data_decimal.parquet"
        db = bumblebee.BumbleBeeDB()

        db.run(_sql(f"""
            (SELECT COUNT(*) AS CNT
             FROM "{pq}"
             WHERE COL1 NOT IN (SELECT MAX(COL1) FROM "{pq}")) AS pq_no_max
        """))
        db.run(_sql(f"""
            (SELECT COL4, COL1
             FROM "{pq}"
             WHERE COL4 IN ('row_1', 'row_5', 'row_10')) AS pq_in
        """))

        assert _rows(db, "pq_no_max", 1) == [(99,)]
        assert sorted(_rows(db, "pq_in", 2)) == [
            ("row_1",  1.0),
            ("row_10", 10.0),
            ("row_5",  5.0),
        ]


# ---------------------------------------------------------------------------
# SQL queries — Parquet sources
# ---------------------------------------------------------------------------

class TestSQLParquet:
    """SQL queries whose source data comes from Parquet files."""

    def test_count_rows(self):
        """COUNT(*) over a parquet file returns the correct row count."""
        pq = DATA_DIR / "data_decimal.parquet"
        db = bumblebee.BumbleBeeDB()
        db.run(_sql(f"""
            SELECT COUNT(*) AS CNT
            FROM "{pq}"
        """))
        assert _rows(db, "query", 1) == [(100,)]

    def test_filter_and_project(self):
        """Filter on a numeric column and project two columns."""
        pq = DATA_DIR / "data_decimal.parquet"
        db = bumblebee.BumbleBeeDB()
        db.run(_sql(f"""
            SELECT COL4, COL1
            FROM "{pq}"
            WHERE COL1 <= 3
        """))
        rows = _rows(db, "query", 2)
        # COL1 values 1.00, 2.00, 3.00 are returned as floats; COL4 is a string label
        assert sorted(rows) == [("row_1", 1.0), ("row_2", 2.0), ("row_3", 3.0)]

    def test_user_alias_parquet(self):
        """User-supplied alias works for parquet queries too."""
        pq = DATA_DIR / "data_decimal.parquet"
        db = bumblebee.BumbleBeeDB()
        db.run(_sql(f"""
            (SELECT COUNT(*) AS CNT
             FROM "{pq}") AS myresult
        """))
        assert _rows(db, "myresult", 1) == [(100,)]


