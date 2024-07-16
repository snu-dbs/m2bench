\c disaster;
  
DROP TABLE IF EXISTS Finedust;
DROP TABLE IF EXISTS Finedust_idx;

CREATE TABLE IF NOT EXISTS Finedust (
        timestamp INT,
        latitude FLOAT,
        longitude float,
        pm10 FLOAT,
        pm2_5 FLOAT
        );

CREATE TABLE IF NOT EXISTS Finedust_idx (
        timestamp INT,
        latitude INT,
        longitude INT,
        pm10 FLOAT,
        pm2_5 FLOAT
        );
