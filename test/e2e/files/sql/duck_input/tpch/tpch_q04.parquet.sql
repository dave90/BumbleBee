SELECT
    O_ORDERPRIORITY,
    count(*) AS order_count
FROM
    './files/parquet/tpch/orders.parquet'
WHERE
    O_ORDERDATE >= '1993-07-01'
  AND O_ORDERDATE < '1993-10-01'
  AND 0 < (
    SELECT
        COUNT(*)
    FROM
        './files/parquet/tpch/lineitem.parquet'
    WHERE
        L_ORDERKEY = O_ORDERKEY
      AND L_COMMITDATE < L_RECEIPTDATE)
GROUP BY
    O_ORDERPRIORITY
ORDER BY
    O_ORDERPRIORITY
LIMIT 1000
