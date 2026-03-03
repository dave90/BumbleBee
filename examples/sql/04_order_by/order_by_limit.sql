% ORDER BY with LIMIT
% ORDER BY requires LIMIT in BumbleBee.
% Get the top 5 highest-paid employees.
%@sql
SELECT NAME, SALARY
FROM "./data/employees.csv"
ORDER BY SALARY DESC
LIMIT 5
