SELECT SUM(AdvEngineID)as sum, COUNT(*) as count, AVG(ResolutionWidth) as avg
FROM './files/csv/mini_hits.csv'
