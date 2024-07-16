\c disaster;
  
DROP TABLE IF EXISTS Earthquake;
DROP TABLE IF EXISTS Shelter;
DROP TABLE IF EXISTS Gps;

CREATE TABLE IF NOT EXISTS Earthquake (
        earthquake_id INT PRIMARY KEY,
        time TIMESTAMP,
        coordinates GEOMETRY,
        latitude float,
        longitude float,
        depth FLOAT,
        magnitude FLOAT
        );

CREATE TABLE IF NOT EXISTS Shelter (
        shelter_id INT PRIMARY KEY,
        site_id INT,
        capacity FLOAT,
        name  CHAR(100)
        );

CREATE TABLE IF NOT EXISTS Gps (
        gps_id INT PRIMARY KEY,
        user_id INT,
        coordinates GEOMETRY,
        longitude float,
        latitude float,
        time TIMESTAMP
        );
