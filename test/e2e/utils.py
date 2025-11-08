
import subprocess
from pathlib import Path
import filecmp
from typing import List
import pandas as pd

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



def compare_csv(output_file:str, expected_file:str) -> bool:
    """
    Compare two CSV files ignoring:
    - column order
    - row order

    Before comparison:
    - For columns from output_file: if the name contains '.', keep the second part (after the first dot)
    - For all columns (both CSVs): upper-case and replace spaces with '_' (e.g. 'My Column' -> 'MY_COLUMN')

    Returns True if the normalized DataFrames are equal, False otherwise.
    If they differ, prints the differences to help debugging.
    """
    # Helper to normalize any column name
    def _normalize_col(name: str) -> str:
        name = str(name).strip()
        return name.upper().replace(" ", "_")

    # Helper to normalize column names for the output CSV (apply dot rule)
    def _normalize_output_col(name: str) -> str:
        name = str(name)
        if "." in name:
            # keep the second part after the first dot
            parts = name.split(".", 1)
            if len(parts) > 1:
                name = parts[1]
        return _normalize_col(name)

    # Read CSVs
    df_out = pd.read_csv(output_file)
    df_exp = pd.read_csv(expected_file)

    # Rename columns according to rules
    df_out.columns = [_normalize_output_col(c) for c in df_out.columns]
    df_exp.columns = [_normalize_col(c) for c in df_exp.columns]

    # Compare sets of columns (ignoring order)
    if set(df_out.columns) != set(df_exp.columns):
        print("CSV column sets differ.")
        print("Output columns:  ", sorted(df_out.columns))
        print("Expected columns:", sorted(df_exp.columns))
        return False

    # Reorder columns in a consistent way (sorted) for comparison
    cols = sorted(df_out.columns)
    df_out = df_out[cols]
    df_exp = df_exp[cols]

    # Sort rows to ignore row order
    df_out_sorted = df_out.sort_values(by=cols).reset_index(drop=True)
    df_exp_sorted = df_exp.sort_values(by=cols).reset_index(drop=True)

    # If they are exactly equal, good
    if df_out_sorted.equals(df_exp_sorted):
        return True

    # They differ: print some debug info using pandas compare
    print("CSV content differs.")

    if df_out_sorted.shape != df_exp_sorted.shape:
        print(f"Different shapes: output={df_out_sorted.shape}, expected={df_exp_sorted.shape}")

    # Use wide display options and avoid compact summaries
    with pd.option_context(
            "display.max_rows", None,
            "display.max_columns", None,
            "display.width", 0,
            "display.max_colwidth", None
    ):
        # Find rows present in one but not the other (like SQL EXCEPT)
        df_out_sorted["#TYPE"] = "df_out"
        df_exp_sorted["#TYPE"] = "df_exp"
        # drop the duplicates except the #TYPE column
        out_not_in_exp = pd.concat([df_out_sorted, df_exp_sorted]).drop_duplicates(subset=[c for c in df_out_sorted.columns if c != "#TYPE"], keep=False)
        if not out_not_in_exp.empty:
            print("\nRows that differ between output and expected (union of mismatches):")
            print(out_not_in_exp.head(20))  # limit to first 20 rows for readability
            print(f"... total differing rows: {len(out_not_in_exp)}")
        else:
            # Shapes might differ, or some subtle mismatch — use compare for aligned rows
            try:
                diff = df_out_sorted.compare(df_exp_sorted, keep_shape=False, keep_equal=False)
                print("\nCell-level differences:")
                print(diff.head(20))
            except Exception as e:
                print("Could not perform detailed compare due to:", e)

    return False