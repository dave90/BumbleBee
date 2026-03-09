SELECT SearchPhrase as s, COUNT(*) as c FROM './files/csv/mini_hits.csv' WHERE SearchPhrase <> '' GROUP BY SearchPhrase ORDER BY c DESC LIMIT 6
