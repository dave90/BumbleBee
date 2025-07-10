import time
import pytest
import os
from pathlib import Path
from test.e2e.utils import run_process_on_file, compare_files


EXE_PATH = os.path.join("..","cmake-build-debug","BumbleBee")

# Main pytest function
@pytest.mark.parametrize("input_file", list(Path(os.path.join("files","asp","input")).glob("*")))
def test_asp(input_file: Path):
    input_folder = Path(os.path.join("files","asp","input"))
    expected_folder = Path(os.path.join("files","asp","expected"))
    actual_folder = Path(os.path.join("files","asp","actual"))
    actual_folder.mkdir(exist_ok=True)

    output_file = actual_folder / input_file.name
    expected_file = expected_folder / input_file.name

    run_process_on_file(EXE_PATH, ["-i"], input_file, output_file)

    assert expected_file.exists(), f"Expected file missing: {expected_file}"
    assert output_file.exists(), f"Output file missing: {output_file}"
    assert compare_files(output_file, expected_file), f"Files do not match: {output_file} vs {expected_file}"