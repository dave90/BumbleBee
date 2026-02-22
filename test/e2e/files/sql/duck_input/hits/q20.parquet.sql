SELECT COUNT(*) as c
FROM './files/parquet/mini_hits.parquet'
WHERE URL LIKE '%google%'

