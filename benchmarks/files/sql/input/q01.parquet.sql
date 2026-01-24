%@sql
SELECT COUNT(*) as count
FROM "downloads/mini_hits.parquet"
WHERE ADVENGINEID <> 0
