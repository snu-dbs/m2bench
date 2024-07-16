\c disaster;
  
CREATE EXTENSION IF NOT EXISTS file_fdw;
CREATE SERVER IF NOT EXISTS import_server FOREIGN DATA WRAPPER file_fdw;

DROP FOREIGN TABLE IF EXISTS Roadnodes;
DROP FOREIGN TABLE IF EXISTS Roads;
DROP GRAPH Road_network CASCADE;

CREATE FOREIGN TABLE IF NOT EXISTS Roadnodes (
        roadnode_id INT,
        site_id INT
        )
SERVER import_server
OPTIONS (FORMAT 'csv', HEADER 'true', FILENAME '/tmp/m2bench/disaster/property_graph/Roadnode.csv', delimiter',');

CREATE FOREIGN TABLE IF NOT EXISTS Roads (
        _from INT,
        _to INT,
        distance INT
        )
SERVER import_server
OPTIONS (FORMAT 'csv', HEADER 'true', FILENAME '/tmp/m2bench/disaster/property_graph/Road.csv', delimiter',');

CREATE GRAPH Road_network;
