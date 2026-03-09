import time
import subprocess
import pytest
import os
from pathlib import Path
from test.e2e.utils import run_process_on_file, compare_files_no_duplicates, contains_query, compare_files, create_tmp_input_file, compare_csv

EXE_PATH = os.environ.get("BUMBLEBEE_EXE", os.path.join("..","..","cmake-build-debug","BumbleBee"))
EXE_PATH_ABS = str(Path(EXE_PATH).resolve())
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

    args = ["-a", "-t", "1", "-i"]
    if contains_query(str(input_file)):
        args = ["-t","1","-i"]

    run_process_on_file(EXE_PATH, args, input_file, output_file)

    assert expected_file.exists(), f"Expected file missing: {expected_file}"
    assert output_file.exists(), f"Output file missing: {output_file}"
    assert compare_files_no_duplicates(output_file, expected_file), f"Files do not match: {output_file} vs {expected_file}"

@pytest.mark.parametrize("input_file", input_files_asp, ids=_id)
def test_distinct_asp(input_file: Path):
    actual_folder_asp.mkdir(exist_ok=True)

    output_file = actual_folder_asp / input_file.name
    expected_file = expected_folder_asp / input_file.name

    args = ["-d","-t","1", "-a", "-i"]
    if contains_query(str(input_file)):
        args = ["-d","-t","1","-i"]

    print(args)

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

    args = ["-t", "1", "-i"]
    # add the export
    input_file = create_tmp_input_file(input_file, "COPY (",") TO \""+str(output_file)+".csv\" (single_file=1)")

    run_process_on_file(EXE_PATH, args, input_file)

    # remove .csv
    if os.path.exists(str(output_file)+".csv"):
        os.rename(str(output_file)+".csv", str(output_file))

    assert expected_file.exists(), f"Expected file missing: {expected_file}"
    assert compare_csv(str(output_file), str(expected_file)), f"Files do not match: {output_file} vs {expected_file}"
    # remove the tmp file as the test succeed
    os.remove(input_file)


# ── Examples ──────────────────────────────────────────────────────────────────
# Examples live in ../../examples/ and reference ./data/ relative to that dir,
# so BumbleBee must run with cwd = examples_dir.

examples_dir = Path(os.path.join("..", "..", "examples")).resolve()

# Collect DL examples (exclude 02_export)
examples_input_dl = sorted(
    [p for p in (examples_dir / "dl").rglob("*")
     if p.is_file() and "02_export" not in p.parts]
)
# Collect SQL examples (exclude 02_export and .tmp files)
examples_input_sql = sorted(
    [p for p in (examples_dir / "sql").rglob("*")
     if p.is_file() and "02_export" not in p.parts and not p.name.endswith(".tmp")]
)
# Collect export examples (both SQL and DL)
examples_export_files = sorted(
    [p for p in (examples_dir / "sql" / "02_export").rglob("*") if p.is_file()] +
    [p for p in (examples_dir / "dl" / "02_export").rglob("*") if p.is_file()]
)

def _examples_id(p: Path):
    try:
        return p.relative_to(examples_dir).as_posix()
    except ValueError:
        return p.as_posix()


def _create_example_sql_tmp(input_file: Path, output_csv: str) -> Path:
    """Wrap example SQL in COPY TO, handling % comment lines before %@sql."""
    lines = input_file.read_text().splitlines(keepends=True)
    sql_line_idx = None
    for i, line in enumerate(lines):
        if line.strip() == "%@sql":
            sql_line_idx = i
            break
    if sql_line_idx is None:
        raise ValueError(f"No %@sql found in {input_file}")

    tmp_path = str(input_file) + ".tmp"
    with open(tmp_path, "w") as f:
        for i in range(sql_line_idx + 1):
            f.write(lines[i])
        f.write("COPY (\n")
        for i in range(sql_line_idx + 1, len(lines)):
            f.write(lines[i])
        f.write(f') TO "{output_csv}" (single_file=1)\n')
    return Path(tmp_path)


@pytest.mark.parametrize("input_file", examples_input_dl, ids=_examples_id)
def test_examples_dl(input_file: Path):
    actual_dir = examples_dir / "actual" / "dl"
    actual_dir.mkdir(parents=True, exist_ok=True)

    rel_path = input_file.relative_to(examples_dir / "dl")
    output_file = actual_dir / rel_path.name
    expected_file = examples_dir / "expected" / "dl" / rel_path

    if output_file.exists():
        output_file.unlink()

    result = subprocess.run(
        [EXE_PATH_ABS, "-a", "-t", "1", "-i", str(input_file)],
        stdout=output_file.open("wb"),
        stderr=subprocess.PIPE,
        cwd=str(examples_dir),
        check=True,
    )

    assert expected_file.exists(), f"Expected file missing: {expected_file}"
    assert output_file.exists(), f"Output file missing: {output_file}"
    assert compare_files_no_duplicates(output_file, expected_file), \
        f"Files do not match: {output_file} vs {expected_file}"


@pytest.mark.parametrize("input_file", examples_input_sql, ids=_examples_id)
def test_examples_sql(input_file: Path):
    actual_dir = examples_dir / "actual" / "sql"
    actual_dir.mkdir(parents=True, exist_ok=True)

    rel_path = input_file.relative_to(examples_dir / "sql")
    output_csv = actual_dir / (rel_path.name + ".csv")
    final_output = actual_dir / rel_path.name
    expected_file = examples_dir / "expected" / "sql" / rel_path

    tmp_file = _create_example_sql_tmp(input_file, str(output_csv))

    result = subprocess.run(
        [EXE_PATH_ABS, "-t", "1", "-i", str(tmp_file)],
        stderr=subprocess.PIPE,
        cwd=str(examples_dir),
        check=True,
    )

    if output_csv.exists():
        output_csv.rename(final_output)

    assert expected_file.exists(), f"Expected file missing: {expected_file}"
    assert compare_csv(str(final_output), str(expected_file)), \
        f"Files do not match: {final_output} vs {expected_file}"
    tmp_file.unlink()


@pytest.mark.parametrize("input_file", examples_export_files, ids=_examples_id)
def test_examples_export(input_file: Path):
    output_dir = examples_dir / "output"
    output_dir.mkdir(exist_ok=True)

    result = subprocess.run(
        [EXE_PATH_ABS, "-a", "-t", "1", "-i", str(input_file)],
        stderr=subprocess.PIPE,
        cwd=str(examples_dir),
    )
    assert result.returncode == 0, \
        f"Export example failed: {input_file}\n{result.stderr.decode()}"

    # Clean up generated output files
    for f in output_dir.iterdir():
        if f.is_file():
            f.unlink()
