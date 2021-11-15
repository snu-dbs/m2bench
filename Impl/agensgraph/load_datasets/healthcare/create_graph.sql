\c healthcare;
  
CREATE EXTENSION IF NOT EXISTS file_fdw;
CREATE SERVER test FOREIGN DATA WRAPPER file_fdw;

DROP FOREIGN TABLE IF EXISTS Disease;
DROP FOREIGN TABLE IF EXISTS Is_a;
DROP GRAPH Disease_network CASCADE;

CREATE FOREIGN TABLE IF NOT EXISTS Disease (
        disease_id BIGINT,
        term varchar(10000)
        )
SERVER test
OPTIONS (FORMAT 'csv', HEADER 'true', FILENAME '/healthcare/property_graph/Disease_network_nodes.csv', delimiter',');

CREATE FOREIGN TABLE IF NOT EXISTS Is_a (
        source_id BIGINT,
        label varchar(10),
        destination_id BIGINT
        )
SERVER test
OPTIONS (FORMAT 'csv', HEADER 'true', FILENAME '/healthcare/property_graph/Disease_network_edges.csv', delimiter',');

CREATE GRAPH disease_network;
