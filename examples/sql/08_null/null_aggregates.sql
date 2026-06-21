% Aggregates over NULL-bearing data
% NULL inputs are skipped by SUM/AVG/MIN/MAX and by COUNT(col):
%   - COUNT(*)     counts every row, including those with a NULL SCORE.
%   - COUNT(SCORE) counts only the rows where SCORE is not NULL.
%   - SUM/AVG/MIN/MAX(SCORE) ignore NULL scores.
% Grouping is by REGION; the rows whose REGION is empty form a NULL group.
%@sql
SELECT
    REGION,
    COUNT(*) AS n_rows,
    COUNT(SCORE) AS n_scores,
    SUM(SCORE) AS total_score,
    AVG(SCORE) AS avg_score
FROM "./data/survey.csv"
GROUP BY REGION
