USE Ecommerce;

LOAD DATA LOCAL INFILE '../../../Datasets/ecommerce/property_graph/person_node.csv'
INTO TABLE Person
FIELDS TERMINATED BY '|'
ENCLOSED BY '"'
LINES TERMINATED BY '\n'
IGNORE 1 ROWS;

LOAD DATA LOCAL INFILE '../../../Datasets/ecommerce/property_graph/person_follows_person.csv'
INTO TABLE Follows
FIELDS TERMINATED BY '|'
ENCLOSED BY '"'
LINES TERMINATED BY '\n'
IGNORE 1 ROWS;

LOAD DATA LOCAL INFILE '../../../Datasets/ecommerce/property_graph/hashtag_node.csv'
INTO TABLE Hashtag
FIELDS TERMINATED BY ','
ENCLOSED BY '"'
LINES TERMINATED BY '\n'
IGNORE 1 ROWS;

LOAD DATA LOCAL INFILE '../../../Datasets/ecommerce/property_graph/person_interestedIn_tag.csv'
INTO TABLE Interested_in
FIELDS TERMINATED BY ','
ENCLOSED BY '"'
LINES TERMINATED BY '\n'
IGNORE 1 ROWS;
