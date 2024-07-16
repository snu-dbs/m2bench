USE Disaster;

LOAD DATA LOCAL INFILE '../../../Datasets/disaster/table/Earthquake.csv' 
INTO TABLE Earthquake
FIELDS TERMINATED BY ',' 
ENCLOSED BY '"'
LINES TERMINATED BY '\n'
IGNORE 1 ROWS
(id, time, @latitude, @longitude, depth, magnitude)
SET coordinates = POINT(@latitude, @longitude);

LOAD DATA LOCAL INFILE '../../../Datasets/disaster/table/Gps.csv' 
INTO TABLE Gps
FIELDS TERMINATED BY ',' 
ENCLOSED BY '"'
LINES TERMINATED BY '\n'
IGNORE 1 ROWS
(id, user_id, @longitude, @latitude, time)
SET coordinates = POINT(@latitude, @longitude);

LOAD DATA LOCAL INFILE '../../../Datasets/disaster/table/Shelter.csv' 
INTO TABLE Shelter
FIELDS TERMINATED BY '|' 
ENCLOSED BY '"'
LINES TERMINATED BY '\n'
IGNORE 1 ROWS
(id, site_id, capacity, name);

