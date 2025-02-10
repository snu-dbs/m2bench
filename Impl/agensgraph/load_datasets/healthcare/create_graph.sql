\c healthcare;

CREATE EXTENSION IF NOT EXISTS file_fdw;
CREATE SERVER IF NOT EXISTS import_server FOREIGN DATA WRAPPER file_fdw;

DROP FOREIGN TABLE IF EXISTS Disease;
DROP FOREIGN TABLE IF EXISTS Is_a;
DROP GRAPH Disease_network CASCADE;

CREATE FOREIGN TABLE IF NOT EXISTS Disease (
    disease_id BIGINT,
    term       VARCHAR(10000)
)
SERVER import_server
OPTIONS (
    FORMAT 'csv',
    HEADER 'true',
    FILENAME '/tmp/m2bench/healthcare/property_graph/Disease_network_nodes.csv',
    DELIMITER ','
);

CREATE FOREIGN TABLE IF NOT EXISTS Is_a (
    source_id      BIGINT,
    label          VARCHAR(10),
    destination_id BIGINT
)
SERVER import_server
OPTIONS (
    FORMAT 'csv',
    HEADER 'true',
    FILENAME '/tmp/m2bench/healthcare/property_graph/Disease_network_edges.csv',
    DELIMITER ','
);

CREATE GRAPH disease_network;
