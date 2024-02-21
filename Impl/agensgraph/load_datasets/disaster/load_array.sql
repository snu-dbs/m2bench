\c disaster
  
COPY Finedust FROM '/tmp/m2bench/disaster/array/Finedust.csv' DELIMITER ',' CSV HEADER;
CREATE INDEX Finedust_timestamp_idx ON Finedust USING btree (timestamp);
CREATE INDEX Finedust_latitude_idx ON Finedust USING btree (latitude);
CREATE INDEX Finedust_longitude_idx ON Finedust USING btree (longitude);

COPY Finedust_idx FROM '/tmp/m2bench/disaster/array/Finedust_idx.csv' DELIMITER ',' CSV HEADER;
CREATE INDEX Finedust_idx_timestamp_idx ON Finedust_idx USING btree (timestamp);
CREATE INDEX Finedust_idx_composite_idx ON Finedust_idx USING btree (timestamp, latitude, longitude);
CREATE INDEX Finedust_idx_coo_idx ON Finedust_idx USING btree (latitude, longitude);
