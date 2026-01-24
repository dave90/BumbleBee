%@sql
SELECT USERID, COUNT(*) AS c
FROM "./files/parquet/mini_hits.parquet"
GROUP BY USERID
ORDER BY c DESC LIMIT 10