CREATE DATABASE IF NOT EXISTS Healthcare;
  

USE Healthcare;

DROP TABLE IF EXISTS Prescription;
DROP TABLE IF EXISTS Diagnosis;
DROP TABLE IF EXISTS Patient;


CREATE TABLE IF NOT EXISTS Patient (
        patient_id INT PRIMARY KEY,
        patient_name VARCHAR(20),
        gender CHAR(1),
        date_of_birth DATE,
        date_of_death DATE
);

CREATE TABLE IF NOT EXISTS Diagnosis (
        patient_id INT,
        disease_id BIGINT,

        index (disease_id),
        index (patient_id),

        FOREIGN KEY (patient_id)
                REFERENCES Patient (patient_id)
                ON UPDATE RESTRICT ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS Prescription (
        patient_id INT,
        drug_id INT,
        startdate DATE,
        enddate DATE,

        index (patient_id),

        FOREIGN KEY (patient_id)
                REFERENCES Patient (patient_id)
                ON UPDATE RESTRICT ON DELETE CASCADE
        );
