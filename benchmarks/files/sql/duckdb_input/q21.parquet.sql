SELECT SearchPhrase, MIN(Url) as m, COUNT(*) AS c
FROM 'downloads/hits.parquet'
WHERE Url LIKE '%google%' AND SearchPhrase <> '' GROUP BY SearchPhrase ORDER BY c DESC LIMIT 10

