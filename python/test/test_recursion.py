import bumblebeedb as bb
from conftest import _rows


class TestRecursion:
    def test_transitive_closure(self):
        db = bb.db()
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
        db = bb.db()
        db.run("""
            edge(1,2). edge(2,3). edge(3,4).
            reach(X,Y) :- edge(X,Y).
            reach(X,Z) :- reach(X,Y), edge(Y,Z).
            reach(X,Y)?
        """)
        assert len(_rows(db, "reach", 2)) == 6