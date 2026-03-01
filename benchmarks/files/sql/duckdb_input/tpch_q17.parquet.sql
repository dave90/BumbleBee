SELECT
    sum(L_EXTENDEDPRICE) / 7.0 AS avg_yearly
FROM
    'downloads/tpch/lineitem.parquet',
    'downloads/tpch/part.parquet'
WHERE
    P_PARTKEY = L_PARTKEY
    AND P_BRAND = 'Brand#23'
    AND P_CONTAINER = 'MED BOX'
    AND L_QUANTITY < (
        SELECT
            0.2 * avg(L_QUANTITY)
        FROM
            'downloads/tpch/lineitem.parquet'
        WHERE
            L_PARTKEY = P_PARTKEY)