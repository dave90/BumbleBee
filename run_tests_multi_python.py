#!/usr/bin/env python3
"""
Run the BumbleBee Python test suite across multiple Python versions using conda.

For each version, this script:
  1. Removes any existing conda env (bb_testXY)
  2. Creates a fresh env with the target Python
  3. Installs the package (pip install .) and test deps
  4. Runs pytest on python/test/test_examples.py
  5. Reports pass/fail per version
"""

import subprocess
import sys

PYTHON_VERSIONS = ["3.9", "3.10", "3.11", "3.12", "3.13"]
ENV_PREFIX = "bb_test"


def env_name(version: str) -> str:
    return f"{ENV_PREFIX}{version.replace('.', '')}"


def run(cmd: str, check: bool = True) -> int:
    print(f"\n>>> {cmd}")
    result = subprocess.run(cmd, shell=True)
    if check and result.returncode != 0:
        raise subprocess.CalledProcessError(result.returncode, cmd)
    return result.returncode


def main():
    results: dict[str, str] = {}

    for version in PYTHON_VERSIONS:
        name = env_name(version)
        print(f"\n{'='*60}")
        print(f"  Python {version}  (env: {name})")
        print(f"{'='*60}")

        try:
            # Remove existing env if present
            run(f"conda env remove -n {name} -y", check=False)

            # Create fresh env
            run(f"conda create -n {name} python={version} -y")

            # Install package + test deps
            run(f"conda run -n {name} pip install . pytest")

            # Run tests
            rc = run(
                f"conda run -n {name} pytest python/test/test_examples.py -v",
                check=False,
            )

            results[version] = "PASS" if rc == 0 else "FAIL"
        except subprocess.CalledProcessError:
            results[version] = "ERROR"

    # Cleanup: remove all conda envs
    print(f"\n{'='*60}")
    print("  Cleanup")
    print(f"{'='*60}")
    for version in PYTHON_VERSIONS:
        name = env_name(version)
        run(f"conda env remove -n {name} -y", check=False)

    # Summary
    print(f"\n{'='*60}")
    print("  Summary")
    print(f"{'='*60}")
    for version, status in results.items():
        marker = "✓" if status == "PASS" else "✗"
        print(f"  Python {version}: {marker} {status}")

    if any(s != "PASS" for s in results.values()):
        sys.exit(1)


if __name__ == "__main__":
    main()