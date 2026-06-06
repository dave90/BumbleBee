% ORDER BY with NULLs — NULLS LAST
% When ordering ascending, NULL values sort AFTER every real value
% (NULLS LAST). Respondents without a SCORE appear at the end.
%@sql
SELECT RESPONDENT_ID, SCORE
FROM "./data/survey.csv"
ORDER BY SCORE
LIMIT 10
