%@sql
SELECT
    L_ORDERKEY AS l_orderkey,
    SUM(L_EXTENDEDPRICE - L_EXTENDEDPRICE * L_DISCOUNT) AS revenue,
    O_ORDERDATE AS o_orderdate,
    O_SHIPPRIORITY AS o_shippriority
FROM
    "downloads/tpch/lineitem.csv",
    "downloads/tpch/orders.csv",
    "downloads/tpch/customer.csv"
WHERE
    C_MKTSEGMENT = "BUILDING"
  AND C_CUSTKEY = O_CUSTKEY
  AND L_ORDERKEY = O_ORDERKEY
  AND O_ORDERDATE < "1995-03-15"
  AND L_SHIPDATE > "1995-03-15"
GROUP BY
    L_ORDERKEY,
    O_ORDERDATE,
    O_SHIPPRIORITY
ORDER BY
    revenue DESC,
    o_orderdate
    LIMIT 10