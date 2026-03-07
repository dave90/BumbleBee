import pytest
import bumblebee
from conftest import _rows


class TestAPI:
    def test_get_output_predicates_returns_list(self):
        db = bumblebee.db({"-a":""})
        db.run("pred_a(1). pred_b(2,3). pred_c(4,5,6).")
        predicates = db.get_output_predicates()
        assert isinstance(predicates, list)
        assert {("pred_a", 1), ("pred_b", 2), ("pred_c", 3)}.issubset(set(predicates))

    def test_get_table_by_name_only(self):
        """get_table without arity should find the predicate by name alone."""
        db = bumblebee.db({"-a":""})
        db.run("fact(1). fact(2). fact(3).")
        assert sorted(db.get_table("fact").tuples()) == [(1,), (2,), (3,)]

    def test_get_table_wrong_predicate_raises(self):
        db = bumblebee.db()
        db.run("fact(1).")
        with pytest.raises(RuntimeError, match="Predicate not found"):
            db.get_table("nonexistent", 1)

    def test_get_table_wrong_arity_raises(self):
        db = bumblebee.db({"-a":""})
        db.run("fact(1).")
        with pytest.raises(RuntimeError, match="Predicate not found"):
            db.get_table("fact", 2)  # fact has arity 1, not 2

    def test_empty_result(self):
        """A rule whose body is never satisfied returns empty."""
        db = bumblebee.db({"-a":""})
        db.run("""
            b(1). b(2).
            a(X) :- b(X), X > 100.
        """)
        assert _rows(db, "a", 1) == []

    def test_internal_predicates_not_exposed(self):
        """Aggregate helper predicates (#AGG...) must not appear in results."""
        db = bumblebee.db({"-a":""})
        db.run("""
            val(1). val(2). val(3).
            total(S) :- S = #sum{X:val(X)}.
        """)
        predicates = db.get_output_predicates()
        for name, _ in predicates:
            assert not name.startswith("#"), f"Internal predicate exposed: {name}"

    def test_run_twice_different_rules_same_head(self):
        """Second run accumulates into the same head predicate (data not cleared)."""
        db = bumblebee.db()
        db.run("base(1). base(2). base(3). out(X) :- base(X), X > 1. out(X)?")
        assert sorted(db.get_table("out").tuples()) == [(2,), (3,)]
        db.run("src(10). src(20). out(X) :- src(X), X < 20. out(X)?")
        # out now contains results from both runs
        assert sorted(db.get_table("out").tuples()) == [(2,), (3,), (10,)]

    def test_recreate_db_clears_previous_results(self):
        """Recreating the db instance clears previous results; second run yields only its own output.
        Note: del is required before reassigning to ensure the old instance (and its catalog)
        is fully destroyed before the new one is constructed."""
        db = bumblebee.db()
        db.run("base(1). base(2). base(3). out(X) :- base(X), X > 1. out(X)?")
        assert sorted(db.get_table("out").tuples()) == [(2,), (3,)]
        del db  # drop catalog before constructing new instance
        db = bumblebee.db()
        db.run("src(10). src(20). out(X) :- src(X), X < 20. out(X)?")
        assert sorted(db.get_table("out").tuples()) == [(10,)]

    def test_run_twice_returns_new_results(self):
        """A second run accumulates into the same predicate (data is not cleared between runs)."""
        db = bumblebee.db()
        db.run("out(1). out(2). out(X)?")
        assert sorted(db.get_table("out").tuples()) == [(1,), (2,)]
        db.run("out(10). out(20). out(30). out(X)?")
        assert sorted(db.get_table("out").tuples()) == [(1,), (2,), (10,), (20,), (30,)]

    def test_docstring_accessible(self):
        assert bumblebee.db.__doc__ is not None