% GROUP BY with Multiple Aggregations
% Aggregate employee data by department.
%@sql
SELECT
    DEPARTMENT_ID,
    COUNT(*) AS num_employees,
    SUM(SALARY) AS total_salary,
    MIN(SALARY) AS min_salary,
    MAX(SALARY) AS max_salary
FROM "./data/employees.csv"
GROUP BY DEPARTMENT_ID
