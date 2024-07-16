\c healthcare;
  
DROP TABLE IF EXISTS Prescription;
DROP TABLE IF EXISTS Diagnosis;
DROP TABLE IF EXISTS Patient;


CREATE TABLE IF NOT EXISTS Patient (
        patient_id INT PRIMARY KEY,
        patient_name VARCHAR(50),
        gender CHAR(1),
        date_of_birth DATE,
        date_of_death DATE
        );

CREATE TABLE IF NOT EXISTS Diagnosis (
        patient_id INT,
        disease_id BIGINT
        );

CREATE TABLE IF NOT EXISTS Prescription (
        patient_id INT,
        drug_id INT,
        startdate DATE,
        enddate DATE
        );
