import bumblebee
from conftest import _rows


class TestAggregates:
    def test_sum(self):
        db = bumblebee.db()
        db.run("""
            val(1). val(2). val(3). val(4). val(5).
            total(S) :- S = #sum{X:val(X)}.
            total(S) ?
        """)
        assert _rows(db, "total", 1) == [(15,)]

    def test_count(self):
        db = bumblebee.db()
        db.run("""
            item(a). item(b). item(c).
            n(C) :- C = #count{X:item(X)}.
            n(X)?
        """)
        assert _rows(db, "n", 1) == [(3,)]

    def test_min_max(self):
        db = bumblebee.db({"-a":""})
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
        db = bumblebee.db({"-d": "","-a":""})
        db.run("""
            sale(1, 10). sale(1, 20). sale(2, 5). sale(2, 15).
            total(G, S) :- sale(G,_), S = #sum{V:sale(G,V)}.
        """)
        assert _rows(db, "total", 2) == [(1, 30), (2, 20)]