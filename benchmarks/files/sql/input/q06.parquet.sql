%@sql
SELECT MIN(EVENTDATE) as min, MAX(EVENTDATE) as max
FROM "downloads/hits.parquet"
