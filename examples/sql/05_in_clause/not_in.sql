% NOT IN with Subquery
% Find employees NOT in departments located in Chicago.
%@sql
SELECT e.NAME, e.DEPARTMENT_ID, e.SALARY
FROM "./data/employees.csv" AS e
WHERE e.DEPARTMENT_ID NOT IN (
    SELECT d.DEPT_ID
    FROM "./data/departments.csv" AS d
    WHERE d.LOCATION = "Chicago"
)
