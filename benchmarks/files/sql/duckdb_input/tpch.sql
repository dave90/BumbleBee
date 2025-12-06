INSTALL tpch;
LOAD tpch;
CALL dbgen(sf=1);

COPY customer  TO 'downloads/tpch/customer.csv'  (FORMAT CSV, HEADER TRUE);
COPY orders    TO 'downloads/tpch/orders.csv'    (FORMAT CSV, HEADER TRUE);
COPY lineitem  TO 'downloads/tpch/lineitem.csv'  (FORMAT CSV, HEADER TRUE);
COPY part      TO 'downloads/tpch/part.csv'      (FORMAT CSV, HEADER TRUE);
COPY partsupp  TO 'downloads/tpch/partsupp.csv'  (FORMAT CSV, HEADER TRUE);
COPY supplier  TO 'downloads/tpch/supplier.csv'  (FORMAT CSV, HEADER TRUE);
COPY nation    TO 'downloads/tpch/nation.csv'    (FORMAT CSV, HEADER TRUE);
COPY region    TO 'downloads/tpch/region.csv'    (FORMAT CSV, HEADER TRUE);