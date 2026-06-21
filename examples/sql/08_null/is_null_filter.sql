% IS NULL / IS NOT NULL Filtering
% Empty fields in a CSV are read as NULL. Use IS NULL / IS NOT NULL to
% filter them. A plain comparison (e.g. SCORE = 80) follows three-valued
% logic and never matches a NULL, so IS NULL is the way to select them.
% Here: the respondents who did not provide a SCORE.
%@sql
SELECT RESPONDENT_ID, REGION, AGE, SCORE
FROM "./data/survey.csv"
WHERE SCORE IS NULL
