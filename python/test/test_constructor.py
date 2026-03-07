import bumblebee
from conftest import _rows


class TestConstructor:
    def test_no_args(self):
        db = bumblebee.db()
        db.run("fact(1).fact(X)?")
        assert _rows(db, "fact", 1) == [(1,)]

    def test_threads_arg(self):
        db = bumblebee.db({"-t": "2", "-a":""})
        db.run("fact(42).")
        assert _rows(db, "fact", 1) == [(42,)]

    def test_distinct_arg(self):
        db = bumblebee.db({"-d": "", "-a":""})
        db.run("out(1). out(1). out(2).")
        # with -d all predicates are distinct → duplicates removed
        assert sorted(_rows(db, "out", 1)) == [(1,), (2,)]