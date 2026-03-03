% Basic CSV Export
% Use COPY ... TO to export query results to a CSV file.
% Without single_file=1, BumbleBee writes one file per thread into the output directory.
%@sql
COPY (
    SELECT NAME, SALARY
    FROM "./data/employees.csv"
    WHERE SALARY > 80000
) TO "./output/high_earners.csv" (single_file=1)
