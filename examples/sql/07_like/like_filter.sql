% LIKE Filter
% Filter rows using the SQL LIKE operator with '%' wildcards.
% '%' matches any sequence of characters.
%@sql
SELECT NAME, SALARY
FROM "./data/employees.csv"
WHERE NAME LIKE "%son%"