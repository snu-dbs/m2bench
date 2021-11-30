
\c ecommerce;


\COPY brand FROM '../../../../Datasets/ecommerce/table/Brand.csv'   DELIMITER ',' CSV HEADER; 

\COPY product FROM '../../../../Datasets/ecommerce/table/Product.csv' DELIMITER ',' CSV HEADER;

\COPY customer FROM '../../../../Datasets/ecommerce/table/Customer.csv' DELIMITER '|' CSV HEADER;


