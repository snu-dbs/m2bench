\c ecommerce;

CREATE EXTENSION IF NOT EXISTS file_fdw;
CREATE SERVER IF NOT EXISTS import_server FOREIGN DATA WRAPPER file_fdw;

DROP FOREIGN TABLE IF EXISTS person;
DROP FOREIGN TABLE IF EXISTS follows;
DROP FOREIGN TABLE IF EXISTS hashtag;
DROP FOREIGN TABLE IF EXISTS interested_in;
DROP GRAPH social_network cascade;

CREATE FOREIGN TABLE IF NOT EXISTS person(
        person_id int,
        gender char(1),
        date_of_brith date,
        firstname varchar(20),
        lastname varchar(20),
        nationality varchar(20),
        email varchar(50)
        )
SERVER import_server
OPTIONS (FORMAT 'csv', HEADER 'true', FILENAME :p, DELIMITER '|');


CREATE FOREIGN TABLE IF NOT EXISTS follows(
        _from int,
        _to int,
        created_time timestamp
        )
SERVER import_server
OPTIONS (FORMAT 'csv', HEADER 'true', FILENAME :pfp, DELIMITER '|');


CREATE FOREIGN TABLE IF NOT EXISTS hashtag(
        tag_id int,
        content varchar(30)
        )
SERVER import_server
OPTIONS (FORMAT 'csv', HEADER 'true', FILENAME :h , DELIMITER ',');


CREATE FOREIGN TABLE IF NOT EXISTS interested_in(
        _from int,
        _to int,
        created_time char(20)
        )
SERVER import_server
OPTIONS (FORMAT 'csv', HEADER 'true', FILENAME :pih, DELIMITER ',');


CREATE GRAPH social_network;

