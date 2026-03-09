SELECT SearchPhrase, EventTime  FROM './files/csv/mini_hits.csv' WHERE SearchPhrase <> '' ORDER BY EventTime LIMIT 10
