SELECT UserID, COUNT(*) as c FROM './files/parquet/mini_hits.parquet' GROUP BY UserID ORDER BY c DESC LIMIT 10
