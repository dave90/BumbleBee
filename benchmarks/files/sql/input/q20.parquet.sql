%@sql
SELECT COUNT(*) as c
FROM "downloads/hits.parquet"
WHERE URL LIKE '%google%'

