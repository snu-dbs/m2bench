SET GLOBAL local_infile = 1;

CREATE DATABASE IF NOT EXISTS Ecommerce;

USE Ecommerce;

DROP TABLE IF EXISTS Brand;
DROP TABLE IF EXISTS Product;
DROP TABLE IF EXISTS Customer;

CREATE TABLE IF NOT EXISTS Brand (
    brand_id INT,
    name     VARCHAR(40),
    country  VARCHAR(20),
    industry VARCHAR(20)
);

CREATE TABLE IF NOT EXISTS Product (
    product_id CHAR(11) PRIMARY KEY,
    title      VARCHAR(1000),
    price      FLOAT,
    brand_id   INT
);

CREATE TABLE IF NOT EXISTS Customer (
    customer_id  VARCHAR(20) PRIMARY KEY,
    person_id    INT,
    gender       CHAR(1),
    date_of_birth DATE,
    zipcode      VARCHAR(10),
    city         VARCHAR(30),
    county       VARCHAR(30),
    state        VARCHAR(15)
);
