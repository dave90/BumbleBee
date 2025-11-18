%@sql
SELECT COUNT(*) as count
FROM "./files/csv/mini_hits.csv"
WHERE ADVENGINEID <> 0
