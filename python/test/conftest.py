"""
Add the built bumblebee .so to sys.path.

Searches cmake-build-release first (preferred), then cmake-build-debug.
Run from any directory; the project root is resolved relative to this file.
"""
import sys
from pathlib import Path

_PROJECT_ROOT = Path(__file__).resolve().parents[2]

def _find_module_dir() -> Path:
    for build_dir in [ "cmake-build-release"]:
        candidate = _PROJECT_ROOT / build_dir
        if any(candidate.glob("bumblebee*.so")):
            return candidate
    raise RuntimeError(
        "bumblebee*.so not found under cmake-build-debug. "
        "Run: cmake --build cmake-build-release --target bumblebee -j 8"
    )

_module_dir = _find_module_dir()
if str(_module_dir) not in sys.path:
    sys.path.insert(0, str(_module_dir))
