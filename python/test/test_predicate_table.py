import bumblebeedb as bb
from conftest import _rows


class TestPredicateTable:
    def test_isinstance(self):
        db = bb.db()
        db.run("fact(1). fact(2). fact(X)?")
        result = db.get_table("fact", 1)
        assert isinstance(result, bb.PredicateTable)

    def test_tuples_returns_list(self):
        db = bb.db()
        db.run("fact(1). fact(2). fact(3). fact(X)?")
        result = db.get_table("fact", 1)
        rows = result.tuples()
        assert isinstance(rows, list)
        assert sorted(rows) == [(1,), (2,), (3,)]

    def test_name_property(self):
        db = bb.db()
        db.run("mypred(42). mypred(X)?")
        result = db.get_table("mypred", 1)
        assert result.name == "mypred"

    def test_arity_property(self):
        db = bb.db()
        db.run("pair(1, 2). pair(3, 4). pair(X,Y)?")
        result = db.get_table("pair", 2)
        assert result.arity == 2

    def test_len(self):
        db = bb.db()
        db.run("item(a). item(b). item(c). item(d). item(X)?")
        result = db.get_table("item", 1)
        assert len(result) == 4

    def test_lazy_materialization(self):
        """tuples() should work correctly when called multiple times."""
        db = bb.db()
        db.run("v(10). v(20). v(X)?")
        result = db.get_table("v", 1)
        first = sorted(result.tuples())
        second = sorted(result.tuples())
        assert first == second == [(10,), (20,)]