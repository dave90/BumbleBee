import pytest
import bumblebeedb as bb
from conftest import _rows


class TestAPI:
    def test_get_output_predicates_returns_list(self):
        db = bb.db({"-a":""})
        db.run("pred_a(1). pred_b(2,3). pred_c(4,5,6).")
        predicates = db.get_output_predicates()
        assert isinstance(predicates, list)
        assert {("pred_a", 1), ("pred_b", 2), ("pred_c", 3)}.issubset(set(predicates))

    def test_get_table_by_name_only(self):
        """get_table without arity should find the predicate by name alone."""
        db = bb.db({"-a":""})
        db.run("fact(1). fact(2). fact(3).")
        assert sorted(db.get_table("fact").tuples()) == [(1,), (2,), (3,)]

    def test_get_table_wrong_predicate_raises(self):
        db = bb.db()
        db.run("fact(1).")
        with pytest.raises(RuntimeError, match="Predicate not found"):
            db.get_table("nonexistent", 1)

    def test_get_table_wrong_arity_raises(self):
        db = bb.db({"-a":""})
        db.run("fact(1).")
        with pytest.raises(RuntimeError, match="Predicate not found"):
            db.get_table("fact", 2)  # fact has arity 1, not 2

    def test_empty_result(self):
        """A rule whose body is never satisfied returns empty."""
        db = bb.db({"-a":""})
        db.run("""
            b(1). b(2).
            a(X) :- b(X), X > 100.
        """)
        assert _rows(db, "a", 1) == []

    def test_internal_predicates_not_exposed(self):
        """Aggregate helper predicates (#AGG...) must not appear in results."""
        db = bb.db({"-a":""})
        db.run("""
            val(1). val(2). val(3).
            total(S) :- S = #sum{X:val(X)}.
        """)
        predicates = db.get_output_predicates()
        for name, _ in predicates:
            assert not name.startswith("#"), f"Internal predicate exposed: {name}"

    def test_run_twice_different_rules_same_head(self):
        """Second run accumulates into the same head predicate (data not cleared)."""
        db = bb.db()
        db.run("base(1). base(2). base(3). out(X) :- base(X), X > 1. out(X)?")
        assert sorted(db.get_table("out").tuples()) == [(2,), (3,)]
        db.run("src(10). src(20). out(X) :- src(X), X < 20. out(X)?")
        # out now contains results from both runs
        assert sorted(db.get_table("out").tuples()) == [(2,), (3,), (10,)]

    def test_recreate_db_clears_previous_results(self):
        """Recreating the db instance clears previous results; second run yields only its own output.
        Note: del is required before reassigning to ensure the old instance (and its catalog)
        is fully destroyed before the new one is constructed."""
        db = bb.db()
        db.run("base(1). base(2). base(3). out(X) :- base(X), X > 1. out(X)?")
        assert sorted(db.get_table("out").tuples()) == [(2,), (3,)]
        del db  # drop catalog before constructing new instance
        db = bb.db()
        db.run("src(10). src(20). out(X) :- src(X), X < 20. out(X)?")
        assert sorted(db.get_table("out").tuples()) == [(10,)]

    def test_run_twice_returns_new_results(self):
        """A second run accumulates into the same predicate (data is not cleared between runs)."""
        db = bb.db()
        db.run("out(1). out(2). out(X)?")
        assert sorted(db.get_table("out").tuples()) == [(1,), (2,)]
        db.run("out(10). out(20). out(30). out(X)?")
        assert sorted(db.get_table("out").tuples()) == [(1,), (2,), (10,), (20,), (30,)]

    def test_docstring_accessible(self):
        assert bb.db.__doc__ is not None

    def test_remove_table(self):
        """remove_table removes a predicate by name and arity."""
        db = bb.db({"-a": ""})
        db.run("a(1). a(2). b(X) :- a(X).")
        assert len([p for p in db.get_output_predicates() if p[0] == "b"]) == 1
        db.remove_table("b", 1)
        names = [p[0] for p in db.get_output_predicates()]
        assert "b" not in names
        assert "a" in names

    def test_remove_table_not_found_raises(self):
        """remove_table raises RuntimeError for a nonexistent predicate."""
        db = bb.db()
        db.run("a(1). a(X)?")
        with pytest.raises(RuntimeError, match="Predicate not found"):
            db.remove_table("nonexist", 1)

    def test_remove_table_wrong_arity_raises(self):
        """remove_table raises RuntimeError when the arity does not match."""
        db = bb.db({"-a": ""})
        db.run("a(1).")
        with pytest.raises(RuntimeError, match="Predicate not found"):
            db.remove_table("a", 2)

    def test_remove_table_negative_arity_raises(self):
        """remove_table raises ValueError on negative arity."""
        db = bb.db()
        with pytest.raises(Exception):
            db.remove_table("a", -1)

    def test_remove_table_then_reuse_name(self):
        """After removing a predicate, the same name can be redefined."""
        db = bb.db({"-a": ""})
        db.run("out(1). out(2).")
        assert sorted(db.get_table("out", 1).tuples()) == [(1,), (2,)]
        db.remove_table("out", 1)
        db.run("out(10). out(20). out(30).")
        assert sorted(db.get_table("out", 1).tuples()) == [(10,), (20,), (30,)]