SELECT MIN(EventDate) as min, MAX(EventDate) as max
FROM 'downloads/hits.parquet'