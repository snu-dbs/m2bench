\c ecommerce;

CREATE EXTENSION IF NOT EXISTS file_fdw;
CREATE SERVER IF NOT EXISTS import_server FOREIGN DATA WRAPPER file_fdw;

DROP FOREIGN TABLE IF EXISTS person;
DROP FOREIGN TABLE IF EXISTS follows;
DROP FOREIGN TABLE IF EXISTS hashtag;
DROP FOREIGN TABLE IF EXISTS interested_in;
DROP GRAPH social_network CASCADE;

CREATE FOREIGN TABLE IF NOT EXISTS person (
    person_id    INT,
    gender       CHAR(1),
    date_of_brith DATE,
    firstname    VARCHAR(20),
    lastname     VARCHAR(20),
    nationality  VARCHAR(20),
    email        VARCHAR(50)
)
SERVER import_server
OPTIONS (
    FORMAT 'csv',
    HEADER 'true',
    FILENAME '/tmp/m2bench/ecommerce/property_graph/person_node.csv',
    DELIMITER '|'
);

CREATE FOREIGN TABLE IF NOT EXISTS follows (
    _from        INT,
    _to          INT,
    created_time TIMESTAMP
)
SERVER import_server
OPTIONS (
    FORMAT 'csv',
    HEADER 'true',
    FILENAME '/tmp/m2bench/ecommerce/property_graph/person_follows_person.csv',
    DELIMITER '|'
);

CREATE FOREIGN TABLE IF NOT EXISTS hashtag (
    tag_id  INT,
    content VARCHAR(31)
)
SERVER import_server
OPTIONS (
    FORMAT 'csv',
    HEADER 'true',
    FILENAME '/tmp/m2bench/ecommerce/property_graph/hashtag_node.csv',
    DELIMITER ','
);

CREATE FOREIGN TABLE IF NOT EXISTS interested_in (
    _from        INT,
    _to          INT,
    created_time CHAR(20)
)
SERVER import_server
OPTIONS (
    FORMAT 'csv',
    HEADER 'true',
    FILENAME '/tmp/m2bench/ecommerce/property_graph/person_interestedIn_tag.csv',
    DELIMITER ','
);

CREATE GRAPH social_network;
