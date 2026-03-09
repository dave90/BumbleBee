% IN with Subquery
% Find employees in departments located in New York.
%@sql
SELECT e.NAME, e.DEPARTMENT_ID, e.SALARY
FROM "./data/employees.csv" AS e
WHERE e.DEPARTMENT_ID IN (
    SELECT d.DEPT_ID
    FROM "./data/departments.csv" AS d
    WHERE d.LOCATION = "New York"
)
