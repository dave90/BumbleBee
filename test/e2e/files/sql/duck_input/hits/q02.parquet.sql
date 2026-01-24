SELECT SUM(AdvEngineID)as sum, COUNT(*) as count, AVG(ResolutionWidth) as avg
FROM './files/parquet/mini_hits.parquet'