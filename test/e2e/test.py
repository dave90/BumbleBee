import time
import pytest
import os
from pathlib import Path
from test.e2e.utils import run_process_on_file, compare_files_no_duplicates, contains_query


EXE_PATH = os.path.join("..","..","cmake-build-debug","BumbleBee")
input_folder = Path(os.path.join("files","asp","input"))
expected_folder = Path(os.path.join("files","asp","expected"))
actual_folder = Path(os.path.join("files","asp","actual"))

input_files = [p for p in input_folder.rglob("*") if p.is_file()]

# Main pytest function
@pytest.mark.parametrize("input_file", input_files)
def test_asp(input_file: Path):
    actual_folder.mkdir(exist_ok=True)

    output_file = actual_folder / input_file.name
    expected_file = expected_folder / input_file.name

    args = ["-a", "-i"]
    if contains_query(str(input_file)):
        args = ["-i"]

    run_process_on_file(EXE_PATH, args, input_file, output_file)

    assert expected_file.exists(), f"Expected file missing: {expected_file}"
    assert output_file.exists(), f"Output file missing: {output_file}"
    assert compare_files_no_duplicates(output_file, expected_file), f"Files do not match: {output_file} vs {expected_file}"