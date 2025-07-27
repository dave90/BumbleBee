
import subprocess
from pathlib import Path
import filecmp
from typing import List
import os

# Utility function to run the exe process
def run_process_on_file(exe_path: str, args:List[str], input_file: Path, output_file: Path) -> None:
    """Run the executable with the input file and write the output to output_file."""
    # delete output file if exist
    if output_file.exists():
        output_file.unlink()

    result = subprocess.run(
        [exe_path]+ args + [str(input_file)],
        stdout=output_file.open("wb"),
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