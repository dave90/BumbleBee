import os
import subprocess
import re
import tempfile
from utils import contains_query

# Define input and output directories
INPUT_DIR = 'files/sql/duck_input'
OUTPUT_DIR = 'files/sql/expected'

# Ensure output directory exists
os.makedirs(OUTPUT_DIR, exist_ok=True)

# list of tests with filtered atoms (clingo does not support it)

def create_expected():
    # Iterate over all files in the input directory
    for root, _, files in os.walk(INPUT_DIR):
        for filename in files:

            relative_path = os.path.relpath(root, INPUT_DIR)
            input_path = os.path.join(INPUT_DIR, relative_path, filename)
            output_path = os.path.join(OUTPUT_DIR, filename)

            if not os.path.isfile(input_path):
                continue

            print(f"Processing {filename}...")
            f = open(input_path, 'r')
            sql = f.read()
            query = "COPY ("+str(sql)+") TO '"+output_path+"'"

            # Build the command
            command = [
                'duckdb',
                '-c',
                query
            ]

            # Run the command and capture the output
            try:
                result = subprocess.run(command, capture_output=True, text=True, check=True)
                pass
            except subprocess.CalledProcessError as e:
                print(f"Error processing {filename}: {e} ---------------------------")
                with open(output_path, 'w') as f:
                    f.write("ERROR")


if __name__ == '__main__':
    create_expected()
