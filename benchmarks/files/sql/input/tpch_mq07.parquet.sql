%@sql
SELECT
    SUPP_NATION,
    CUST_NATION,
    L_SHIPDATE,
    SUM(VOLUME) AS REVENUE
FROM (
    SELECT
        N1.N_NAME AS SUPP_NATION,
        N2.N_NAME AS CUST_NATION,
        L_SHIPDATE,
        L_EXTENDEDPRICE * (1 - L_DISCOUNT) AS VOLUME
    FROM
        "downloads/tpch/lineitem.parquet",
        "downloads/tpch/supplier.parquet",
        "downloads/tpch/orders.parquet",
        "downloads/tpch/customer.parquet",
        "downloads/tpch/nation.parquet" AS N1,
        "downloads/tpch/nation.parquet" AS N2
    WHERE
        S_SUPPKEY = L_SUPPKEY
        AND O_ORDERKEY = L_ORDERKEY
        AND C_CUSTKEY = O_CUSTKEY
        AND S_NATIONKEY = N1.N_NATIONKEY
        AND C_NATIONKEY = N2.N_NATIONKEY
        AND ((N1.N_NAME = "FRANCE"
                AND N2.N_NAME = "GERMANY")
            OR (N1.N_NAME = "GERMANY"
                AND N2.N_NAME = "FRANCE"))
        AND L_SHIPDATE > "1995-01-01" AND L_SHIPDATE < "1996-12-31") AS SHIPPING
GROUP BY
    SUPP_NATION,
    CUST_NATION,
    L_SHIPDATE
ORDER BY
    SUPP_NATION,
    CUST_NATION,
    L_SHIPDATE
LIMIT 1000