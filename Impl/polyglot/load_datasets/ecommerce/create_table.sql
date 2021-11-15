CREATE DATABASE IF NOT EXISTS Ecommerce;
  
USE Ecommerce;

DROP TABLE IF EXISTS Customer;
DROP TABLE IF EXISTS Product;
DROP TABLE IF EXISTS Brand;


CREATE TABLE IF NOT EXISTS Customer (
        customer_id varchar(20) PRIMARY KEY,
        person_id INT,
        gender CHAR(1),
        date_of_birth DATE,
        zipcode varchar(10),
        city varchar(30),
        county varchar(30),
        state varchar(15)
        );

CREATE TABLE IF NOT EXISTS Product (
        product_id CHAR(11) PRIMARY KEY,
        title VARCHAR(1000),
        price  float,
        brand_id int
        );

CREATE TABLE IF NOT EXISTS Brand (
        brand_id INT,
        name VARCHAR(40),
        country VARCHAR(20),
        industry VARCHAR(20)
        );
