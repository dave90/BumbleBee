SELECT UserID, SearchPhrase, COUNT(*) as c FROM './files/parquet/mini_hits.parquet' GROUP BY UserID, SearchPhrase ORDER BY c DESC LIMIT 10
