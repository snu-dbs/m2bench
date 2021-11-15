CREATE DATABASE IF NOT EXISTS Disaster;
  

USE Disaster;

DROP TABLE IF EXISTS Earthquake;
DROP TABLE IF EXISTS Shelter;
DROP TABLE IF EXISTS Gps;


CREATE TABLE IF NOT EXISTS Earthquake (
        id INT PRIMARY KEY,
        gender CHAR(1),
        time DATETIME,
        coordinates POINT NOT NULL SRID 0,
        depth FLOAT,
        magnitude FLOAT,
        spatial index (coordinates)

        );

CREATE TABLE IF NOT EXISTS Shelter (
        id INT,
        shelter_index INT,
        coordinates POINT NOT NULL SRID 0,
        city CHAR(50),
        state CHAR(2),
        capacity FLOAT,
        name  CHAR(100),
        spatial index(coordinates)
        );

CREATE TABLE IF NOT EXISTS Gps (
        id INT,
        user_id INT,
        coordinates POINT not NULL SRID 0,
        time DATETIME,
        spatial index(coordinates)
        );
