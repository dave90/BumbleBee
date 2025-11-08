
import subprocess
from pathlib import Path
import filecmp
from typing import List
import os
import tempfile

# Utility function to run the exe process
def run_process_on_file(exe_path: str, args:List[str], input_file: Path, output_file: Path = None) -> None:
    """Run the executable with the input file and write the output to output_file."""
    # delete output file if exist
    if output_file and output_file.exists():
        output_file.unlink()

    result = subprocess.run(
        [exe_path]+ args + [str(input_file)],
        stdout=output_file.open("wb") if output_file else None,
        stderr=subprocess.PIPE,
        check=True
    )
    if result.returncode != 0:
        raise RuntimeError(f"Process failed: {result.stderr.decode()}")

# Utility function to compare two files
def compare_files_no_duplicates(file1: Path, file2: Path) -> bool:
    """Compare two files after sorting their lines."""
    with file1.open("r", encoding="utf-8") as f1, file2.open("r", encoding="utf-8") as f2:
        lines1 = sorted(set(f1.readlines()))
        lines2 = sorted((f2.readlines()))
    return lines1 == lines2

def compare_files(file1: Path, file2: Path) -> bool:
    """Compare two files after sorting their lines."""
    with file1.open("r", encoding="utf-8") as f1, file2.open("r", encoding="utf-8") as f2:
        lines1 = sorted(f1.readlines())
        lines2 = sorted(f2.readlines())
    return lines1 == lines2

# return true is containing a query
def contains_query(input_file:str):
    f = open(input_file, "r")
    lines = f.readlines()
    for line in lines:
        if "?" in line:
            return True
    return False

def create_tmp_input_file(input_file:Path, prefix:str, suffix:str, skip_lines=1) -> Path:
    temp_path = str(input_file) + ".tmp"
    print(temp_path)

    fi = open(input_file, "r")
    fo = open(temp_path, "w")

    idx = 0
    for line in fi.readlines():
        if idx == skip_lines:
            fo.write(prefix)
        fo.write(line)
        idx += 1

    fo.write(suffix)
    fo.close()
    return Path(temp_path)