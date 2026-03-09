"""
Shared helpers and fixtures for the bumblebeedb Python test suite.

Also adds the built bumblebeedb .so to sys.path.
Searches cmake-build-release first (preferred), then cmake-build-debug.
Run from any directory; the project root is resolved relative to this file.
"""
import sys
from pathlib import Path

_PROJECT_ROOT = Path(__file__).resolve().parents[2]

def _find_module_dir() -> Path:
    for build_dir in [ "cmake-build-release"]:
        candidate = _PROJECT_ROOT / build_dir
        if any(candidate.glob("bumblebeedb*.so")):
            return candidate
    raise RuntimeError(
        "bumblebeedb*.so not found under cmake-build-release. "
        "Run: cmake --build cmake-build-release --target bumblebeedb -j 8"
    )

_module_dir = _find_module_dir()
if str(_module_dir) not in sys.path:
    sys.path.insert(0, str(_module_dir))

DATA_DIR = Path(__file__).parent / "data"


def _rows(db, name, arity):
    """Return a sorted list of tuples for the given predicate."""
    return sorted(db.get_table(name, arity).tuples())


def _sql(query: str) -> str:
    """Helper to build a SQL program string."""
    return "%@sql\n" + query.strip()
