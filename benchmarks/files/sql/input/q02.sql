%@sql
SELECT SUM(ADVENGINEID) as sum, COUNT(*) as count, AVG(RESOLUTIONWIDTH)  as avg
FROM "downloads/mini_hits.csv"
