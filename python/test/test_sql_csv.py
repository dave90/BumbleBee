import bumblebeedb as bb
from conftest import _rows, _sql, DATA_DIR


class TestSQLCSV:
    """SQL queries whose source data comes from CSV files."""

    def test_count_rows(self):
        """COUNT(*) over customers-1.csv returns the row count."""
        csv = DATA_DIR / "customers-1.csv"
        db = bb.db()
        db.run(_sql(f"""
            SELECT COUNT(*) AS CNT
            FROM "{csv}"
        """))
        assert _rows(db, "query", 1) == [(99,)]

    def test_where_filter(self):
        """Filter rows by a string equality condition."""
        csv = DATA_DIR / "customers-1.csv"
        db = bb.db()
        db.run(_sql(f"""
            SELECT FIRST_NAME, LAST_NAME
            FROM "{csv}"
            WHERE COUNTRY = 'Norway'
        """))
        assert _rows(db, "query", 2) == [("Heather", "Callahan")]

    def test_group_by_count(self):
        """GROUP BY with COUNT(*) produces one row per group."""
        csv = DATA_DIR / "tpch" / "lineitem_2.csv"
        db = bb.db()
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
        db = bb.db()
        db.run(_sql(f"""
            SELECT SUM(L_QUANTITY) AS TOTAL
            FROM "{csv}"
        """))
        [(total,)] = _rows(db, "query", 1)
        assert abs(total - 2624.0) < 1e-6

    def test_user_alias(self):
        """Wrapping the query in (SELECT ...) AS name uses that name as result predicate."""
        csv = DATA_DIR / "tpch" / "lineitem_2.csv"
        db = bb.db()
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
        db = bb.db()
        db.run(_sql(f"""
            SELECT FIRST_NAME, LAST_NAME, COUNTRY
            FROM "{csv}"
            WHERE COUNTRY = 'Norway'
        """))
        assert _rows(db, "query", 3) == [("Heather", "Callahan", "Norway")]

    def test_multiple_runs(self):
        """Multiple runs of SQL query."""
        csv = DATA_DIR / "customers-1.csv"
        db = bb.db()
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