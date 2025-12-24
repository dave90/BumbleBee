import os
import json
import subprocess
import time
import csv
import argparse
import re
from datetime import datetime
from statistics import mean
from pathlib import Path
from glob import glob
import tempfile
import shutil
import hashlib
import urllib.request
import urllib.parse
import email
import contextlib
import sys

# add test utilities
project_root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
sys.path.insert(0, project_root)  # put project root *first* in sys.path

NUM_TRIES = 3
RESULTS_FOLDER = Path("results")
TIMEOUT = 120

DOWNLOAD_DIR = Path("./downloads")
DOWNLOAD_DIR.mkdir(parents=True, exist_ok=True)

def run_command(command, output_path, timeout=TIMEOUT):
    with open(output_path, 'w') as out_file:
        start = time.perf_counter()
        try:
            subprocess.run(
                command,
                shell=True,
                stdout=out_file,
                stderr=subprocess.STDOUT,
                timeout=timeout  # Kill process if it runs longer than 2 minutes
            )
            end = time.perf_counter()
            return end - start
        except subprocess.TimeoutExpired:
            out_file.write("\n[ERROR] Process timed out after 120 seconds and was killed.\n")
            return TIMEOUT


def normalize_output(file_path):
    if not os.path.exists(file_path):
        return set()
    with open(file_path, 'r') as f:
        lines = f.readlines()
    return set(sorted(line.strip() for line in lines if line.strip()))

def normalize_output_shell(file_path):
    if not os.path.exists(file_path):
        return set()

    result = subprocess.run(
        ["bash", "-c", "awk '{$1=$1} NF' \"$1\" | sort -u", "_", file_path],
        capture_output=True,
        text=True,
        check=True,
    )

    return set(result.stdout.splitlines())

def compare_outputs(file1, file2):
    is_unix = sys.platform.startswith(("linux", "darwin"))
    if is_unix:
        return normalize_output_shell(file1) == normalize_output_shell(file2)
    return normalize_output(file1) == normalize_output(file2)

def compare_csv_outputs(file1, file2):
    from test.e2e.utils import compare_csv
    return compare_csv(str(file1), str(file2))

def extract_query_predicates(input_file_path):
    query_predicates = set()
    filtered_lines = []
    with open(input_file_path, 'r') as f:
        for line in f:
            stripped = line.strip()
            if stripped.endswith('?'):
                match = re.match(r'(\w+)\s*\(.*\)\s*\?', stripped)
                if match:
                    query_predicates.add(match.group(1))
            else:
                filtered_lines.append(line)
    return query_predicates, filtered_lines


def create_export_query(input_file_path, output_file_path):
    export_query = ""
    with open(input_file_path, 'r') as f:
        lines = f.readlines()
        contains_directive = sum([1 for l in lines if "%@sql" in l]+[0])
        if contains_directive:
            assert "%@sql" in lines[0]
            # keep first line as it contains %@sql
            query = "\n".join(lines[1:])
            export_query = lines[0] + "\n" + "COPY (\n"+query+"\n) TO \"" + str(output_file_path) + "\" (single_file=1)"
        else:
            query = "\n".join(lines)
            export_query = "COPY (\n"+query+"\n) TO \"" + str(output_file_path) + "\""

    return export_query

def filter_output_by_predicates(output_file_path, predicates):
    if not os.path.exists(output_file_path):
        return
    with open(output_file_path, 'r') as f:
        lines = f.readlines()
    filtered_lines = [
        line for line in lines
        if any(line.strip().startswith(f"{pred}(") for pred in predicates)
    ]
    with open(output_file_path, 'w') as f:
        f.writelines(filtered_lines)


def get_latest_results(config_name):
    pattern = RESULTS_FOLDER / f"{config_name}_benchmark_results_*.csv"
    files = sorted(glob(str(pattern)), reverse=True)
    if not files:
        return {}
    latest_file = files[0]
    print(f"Latest results: {latest_file}")
    with open(latest_file, newline='') as csvfile:
        reader = csv.DictReader(csvfile)
        return {row['test']: float(row['avg']) for row in reader}


def run_test_multiple_times(command, output_path, num_tries=None, timeout=None):
    times = []
    print(f"Running: {command}")
    if num_tries is None:
        num_tries = NUM_TRIES
    if timeout is None:
        timeout = TIMEOUT
    for i in range(num_tries):
        print(f"  Try {i+1}/{num_tries}...")
        temp_output = output_path.with_name(f"{output_path.stem}_try{i}{output_path.suffix}")
        elapsed = run_command(command, temp_output, timeout)
        times.append(elapsed)
        if i == num_tries - 1:
            temp_output.rename(output_path)
        else:
            os.remove(temp_output)
    return times


def filename_from_headers(hdrs, fallback):
    # Try Content-Disposition
    cd = hdrs.get("Content-Disposition")
    if cd:
        # Parse minimal Content-Disposition for filename / filename*
        # Example: attachment; filename="hits.csv.gz"
        msg = email.message.Message()
        msg.add_header("Content-Disposition", cd)
        params = dict(msg.get_params(header="content-disposition", failobj=[]))
        name = params.get("filename") or params.get("filename*")
        if name:
            return Path(name).name

    # Fallback to URL path
    if fallback:
        return Path(fallback).name

    # Final fallback: deterministic name from URL hash
    return f"download-{hashlib.sha256(fallback.encode('utf-8')).hexdigest()[:16]}"

def choose_filename(url, hdrs):
    parsed = urllib.parse.urlparse(url)
    url_basename = Path(urllib.parse.unquote(parsed.path)).name
    return filename_from_headers(hdrs, url_basename)

def filename_from_url(url: str) -> str:
    # Use ONLY the URL path to decide the filename (ignore headers).
    # Strip query/fragment, decode % escapes.
    parsed = urllib.parse.urlsplit(url)
    name = Path(urllib.parse.unquote(parsed.path)).name
    if not name:
        # Fallback to deterministic hash if URL has no terminal path segment
        name = f"download-{hashlib.sha256(url.encode('utf-8')).hexdigest()[:16]}"
    return name

def download_files(urls):
    """
    Download each URL into ./downloads unless it already exists there.
    Returns a list of local file paths for the requested downloads.
    """
    saved_paths = []

    for url in urls:
        # Basic sanity check
        if not isinstance(url, str) or not url.strip():
            print(f"[WARN] Skipping invalid URL entry: {url!r}")
            continue

        filename = filename_from_url(url)
        dest_path = DOWNLOAD_DIR / filename
        #  Skip BEFORE attempting any network request
        if dest_path.exists():
            print(f"[SKIP] {filename} already exists in {DOWNLOAD_DIR}/")
            saved_paths.append(str(dest_path))
            continue

        last_err = None
        for attempt in range(1, NUM_TRIES + 1):
            try:
                req = urllib.request.Request(
                    url,
                    headers={
                        "User-Agent": "Mozilla/5.0 (compatible; benchmark-downloader/1.0)"
                    },
                    method="GET",
                )
                start = time.perf_counter()
                with contextlib.closing(urllib.request.urlopen(req, timeout=TIMEOUT)) as resp:
                    # Decide filename
                    filename = choose_filename(url, resp.headers)
                    dest_path = DOWNLOAD_DIR / filename

                    # If already present, skip
                    if dest_path.exists():
                        print(f"[SKIP] {filename} already exists in {DOWNLOAD_DIR}/")
                        saved_paths.append(str(dest_path))
                        break

                    # Stream to a temp file then move atomically
                    with tempfile.NamedTemporaryFile(dir=str(DOWNLOAD_DIR), delete=False) as tmp:
                        while True:
                            chunk = resp.read(1024 * 1024)  # 1 MB chunks
                            if not chunk:
                                break
                            tmp.write(chunk)
                        tmp_path = Path(tmp.name)

                    tmp_path.replace(dest_path)
                    elapsed = time.perf_counter() - start
                    size_bytes = dest_path.stat().st_size
                    print(f"[OK] Downloaded {filename} ({size_bytes} bytes) in {elapsed:.2f}s")
                    saved_paths.append(str(dest_path))
                    break  # success; stop retrying
            except Exception as e:
                last_err = e
                if attempt < NUM_TRIES:
                    backoff = min(2 ** attempt, 8)
                    print(f"[RETRY] Failed to download {url} (attempt {attempt}/{NUM_TRIES}): {e}. Retrying in {backoff}s...")
                    time.sleep(backoff)
                else:
                    print(f"[ERROR] Could not download {url} after {NUM_TRIES} attempts: {e}")

    return saved_paths

def process_test(test, config, config_name, comparison_results, previous_results):
    test_name = test["test"]
    input_file = test["input_file"]
    input_path = Path(config["input_folder"]) / input_file
    output_folder = Path(config["output_folder"])
    compare_csv = config.get("compare_csv", False)
    output_folder.mkdir(parents=True, exist_ok=True)
    expected_folder = None
    if "expected_folder" in config:
        expected_folder = Path(config["expected_folder"])
        expected_folder.mkdir(parents=True, exist_ok=True)

    clean_predicates = config.get("clean_predicates", False)
    if clean_predicates:
        query_preds, cleaned_lines = extract_query_predicates(input_path)
        with tempfile.NamedTemporaryFile('w', delete=False, suffix=".lp") as tmp_input:
            tmp_input.writelines(cleaned_lines)
            actual_input_path = tmp_input.name
    else:
        actual_input_path = str(input_path)
        query_preds = set()

    sql_export_csv = test.get("sql_export_csv", False)
    output_csv_file = None
    if sql_export_csv:
        output_csv_file = output_folder / f"{test_name.replace(' ', '_')}.csv"
        export_query = create_export_query(input_path, output_csv_file)
        with tempfile.NamedTemporaryFile('w', delete=False, suffix=".lp") as tmp_input:
            tmp_input.writelines(export_query)
            actual_input_path = tmp_input.name

    command_results = []
    for cmd in test["commands"]:
        resolved_cmd = cmd.replace("$file", actual_input_path)
        output_path = output_folder / f"{test_name.replace(' ', '_')}.txt"
        num_tries = test["num_tries"] if "num_tries" in test else None
        timeout = test["timeout"] if "timeout" in test else None
        times = run_test_multiple_times(resolved_cmd, output_path, num_tries=num_tries, timeout=timeout)

        if clean_predicates and query_preds:
            filter_output_by_predicates(output_path, query_preds)


        if expected_folder and not compare_csv:
            expected_path = expected_folder / f"{test_name.replace(' ', '_')}.txt"
            output_match = "match" if compare_outputs(output_path, expected_path) else "mismatch"
        elif expected_folder and compare_csv:
            assert output_csv_file
            expected_path = expected_folder / f"{test_name.replace(' ', '_')}.csv"
            output_match = "match" if compare_csv_outputs(output_csv_file, expected_path) else "mismatch"
        else:
                output_match = ""

        avg_time = mean(times)
        min_time = min(times)
        max_time = max(times)
        cmp_avg = comparison_results.get(test_name)
        delta_vs_cmp = avg_time - comparison_results.get(test_name, 0.0)
        delta_vs_prev = avg_time - previous_results.get(test_name, 0.0)
        delta_vs_cmp_pct = "-"
        if test_name not in comparison_results:
            delta_vs_cmp = "-"
        else:
            delta_vs_cmp = f"{delta_vs_cmp:.4f}"
            delta_val = avg_time - cmp_avg
            if cmp_avg != 0:
                delta_pct = (delta_val / cmp_avg) * 100.0
                delta_vs_cmp_pct = f"{delta_pct:.2f}%"

        if test_name not in previous_results:
            delta_vs_prev = "-"
        else:
            delta_vs_prev = f"{delta_vs_prev:.4f}"

        command_results.append({
            "test": test_name,
            "input_file": input_file,
            "avg": f"{avg_time:.4f}",
            "min": f"{min_time:.4f}",
            "max": f"{max_time:.4f}",
            "delta_vs_cmp": f"{delta_vs_cmp}",
            "delta_vs_cmp_%": delta_vs_cmp_pct,
            "delta_vs_prev": f"{delta_vs_prev}",
            "output_match": output_match
        })

    if clean_predicates:
        os.remove(actual_input_path)

    return command_results


def write_results_csv(config_name, results):
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    csv_filename = RESULTS_FOLDER / f"{config_name}_benchmark_results_{timestamp}.csv"
    with open(csv_filename, 'w', newline='') as csvfile:
        fieldnames = [
            "test", "input_file",
            "avg", "min", "max",
            "delta_vs_cmp", "delta_vs_cmp_%", "delta_vs_prev",
            "output_match"
        ]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        for row in results:
            writer.writerow(row)
    return csv_filename


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("config", help="Path to benchmark config JSON")
    args = parser.parse_args()

    config_path = Path(args.config)
    config_name = config_path.stem
    RESULTS_FOLDER.mkdir(parents=True, exist_ok=True)

    with open(config_path, 'r') as f:
        config = json.load(f)

    previous_results = get_latest_results(config_name) if config.get("previous_comparison") else {}

    comparison_config_name = config.get("comparison_config")
    comparison_results = get_latest_results(Path(comparison_config_name).stem) if comparison_config_name else {}

    output_folder = config["output_folder"]
    # Remove the folder if it exists
    if os.path.exists(output_folder):
        shutil.rmtree(output_folder)

    # Recreate the folder
    os.makedirs(output_folder)

    if "downloads" in config:
        # download the required files if not exist
        download_files(config["downloads"])

    all_results = []
    for test in config["tests"]:
        if "skip" in test and test["skip"]:
            continue
        results = process_test(test, config, config_name, comparison_results, previous_results)
        all_results.extend(results)

    csv_file = write_results_csv(config_name, all_results)
    print(f"\n✅ Benchmark complete. Results saved to: {csv_file}")


if __name__ == "__main__":
    main()