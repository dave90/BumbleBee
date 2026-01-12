-- Snappy
COPY (
SELECT
    i::INTEGER               AS col1,
    (i + 1)::INTEGER AS col2,
    ('row_' || i::VARCHAR)   AS col3
FROM range(1, 101) t(i)
    ) TO 'data_snappy.parquet'
(FORMAT PARQUET, CODEC 'SNAPPY');

-- Gzip
COPY (
SELECT
    i::INTEGER               AS col1,
    (i * 2)::UINTEGER AS col2,
    ('row_' || i::VARCHAR)   AS col3
FROM range(1, 101) t(i)
    ) TO 'data_gzip.parquet'
(FORMAT PARQUET, CODEC 'GZIP');

-- Zstd
COPY (
SELECT
    i::INTEGER               AS col1,
    (i - 200)::INTEGER AS col2,
    ('row_' || i::VARCHAR)   AS col3
FROM range(1, 101) t(i)
    ) TO 'data_zstd.parquet'
(FORMAT PARQUET, CODEC 'ZSTD');

-- Null
COPY (
SELECT
    i::INTEGER                   AS col1,
    (i + 1)::INTEGER             AS col2,
    ('row_' || i::VARCHAR)       AS col3,

    -- now mix of NULL + values
    CASE WHEN i % 3 = 0 THEN NULL ELSE ('val_' || i::VARCHAR) END::VARCHAR AS col4,
    CASE WHEN i % 4 = 0 THEN NULL ELSE (i * 10) END::INTEGER              AS col5,
    CASE WHEN i % 5 = 0 THEN NULL ELSE (i::DECIMAL(10,2) / 3) END::DECIMAL(10,2) AS col_decimal,
    CASE WHEN i % 2 = 0 THEN NULL ELSE (TIMESTAMP '2025-01-01 00:00:00' + i * INTERVAL '1 hour') END AS col_timestamp,
    CASE WHEN i % 7 = 0 THEN NULL ELSE (DATE '2025-01-01') END AS col_date

FROM range(1, 10) t(i)
) TO 'null.parquet'
(FORMAT PARQUET);