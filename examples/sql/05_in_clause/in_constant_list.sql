% IN with Constant List
% Filter products by category.
%@sql
SELECT PRODUCT_NAME, CATEGORY, PRICE
FROM "./data/products.csv"
WHERE CATEGORY IN ("Electronics", "Clothing")
