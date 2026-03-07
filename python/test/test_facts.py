import bumblebee
from conftest import _rows


class TestFacts:
    def test_integer_facts(self):
        db = bumblebee.db()
        db.run("p(1,2). p(3,4). p(5,6). p(X,Y)?")
        assert _rows(db, "p", 2) == [(1, 2), (3, 4), (5, 6)]

    def test_string_facts(self):
        db = bumblebee.db({"-a":""})
        db.run('name("alice"). name("bob"). name("carol").')
        assert _rows(db, "name", 1) == [("alice",), ("bob",), ("carol",)]

    def test_mixed_types(self):
        db = bumblebee.db({"-a":""})
        db.run('row(1, "hello", 3). row(2, "world", 4).')
        assert _rows(db, "row", 3) == [(1, "hello", 3), (2, "world", 4)]

    def test_float_facts(self):
        db = bumblebee.db({"-a":""})
        db.run("val(1.5). val(2.75).")
        rows = _rows(db, "val", 1)
        assert rows == [(1.5,), (2.75,)]

    def test_unary_fact(self):
        db = bumblebee.db({"-a":""})
        db.run("item(x). item(y). item(z).")
        assert _rows(db, "item", 1) == [("x",), ("y",), ("z",)]