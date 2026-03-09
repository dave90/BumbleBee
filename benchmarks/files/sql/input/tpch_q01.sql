%@sql
SELECT
    L_RETURNFLAG AS l_returnflag,
    L_LINESTATUS AS l_linestatus,
    SUM(L_QUANTITY) AS sum_qty,
    SUM(L_EXTENDEDPRICE) AS sum_base_price,
    SUM(L_EXTENDEDPRICE - L_EXTENDEDPRICE * L_DISCOUNT) AS sum_disc_price,
    SUM(L_EXTENDEDPRICE
        + L_EXTENDEDPRICE * L_TAX
        - L_EXTENDEDPRICE * L_DISCOUNT
        - L_EXTENDEDPRICE * L_DISCOUNT * L_TAX) AS sum_charge,
    AVG(L_QUANTITY) AS avg_qty,
    AVG(L_EXTENDEDPRICE) AS avg_price,
    AVG(L_DISCOUNT) AS avg_disc,
    COUNT(*) AS count_order
FROM
    "downloads/tpch/lineitem.csv"
WHERE
    L_SHIPDATE <= "1998-09-02"
GROUP BY
    L_RETURNFLAG,
    L_LINESTATUS
ORDER BY
    l_returnflag,
    l_linestatus
LIMIT 10
