\c disaster
  
COPY Earthquake(earthquake_id, time, latitude, longitude, depth, magnitude) FROM '/tmp/m2bench/disaster/table/Earthquake.csv' DELIMITER ',' CSV HEADER;
UPDATE Earthquake set coordinates = ST_PointFromText('Point(' ||  longitude || ' ' || latitude || ')', 0);
CREATE INDEX Earthquake_coordinates_idx ON Earthquake USING GIST (coordinates);
CREATE INDEX Earthquake_coordinates_idx2 ON Earthquake USING GIST((coordinates::geography));
ALTER TABLE Earthquake DROP COlUMN latitude CASCADE;
ALTER TABLE Earthquake DROP COlUMN longitude CASCADE;

COPY Shelter FROM '/tmp/m2bench/disaster/table/Shelter.csv' DELIMITER '|' CSV HEADER;

COPY Gps(gps_id, user_id, longitude, latitude, time) FROM '/tmp/m2bench/disaster/table/Gps.csv' DELIMITER ',' CSV HEADER;
UPDATE Gps set coordinates = ST_PointFromText('Point(' ||  longitude || ' ' || latitude || ')', 0);
CREATE INDEX Gps_coordinates_idx ON Gps USING GIST (coordinates);
CREATE INDEX Gps_coordinates_idx2 ON Gps USING GIST((coordinates::geography));
ALTER TABLE Gps DROP COlUMN latitude CASCADE;
ALTER TABLE Gps DROP COlUMN longitude CASCADE;


create index Earthquake_magnitude_id on Earthquake(magnitude);
