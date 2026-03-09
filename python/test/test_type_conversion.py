import bumblebeedb as bb
from conftest import _rows


class TestTypeConversion:
    def test_int_type(self):
        db = bb.db()
        db.run("v(42). v(X)?")
        [(val,)] = _rows(db, "v", 1)
        assert isinstance(val, int)
        assert val == 42

    def test_float_type(self):
        db = bb.db()
        db.run("v(3.14). v(X)?")
        [(val,)] = _rows(db, "v", 1)
        assert isinstance(val, float)
        assert abs(val - 3.14) < 1e-6

    def test_string_type(self):
        db = bb.db()
        db.run('v("hello"). v(X)?')
        [(val,)] = _rows(db, "v", 1)
        assert isinstance(val, str)
        assert val == "hello"

    def test_negative_int(self):
        db = bb.db()
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
        db = bb.db()
        db.run(facts + " row(X)?")
        rows = _rows(db, "row", 1)
        assert len(rows) == n
        assert rows[0] == (0,)
        assert rows[-1] == (n - 1,)