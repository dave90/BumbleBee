%@sql
SELECT
    S_SUPPKEY AS s_suppkey,
    S_NAME AS s_name,
    TOTAL_REVENUE AS total_revenue
FROM
    "downloads/tpch/supplier.parquet",
    (SELECT
         L_SUPPKEY AS SUPPLIER_NO,
         SUM(L_EXTENDEDPRICE * (1 - L_DISCOUNT)) AS TOTAL_REVENUE
     FROM
         "downloads/tpch/lineitem.parquet"
     WHERE
         L_SHIPDATE >= "1996-01-01"
         AND L_SHIPDATE < "1996-04-01"
     GROUP BY
         SUPPLIER_NO) AS REVENUE0
WHERE
    S_SUPPKEY = SUPPLIER_NO
    AND TOTAL_REVENUE = (
        SELECT MAX(TOTAL_REVENUE)
        FROM (
            SELECT
                L_SUPPKEY AS SUPPLIER_NO,
                SUM(L_EXTENDEDPRICE * (1 - L_DISCOUNT)) AS TOTAL_REVENUE
            FROM
                "downloads/tpch/lineitem.parquet"
            WHERE
                L_SHIPDATE >= "1996-01-01"
                AND L_SHIPDATE < "1996-04-01"
            GROUP BY
                SUPPLIER_NO) AS REVENUE1)
ORDER BY
    s_suppkey
LIMIT 1000