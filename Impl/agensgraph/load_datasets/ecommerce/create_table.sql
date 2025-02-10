\c ecommerce;

DROP TABLE IF EXISTS brand;
DROP TABLE IF EXISTS product;
DROP TABLE IF EXISTS customer;

CREATE TABLE brand (
    brand_id  INT,
    name      VARCHAR(40),
    country   VARCHAR(20),
    industry  VARCHAR(20)
);

CREATE TABLE product (
    product_id  CHAR(11) PRIMARY KEY,
    title       VARCHAR(1000),
    price       REAL,
    brand_id    INT
);

CREATE TABLE customer (
    customer_id   VARCHAR(20) PRIMARY KEY,
    person_id     INT,
    gender        CHAR(1),
    date_of_birth TIMESTAMP,
    zipcode       VARCHAR(10),
    city          VARCHAR(30),
    county        VARCHAR(30),
    state         VARCHAR(15)
);

CREATE INDEX ON customer (customer_id);
CREATE INDEX ON product (product_id);
