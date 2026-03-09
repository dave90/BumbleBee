% Scalar Subqueries in WHERE
% Find the employee(s) with the highest salary.
%@sql
SELECT NAME, SALARY
FROM "./data/employees.csv"
WHERE SALARY = (
    SELECT MAX(SALARY)
    FROM "./data/employees.csv"
)
