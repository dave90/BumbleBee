% Filtering on Aggregates using Subqueries
% Find employees whose salary is above their department average.
%@sql
SELECT e1.NAME, e1.DEPARTMENT_ID, e1.SALARY
FROM "./data/employees.csv" AS e1
WHERE e1.SALARY > (
    SELECT AVG(e2.SALARY)
    FROM "./data/employees.csv" AS e2
    WHERE e2.DEPARTMENT_ID = e1.DEPARTMENT_ID
)
