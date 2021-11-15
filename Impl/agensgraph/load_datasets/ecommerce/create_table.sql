\c ecommerce;
  
DROP TABLE IF EXISTS brand;
DROP TABLE IF EXISTS product;
DROP TABLE IF EXISTS customer;

CREATE TABLE brand(
brand_id int,
name varchar(40),
country varchar(20),
industry varchar(20)
);

CREATE TABLE product(
product_id char(11) PRIMARY KEY,
title varchar(1000),
price real,
brand_id int
);

CREATE TABLE customer (
customer_id varchar(20) PRIMARY KEY,
person_id int,
gender CHAR(1),
date_of_birth timestamp,
zipcode varchar(10),
city varchar(30),
county varchar(30),
state varchar(15)
);

create index on customer(customer_id);
create index on product(product_id);

