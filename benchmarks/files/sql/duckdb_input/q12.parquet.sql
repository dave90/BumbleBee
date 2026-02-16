SELECT SearchPhrase as s, COUNT(*) as c FROM 'downloads/hits.parquet' WHERE SearchPhrase <> '' GROUP BY SearchPhrase ORDER BY c DESC, SearchPhrase DESC LIMIT 6
