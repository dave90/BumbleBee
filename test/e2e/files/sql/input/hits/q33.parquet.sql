%@sql
SELECT URL, COUNT(*) AS c
FROM "./files/parquet/mini_hits.parquet"
GROUP BY URL
ORDER BY c DESC
LIMIT 10