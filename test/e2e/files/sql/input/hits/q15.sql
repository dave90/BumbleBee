%@sql
SELECT USERID, COUNT(*) AS c
FROM "./files/csv/mini_hits.csv"
GROUP BY USERID
ORDER BY c DESC LIMIT 10