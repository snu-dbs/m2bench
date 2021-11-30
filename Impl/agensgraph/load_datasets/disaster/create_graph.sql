\c disaster;
  
CREATE EXTENSION IF NOT EXISTS file_fdw;
CREATE SERVER test FOREIGN DATA WRAPPER file_fdw;

DROP FOREIGN TABLE IF EXISTS Roadnodes;
DROP FOREIGN TABLE IF EXISTS Roads;
DROP GRAPH Road_network CASCADE;

CREATE FOREIGN TABLE IF NOT EXISTS Roadnodes (
        roadnode_id INT,
        site_id INT
        )
SERVER test
OPTIONS (FORMAT 'csv', HEADER 'true', FILENAME :n, delimiter',');

CREATE FOREIGN TABLE IF NOT EXISTS Roads (
        _from INT,
        _to INT,
        distance INT
        )
SERVER test
OPTIONS (FORMAT 'csv', HEADER 'true', FILENAME :e, delimiter',');

CREATE GRAPH Road_network;
