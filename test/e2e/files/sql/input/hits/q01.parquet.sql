%@sql
SELECT COUNT(*) as count
FROM "./files/parquet/mini_hits.parquet"
WHERE ADVENGINEID <> 0
