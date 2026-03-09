%@sql
SELECT SUM(ADVENGINEID) as sum, COUNT(*) as count, AVG(RESOLUTIONWIDTH)  as avg
FROM "./files/parquet/mini_hits.parquet"