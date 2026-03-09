import bumblebeedb as bb
from conftest import _rows


class TestRunFile:
    def test_run_file_basic(self, tmp_path):
        prog = tmp_path / "prog.dl"
        prog.write_text("out(10). out(20). out(30). out(X)?\n")
        db = bb.db()
        db.run_file(str(prog))
        assert _rows(db, "out", 1) == [(10,), (20,), (30,)]

    def test_run_file_with_rules(self, tmp_path):
        prog = tmp_path / "prog.dl"
        prog.write_text(
            "num(1). num(2). num(3).\n"
            "big(X) :- num(X), X > 1.\n"
            "big(X)?\n"
        )
        db = bb.db()
        db.run_file(str(prog))
        assert _rows(db, "big", 1) == [(2,), (3,)]