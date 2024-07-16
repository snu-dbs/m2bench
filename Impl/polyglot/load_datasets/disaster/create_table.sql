CREATE DATABASE IF NOT EXISTS Disaster;
  

USE Disaster;

DROP TABLE IF EXISTS Earthquake;
DROP TABLE IF EXISTS Shelter;
DROP TABLE IF EXISTS Gps;


CREATE TABLE IF NOT EXISTS Earthquake (
        id INT PRIMARY KEY,
        time DATETIME,
        coordinates POINT NOT NULL SRID 0,
        depth FLOAT,
        magnitude FLOAT,

        spatial index (coordinates),
        index (magnitude)
);

CREATE TABLE IF NOT EXISTS Shelter (
        id INT PRIMARY KEY,
        site_id INT,
        capacity FLOAT,
        name  CHAR(100),

        index (site_id)
);

CREATE TABLE IF NOT EXISTS Gps (
        id INT PRIMARY KEY,
        user_id INT,
        coordinates POINT not NULL SRID 0,
        time DATETIME,

        spatial index(coordinates)
);
