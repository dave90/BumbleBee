SELECT COUNT(*)  as count
FROM 'downloads/mini_hits.parquet'
WHERE AdvEngineID <> 0
