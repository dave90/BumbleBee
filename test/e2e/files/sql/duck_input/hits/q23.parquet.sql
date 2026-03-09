SELECT *
FROM './files/parquet/mini_hits.parquet'
WHERE URL LIKE '%google%'
ORDER BY EventTime LIMIT 10

