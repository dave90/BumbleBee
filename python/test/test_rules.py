import bumblebeedb as bb
from conftest import _rows


class TestRules:
    def test_simple_rule(self):
        db = bb.db()
        db.run("""
            b(1). b(2). b(3).
            a(X) :- b(X).
            a(X) ?
        """)
        assert _rows(db, "a", 1) == [(1,), (2,), (3,)]

    def test_filter_rule(self):
        db = bb.db()
        db.run("""
            b(1). b(2). b(3). b(4). b(5).
            a(X) :- b(X), X > 2.
            a(X)?
        """)
        assert _rows(db, "a", 1) == [(3,), (4,), (5,)]

    def test_arithmetic_rule(self):
        db = bb.db()
        db.run("""
            b(1). b(2). b(3).
            a(X, Y) :- b(X), Y = X + 10.
            a(X,Y)?
        """)
        assert _rows(db, "a", 2) == [(1, 11), (2, 12), (3, 13)]

    def test_join_rule(self):
        db = bb.db({"-a":""})
        db.run("""
            p(1, "a"). p(2, "b").
            q("a", 10). q("b", 20).
            r(X, Z) :- p(X, Y), q(Y, Z).
        """)
        assert _rows(db, "r", 2) == [(1, 10), (2, 20)]

    def test_chain_rules(self):
        db = bb.db({"-a":""})
        db.run("""
            a(1). a(2). a(3).
            b(X) :- a(X), X >= 2.
            c(X) :- b(X), X >= 3.
        """)
        assert _rows(db, "b", 1) == [(2,), (3,)]
        assert _rows(db, "c", 1) == [(3,)]