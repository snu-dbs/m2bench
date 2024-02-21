USE Healthcare;

LOAD DATA LOCAL INFILE '../../../Datasets/healthcare/table/Prescription.csv' 
INTO TABLE Prescription
FIELDS TERMINATED BY ',' 
ENCLOSED BY '"'
LINES TERMINATED BY '\n'
IGNORE 1 ROWS;

LOAD DATA LOCAL INFILE '../../../Datasets/healthcare/table/Diagnosis.csv' 
INTO TABLE Diagnosis
FIELDS TERMINATED BY ',' 
ENCLOSED BY '"'
LINES TERMINATED BY '\n'
IGNORE 1 ROWS;

LOAD DATA LOCAL INFILE '../../../Datasets/healthcare/table/Patient.csv' 
INTO TABLE Patient
FIELDS TERMINATED BY ',' 
ENCLOSED BY '"'
LINES TERMINATED BY '\n'
IGNORE 1 ROWS;

