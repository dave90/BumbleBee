SELECT SUM(AdvEngineID)as sum, COUNT(*) as count, AVG(ResolutionWidth) as avg
FROM 'downloads/hits.parquet'
