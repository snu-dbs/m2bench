SET GLOBAL local_infile = 1;

CREATE DATABASE IF NOT EXISTS Ecommerce;

USE Ecommerce;

DROP TABLE IF EXISTS Person;
DROP TABLE IF EXISTS Follows;
DROP TABLE IF EXISTS Hashtag;
DROP TABLE IF EXISTS Interested_in;

CREATE TABLE IF NOT EXISTS Person (
    person_id     INT,
    gender        CHAR(1),
    date_of_brith DATE,
    firstname     VARCHAR(20),
    lastname      VARCHAR(20),
    nationality   VARCHAR(20),
    email         VARCHAR(50)
);

CREATE TABLE IF NOT EXISTS Follows (
    _from        INT,
    _to          INT,
    created_time TIMESTAMP
);

CREATE TABLE IF NOT EXISTS Hashtag (
    tag_id  INT,
    content VARCHAR(30)
);

CREATE TABLE IF NOT EXISTS Interested_in (
    _from        INT,
    _to          INT,
    created_time CHAR(20)
);
