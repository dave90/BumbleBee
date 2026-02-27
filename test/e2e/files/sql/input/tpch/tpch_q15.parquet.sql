%@sql

SELECT S_SUPPKEY, S_NAME, TOTAL_REVENUE
FROM "./files/parquet/tpch/supplier.parquet",
     (SELECT L_SUPPKEY AS SUPPLIER_NO, SUM(L_EXTENDEDPRICE * (1 - L_DISCOUNT)) AS TOTAL_REVENUE
      FROM "./files/parquet/tpch/lineitem.parquet"
      WHERE L_SHIPDATE >= '1996-01-01' AND L_SHIPDATE < '1996-04-01'
      GROUP BY SUPPLIER_NO) AS REVENUE0
WHERE S_SUPPKEY = SUPPLIER_NO
  AND TOTAL_REVENUE = (SELECT MAX(TOTAL_REVENUE)
                       FROM (SELECT L_SUPPKEY AS SUPPLIER_NO, SUM(L_EXTENDEDPRICE * (1 - L_DISCOUNT)) AS TOTAL_REVENUE
                             FROM "./files/parquet/tpch/lineitem.parquet"
                             WHERE L_SHIPDATE >= '1996-01-01' AND L_SHIPDATE < '1996-04-01'
                             GROUP BY SUPPLIER_NO) AS REVENUE1)

