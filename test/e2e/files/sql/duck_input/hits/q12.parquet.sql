SELECT SearchPhrase as s, COUNT(*) as c FROM './files/parquet/mini_hits.parquet' WHERE SearchPhrase <> '' GROUP BY SearchPhrase ORDER BY c DESC LIMIT 6
