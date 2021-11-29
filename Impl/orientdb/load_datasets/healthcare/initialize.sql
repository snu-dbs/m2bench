CREATE CLASS Patient;
CREATE PROPERTY Patient.patient_id INTEGER;

CREATE CLASS Prescription;
CREATE PROPERTY Prescription.patient_id INTEGER;
CREATE PROPERTY Prescription.drug_id INTEGER;

CREATE CLASS Diagnosis;
CREATE PROPERTY Diagnosis.patient_id INTEGER;
CREATE PROPERTY Diagnosis.disease_id LONG;

CREATE CLASS Drug;
CREATE PROPERTY Drug.drug_id INTEGER;

CREATE CLASS Disease EXTENDS V;
CREATE PROPERTY Disease.disease_id LONG;

