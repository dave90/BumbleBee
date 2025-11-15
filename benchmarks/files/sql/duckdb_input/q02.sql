SELECT SUM(AdvEngineID)as sum, COUNT(*) as count, AVG(ResolutionWidth) as avg
FROM 'downloads/mini_hits.csv'
