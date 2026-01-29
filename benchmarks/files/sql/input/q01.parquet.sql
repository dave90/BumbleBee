%@sql
SELECT COUNT(*) as count
FROM "downloads/hits.parquet"
WHERE ADVENGINEID <> 0
