CREATE LINK patient TYPE LINK FROM Prescription.patient_id TO Patient.patient_id;
CREATE LINK patient TYPE LINK FROM Diagnosis.patient_id TO Patient.patient_id;
CREATE LINK drug TYPE LINK FROM Prescription.drug_id TO Drug.drug_id;
CREATE LINK disease TYPE LINK FROM Diagnosis.disease_id TO Disease.disease_id;

