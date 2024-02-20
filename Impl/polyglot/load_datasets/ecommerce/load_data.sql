USE Ecommerce;

LOAD DATA LOCAL INFILE '../../../Datasets/ecommerce/table/Brand.csv' 
INTO TABLE Brand
FIELDS TERMINATED BY ',' 
ENCLOSED BY '"'
LINES TERMINATED BY '\n'
IGNORE 1 ROWS;

LOAD DATA LOCAL INFILE '../../../Datasets/ecommerce/table/Customer.csv' 
INTO TABLE Customer
FIELDS TERMINATED BY ',' 
ENCLOSED BY '"'
LINES TERMINATED BY '\n'
IGNORE 1 ROWS;

LOAD DATA LOCAL INFILE '../../../Datasets/ecommerce/table/Product.csv' 
INTO TABLE Product
FIELDS TERMINATED BY ',' 
ENCLOSED BY '"'
LINES TERMINATED BY '\n'
IGNORE 1 ROWS;