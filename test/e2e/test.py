import time
import pytest
import os
from pathlib import Path
from test.e2e.utils import run_process_on_file, compare_files_no_duplicates, contains_query, compare_files, create_tmp_input_file, compare_csv


EXE_PATH = os.path.join("..","..","cmake-build-debug","BumbleBee")
# ASP
input_folder_asp = Path(os.path.join("files","asp","input"))
expected_folder_asp = Path(os.path.join("files","asp","expected"))
actual_folder_asp = Path(os.path.join("files","asp","actual"))
input_files_asp = [p for p in input_folder_asp.rglob("*") if p.is_file()]

# SQL
input_folder_sql = Path(os.path.join("files","sql","input"))
expected_folder_sql = Path(os.path.join("files","sql","expected"))
actual_folder_sql = Path(os.path.join("files","sql","actual"))
input_files_sql = [p for p in input_folder_sql.rglob("*") if p.is_file() and not p.name.endswith(".tmp")]


RERUN_MT = 4
THREAD = 4

# makes id friendly (pytest test.py::test_asp -q --collect-onl)
# so you can run test:  pytest  'test.py::test_asp[aggregate/agg.complex.5]'
def _id(p: Path):
    try:
        return p.relative_to(input_folder_asp).as_posix()
    except ValueError:
        return p.as_posix()

# Main pytest function
@pytest.mark.parametrize("input_file", input_files_asp, ids=_id)
def test_asp(input_file: Path):
    actual_folder_asp.mkdir(exist_ok=True)

    output_file = actual_folder_asp / input_file.name
    expected_file = expected_folder_asp / input_file.name

    args = ["-a", "-i"]
    if contains_query(str(input_file)):
        args = ["-i"]

    run_process_on_file(EXE_PATH, args, input_file, output_file)

    assert expected_file.exists(), f"Expected file missing: {expected_file}"
    assert output_file.exists(), f"Output file missing: {output_file}"
    assert compare_files_no_duplicates(output_file, expected_file), f"Files do not match: {output_file} vs {expected_file}"

@pytest.mark.parametrize("input_file", input_files_asp, ids=_id)
def test_distinct_asp(input_file: Path):
    actual_folder_asp.mkdir(exist_ok=True)

    output_file = actual_folder_asp / input_file.name
    expected_file = expected_folder_asp / input_file.name

    args = ["-d", "-a", "-i"]
    if contains_query(str(input_file)):
        args = ["-d","-i"]

    run_process_on_file(EXE_PATH, args, input_file, output_file)

    assert expected_file.exists(), f"Expected file missing: {expected_file}"
    assert output_file.exists(), f"Output file missing: {output_file}"
    assert compare_files(output_file, expected_file), f"Files do not match: {output_file} vs {expected_file}"


@pytest.mark.parametrize("input_file", input_files_asp, ids=_id)
def test_mt_asp(input_file: Path):
    actual_folder_asp.mkdir(exist_ok=True)

    # rerun multithread multiple times
    for i in range(RERUN_MT):
        output_file = actual_folder_asp / input_file.name
        expected_file = expected_folder_asp / input_file.name

        args = ["-a", "-t",str(THREAD), "-i"]
        if contains_query(str(input_file)):
            args = ["-t",str(THREAD),"-i"]

        run_process_on_file(EXE_PATH, args, input_file, output_file)

        assert expected_file.exists(), f"Expected file missing: {expected_file}"
        assert output_file.exists(), f"Output file missing: {output_file}"
        assert compare_files_no_duplicates(output_file, expected_file), f"Files do not match: {output_file} vs {expected_file}"




@pytest.mark.parametrize("input_file", input_files_sql)
def test_sql(input_file: Path):
    actual_folder_sql.mkdir(exist_ok=True)

    output_file = actual_folder_sql / input_file.name
    expected_file = expected_folder_sql / input_file.name

    args = ["-i"]
    # add the export
    input_file = create_tmp_input_file(input_file, "COPY (",") TO \""+str(output_file)+".csv\" (single_file=1)")

    run_process_on_file(EXE_PATH, args, input_file)

    # remove .csv
    if os.path.exists(str(output_file)+".csv"):
        os.rename(str(output_file)+".csv", str(output_file))

    assert expected_file.exists(), f"Expected file missing: {expected_file}"
    assert output_file.exists(), f"Output file missing: {output_file}"
    assert compare_csv(str(output_file), str(expected_file)), f"Files do not match: {output_file} vs {expected_file}"
    # remove the tmp file as the test succeed
    os.remove(input_file)
