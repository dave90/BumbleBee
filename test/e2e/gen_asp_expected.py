import os
import subprocess
import re
import tempfile
from utils import contains_query

# Define input and output directories
INPUT_DIR = 'files/asp/input'
OUTPUT_DIR = 'files/asp/expected'

# Ensure output directory exists
os.makedirs(OUTPUT_DIR, exist_ok=True)

# list of tests with filtered atoms (clingo does not support it)

FILTER_PREDICATES = {}
TEST_TO_EXCLUDE = ["agg.sum.1","agg.sum.4"]

def create_expected():
    # Iterate over all files in the input directory
    for root, _, files in os.walk(INPUT_DIR):
        for filename in files:
            if filename in TEST_TO_EXCLUDE:
                continue

            relative_path = os.path.relpath(root, INPUT_DIR)
            input_path = os.path.join(INPUT_DIR, relative_path, filename)
            output_path = os.path.join(OUTPUT_DIR, filename)

            if not os.path.isfile(input_path):
                continue

            if contains_query(input_path):
                input_path = get_input_no_filters(input_path)

            print(f"Processing {filename}...")

            # Build the command
            command = [
                'clingo',
                '--mode=gringo',
                '--text',
                input_path
            ]

            # Run the command and capture the output
            try:
                result = subprocess.run(command, capture_output=True, text=True, check=True)
                cleaned_output = remove_comment_lines(result.stdout)
                with open(output_path, 'w') as f:
                    f.write(cleaned_output)

                if filename in FILTER_PREDICATES:
                    filter_output(output_path, filename)


                print(f"Output written to {output_path}")
            except subprocess.CalledProcessError as e:
                print(f"Error processing {filename}: {e}")
                with open(output_path, 'w') as f:
                    f.write("")

# return a test file without the
def get_input_no_filters(input_file:str):
    """Removes lines with predicate? and stores the predicates to filter output later"""
    filtered_predicates = []
    tmp_file = tempfile.NamedTemporaryFile(delete=False, mode='w', suffix='.lp', encoding='utf-8')
    temp_path = tmp_file.name

    with open(input_file, 'r') as f_in, open(temp_path, 'w') as f_out:
        for line in f_in:
            match = re.match(r'^(\w+)\([^)]*\)\?', line.strip())
            if match:
                pred = match.group(1)
                filtered_predicates.append(pred)
                continue  # skip writing this line
            f_out.write(line)

    # Store the filtered predicates for use in output filtering
    FILTER_PREDICATES[os.path.basename(input_file)] = set(filtered_predicates)
    return temp_path

def filter_output(output_file:str, filename:str):
    """Keeps only lines in the output that match filtered predicates"""
    predicates = FILTER_PREDICATES.get(filename, set())

    def keep_line(line):
        return any(re.match(rf'^{pred}\b', line.strip()) for pred in predicates)

    with open(output_file, 'r') as f:
        lines = f.readlines()

    with open(output_file, 'w') as f:
        for line in lines:
            if keep_line(line):
                f.write(line)

def remove_comment_lines(text: str) -> str:
    """Removes lines that start with '#' from the given text."""
    return '\n'.join(line for line in text.splitlines() if not line.strip().startswith('#')) + '\n'

if __name__ == '__main__':
    create_expected()
