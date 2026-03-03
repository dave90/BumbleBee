% Basic Aggregate Functions: COUNT, SUM, MIN, MAX
%@sql
SELECT
    COUNT(*) AS total_employees,
    SUM(SALARY) AS total_salary,
    MIN(SALARY) AS min_salary,
    MAX(SALARY) AS max_salary
FROM "./data/employees.csv"
