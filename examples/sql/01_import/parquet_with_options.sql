% Parquet Import with Options
% Use &read_parquet to specify import parameters explicitly.
% Options: binary_as_string, columns_mapping
%@sql
SELECT N_NATIONKEY, N_NAME, N_REGIONKEY
FROM &read_parquet("./data/nations.parquet"; binary_as_string=1)
