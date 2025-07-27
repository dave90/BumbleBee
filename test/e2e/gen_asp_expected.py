import os
import subprocess

# Define input and output directories
INPUT_DIR = 'files/asp/input'
OUTPUT_DIR = 'files/asp/expected'

# Ensure output directory exists
os.makedirs(OUTPUT_DIR, exist_ok=True)

def create_expected():
    # Iterate over all files in the input directory
    for filename in os.listdir(INPUT_DIR):
        input_path = os.path.join(INPUT_DIR, filename)
        output_path = os.path.join(OUTPUT_DIR, filename)

        if not os.path.isfile(input_path):
            continue
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
            with open(output_path, 'w') as f:
                f.write(result.stdout)
            print(f"Output written to {output_path}")
        except subprocess.CalledProcessError as e:
            print(f"Error processing {filename}: {e}")
            with open(output_path, 'w') as f:
                f.write("")


if __name__ == '__main__':
    create_expected()
