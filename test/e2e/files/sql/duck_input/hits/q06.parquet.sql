SELECT MIN(EventDate) as min, MAX(EventDate) as max
FROM './files/parquet/mini_hits.parquet'