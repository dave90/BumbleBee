SELECT
    supp_nation,
    cust_nation,
    l_shipdate,
    sum(volume) AS revenue
FROM (
    SELECT
        n1.n_name AS supp_nation,
        n2.n_name AS cust_nation,
        l_shipdate,
        l_extendedprice * (1 - l_discount) AS volume
    FROM
        './files/parquet/tpch/supplier.parquet',
        './files/parquet/tpch/lineitem.parquet',
        './files/parquet/tpch/orders.parquet',
        './files/parquet/tpch/customer.parquet',
        './files/parquet/tpch/nation.parquet' AS n1,
        './files/parquet/tpch/nation.parquet' AS n2
    WHERE
        s_suppkey = l_suppkey
        AND o_orderkey = l_orderkey
        AND c_custkey = o_custkey
        AND s_nationkey = n1.n_nationkey
        AND c_nationkey = n2.n_nationkey
        AND ((n1.n_name = 'FRANCE'
                AND n2.n_name = 'GERMANY')
            OR (n1.n_name = 'GERMANY'
                AND n2.n_name = 'FRANCE'))
        AND l_shipdate > '1995-01-01' AND l_shipdate < '1996-12-31') AS shipping
GROUP BY
    supp_nation,
    cust_nation,
    l_shipdate
ORDER BY
    supp_nation,
    cust_nation,
    l_shipdate
LIMIT 1000