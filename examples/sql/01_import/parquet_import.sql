% Parquet Import
% Import a Parquet file using the shorthand syntax.
% The path can be a single file, a directory, or a glob pattern (e.g., "./data/**/*.parquet").
%@sql
SELECT *
FROM "./data/nations.parquet"
