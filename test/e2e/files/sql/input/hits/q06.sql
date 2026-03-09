%@sql
SELECT MIN(EVENTDATE) as min, MAX(EVENTDATE) as max
FROM "./files/csv/mini_hits.csv"
