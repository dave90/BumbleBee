%@sql
SELECT
    N_NAME AS n_name,
    SUM(L_EXTENDEDPRICE * (1 - L_DISCOUNT)) AS revenue
FROM
    "./files/parquet/tpch/customer.parquet",
    "./files/parquet/tpch/orders.parquet",
    "./files/parquet/tpch/lineitem.parquet",
    "./files/parquet/tpch/supplier.parquet",
    "./files/parquet/tpch/nation.parquet",
    "./files/parquet/tpch/region.parquet"
WHERE
    C_CUSTKEY = O_CUSTKEY
  AND L_ORDERKEY = O_ORDERKEY
  AND L_SUPPKEY = S_SUPPKEY
  AND C_NATIONKEY = S_NATIONKEY
  AND S_NATIONKEY = N_NATIONKEY
  AND N_REGIONKEY = R_REGIONKEY
  AND R_NAME = "ASIA"
  AND O_ORDERDATE >= "1994-01-01"
  AND O_ORDERDATE < "1995-01-01"
GROUP BY
    N_NAME
ORDER BY
    revenue DESC
LIMIT 100