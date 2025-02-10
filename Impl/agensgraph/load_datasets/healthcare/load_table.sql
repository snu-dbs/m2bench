\c healthcare;

COPY Patient FROM '/tmp/m2bench/healthcare/table/Patient.csv' DELIMITER ',' CSV HEADER;
COPY Prescription FROM '/tmp/m2bench/healthcare/table/Prescription.csv' DELIMITER ',' CSV HEADER;
COPY Diagnosis FROM '/tmp/m2bench/healthcare/table/Diagnosis.csv' DELIMITER ',' CSV HEADER;

CREATE INDEX diagnosis_pi_idx ON Diagnosis(patient_id);
CREATE INDEX diagnosis_di_idx ON Diagnosis(disease_id);
CREATE INDEX prescription_pi_idx ON Prescription(patient_id);