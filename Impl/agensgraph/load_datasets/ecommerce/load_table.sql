
\c ecommerce;

SET DATASET_PAT 

COPY brand FROM '/ecommerce/table/Brand.csv' DELIMITER ',' CSV HEADER; 

COPY product FROM '/ecommerce/table/Product.csv' DELIMITER ',' CSV HEADER;

COPY customer FROM '/ecommerce/table/Customer.csv' DELIMITER '|' CSV HEADER;

