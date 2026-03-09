SELECT UserID, COUNT(*) as c FROM './files/csv/mini_hits.csv' GROUP BY UserID ORDER BY c DESC LIMIT 10
