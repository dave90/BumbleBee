% CSV Import with Options
% Use &read_csv to specify import parameters explicitly.
% Available options: separator, header, auto_detect, quote, escape
%@sql
SELECT ID, NAME, SALARY
FROM &read_csv("./data/employees.csv"; separator=",", header=1, auto_detect=1)
