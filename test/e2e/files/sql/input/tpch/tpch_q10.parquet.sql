%@sql
SELECT
    C_CUSTKEY AS c_custkey,
    C_NAME AS c_name,
    SUM(L_EXTENDEDPRICE * ( 1 - L_DISCOUNT)) AS revenue,
    C_ACCTBAL AS c_acctbal,
    N_NAME AS n_name,
    C_ADDRESS AS c_address,
    C_PHONE AS c_phone,
    C_COMMENT AS c_comment
FROM
    "./files/parquet/tpch/customer.parquet",
    "./files/parquet/tpch/orders.parquet",
    "./files/parquet/tpch/lineitem.parquet",
    "./files/parquet/tpch/nation.parquet"
WHERE
    C_CUSTKEY = O_CUSTKEY
  AND L_ORDERKEY = O_ORDERKEY
  AND O_ORDERDATE >= "1993-10-01"
  AND O_ORDERDATE < "1994-01-01"
  AND L_RETURNFLAG = "R"
  AND C_NATIONKEY = N_NATIONKEY
GROUP BY
    C_CUSTKEY,
    C_NAME,
    C_ACCTBAL,
    C_PHONE,
    N_NAME,
    C_ADDRESS,
    C_COMMENT
ORDER BY
    revenue DESC
LIMIT 20
