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


NUM_TRIES = 3
RESULTS_FOLDER = Path("results")


def run_command(command, output_path):
    with open(output_path, 'w') as out_file:
        start = time.perf_counter()
        subprocess.run(command, shell=True, stdout=out_file, stderr=subprocess.STDOUT)
        end = time.perf_counter()
    return end - start


def normalize_output(file_path):
    if not os.path.exists(file_path):
        return set()
    with open(file_path, 'r') as f:
        lines = f.readlines()
    return set(sorted(line.strip() for line in lines if line.strip()))


def compare_outputs(file1, file2):
    return normalize_output(file1) == normalize_output(file2)


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


def run_test_multiple_times(command, output_path):
    times = []
    print(f"Running: {command}")
    for i in range(NUM_TRIES):
        print(f"  Try {i+1}/{NUM_TRIES}...")
        temp_output = output_path.with_name(f"{output_path.stem}_try{i}{output_path.suffix}")
        elapsed = run_command(command, temp_output)
        times.append(elapsed)
        if i == NUM_TRIES - 1:
            temp_output.rename(output_path)
        else:
            os.remove(temp_output)
    return times


def process_test(test, config, config_name, comparison_results, previous_results):
    test_name = test["test"]
    input_file = test["input_file"]
    input_path = Path(config["input_folder"]) / input_file
    output_folder = Path(config["output_folder"])
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

    command_results = []
    for cmd in test["commands"]:
        resolved_cmd = cmd.replace("$file", actual_input_path)
        output_path = output_folder / f"{test_name.replace(' ', '_')}.txt"
        times = run_test_multiple_times(resolved_cmd, output_path)

        if clean_predicates and query_preds:
            filter_output_by_predicates(output_path, query_preds)

        if expected_folder:
            expected_path = expected_folder / f"{test_name.replace(' ', '_')}.txt"
            output_match = "match" if compare_outputs(output_path, expected_path) else "mismatch"
        else:
            output_match = ""

        avg_time = mean(times)
        min_time = min(times)
        max_time = max(times)
        delta_vs_cmp = avg_time - comparison_results.get(test_name, 0.0)
        delta_vs_prev = avg_time - previous_results.get(test_name, 0.0)
        if test_name not in comparison_results:
            delta_vs_cmp = "-"
        else:
            delta_vs_cmp = f"{delta_vs_cmp:.4f}"

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
            "delta_vs_cmp", "delta_vs_prev",
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

    all_results = []
    for test in config["tests"]:
        results = process_test(test, config, config_name, comparison_results, previous_results)
        all_results.extend(results)

    csv_file = write_results_csv(config_name, all_results)
    print(f"\n✅ Benchmark complete. Results saved to: {csv_file}")


if __name__ == "__main__":
    main()