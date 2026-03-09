% Basic CSV Import
% Import a CSV file using the shorthand syntax.
% BumbleBee auto-detects the delimiter and header.
% The path can be a single file, a directory, or a glob pattern (e.g., "./data/*.csv").
%@sql
SELECT *
FROM "./data/employees.csv"
