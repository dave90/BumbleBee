SELECT UserID, SearchPhrase, COUNT(*) as c FROM './files/csv/mini_hits.csv' GROUP BY UserID, SearchPhrase ORDER BY c DESC LIMIT 10
