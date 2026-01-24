%@sql
SELECT MIN(EVENTDATE) as min, MAX(EVENTDATE) as max
FROM "./files/parquet/mini_hits.parquet"