SELECT COUNT(*)  as count
FROM './files/parquet/mini_hits.parquet'
WHERE AdvEngineID <> 0
