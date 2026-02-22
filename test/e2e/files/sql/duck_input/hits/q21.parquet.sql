SELECT SearchPhrase, MIN(URL) as m, COUNT(*) AS c
FROM './files/parquet/mini_hits.parquet'
WHERE URL LIKE '%google%' AND SearchPhrase <> '' GROUP BY SearchPhrase ORDER BY c DESC LIMIT 10

