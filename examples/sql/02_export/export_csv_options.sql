% CSV Export with Options
% COPY TO options: header, sep, single_file (otherwise one file per thread), mode
%@sql
COPY (
    SELECT NAME, DEPARTMENT_ID, SALARY
    FROM "./data/employees.csv"
) TO "./output/all_employees.csv" (header=1, sep="|", single_file=1, mode="overwrite")
