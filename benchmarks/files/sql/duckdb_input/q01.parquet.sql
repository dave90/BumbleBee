SELECT COUNT(*)  as count
FROM 'downloads/hits.parquet'
WHERE AdvEngineID <> 0
