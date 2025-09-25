#!/usr/bin/env bash

# Config: adjust if your folders live elsewhere
EXPECTED_DIR="files/asp/expected"
ACTUAL_DIR="files/asp/actual"


if [[ $# -ne 1 ]]; then
  echo "Please provide a input file"
  exit 1
fi

REL_PATH="$1"

EXPECTED_FILE="$EXPECTED_DIR/$REL_PATH"
ACTUAL_FILE="$ACTUAL_DIR/$REL_PATH"

if [[ ! -f "$EXPECTED_FILE" ]]; then
  echo "Error: expected file not found: $EXPECTED_FILE" >&2
  exit 2
fi
if [[ ! -f "$ACTUAL_FILE" ]]; then
  echo "Error: actual file not found: $ACTUAL_FILE" >&2
  exit 3
fi

# Create stmpe and stmpa in the current directory
STMPE="stmpe"
STMPA="stmpa"

# Normalize, sort, and deduplicate
# LC_ALL=C for consistent byte-wise sorting regardless of locale
LC_ALL=C sort -u -- "$EXPECTED_FILE" > "$STMPE"
LC_ALL=C sort -u -- "$ACTUAL_FILE"   > "$STMPA"

# Run diff
echo "Diff (expected vs actual) on unique, sorted lines:"
diff -u --label "$EXPECTED_FILE(sorted-uniq)" --label "$ACTUAL_FILE(sorted-uniq)" "$STMPE" "$STMPA" || {
  # diff returns non-zero when differences are found; that's okay
  exit_code=$?
  if [[ $exit_code -eq 1 ]]; then
    # differences found
    exit 1
  else
    # real error
    exit $exit_code
  fi
}
