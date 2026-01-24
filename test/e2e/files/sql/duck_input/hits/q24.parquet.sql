SELECT SearchPhrase, EventTime  FROM './files/parquet/mini_hits.parquet' WHERE SearchPhrase <> '' ORDER BY EventTime LIMIT 10
