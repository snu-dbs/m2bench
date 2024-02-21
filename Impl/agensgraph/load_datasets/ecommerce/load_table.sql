
\c ecommerce;

COPY brand FROM '/tmp/m2bench/ecommerce/table/Brand.csv' DELIMITER ',' CSV HEADER; 
COPY product FROM '/tmp/m2bench/ecommerce/table/Product.csv' DELIMITER ',' CSV HEADER;
COPY customer FROM '/tmp/m2bench/ecommerce/table/Customer.csv' DELIMITER '|' CSV HEADER;
