import bumblebee
from conftest import _rows, _sql, DATA_DIR


class TestSQLParquet:
    """SQL queries whose source data comes from Parquet files."""

    def test_count_rows(self):
        """COUNT(*) over a parquet file returns the correct row count."""
        pq = DATA_DIR / "data_decimal.parquet"
        db = bumblebee.db()
        db.run(_sql(f"""
            SELECT COUNT(*) AS CNT
            FROM "{pq}"
        """))
        assert _rows(db, "query", 1) == [(100,)]

    def test_filter_and_project(self):
        """Filter on a numeric column and project two columns."""
        pq = DATA_DIR / "data_decimal.parquet"
        db = bumblebee.db()
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
        db = bumblebee.db()
        db.run(_sql(f"""
            (SELECT COUNT(*) AS CNT
             FROM "{pq}") AS myresult
        """))
        assert _rows(db, "myresult", 1) == [(100,)]