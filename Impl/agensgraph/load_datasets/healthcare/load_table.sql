\c healthcare;
  
COPY Patient FROM '/healthcare/table/Patient.csv' DELIMITER ',' CSV HEADER;

COPY Prescription FROM '/healthcare/table/Prescription.csv' DELIMITER ',' CSV HEADER;

COPY Diagnosis FROM '/healthcare/table/Diagnosis.csv' DELIMITER ',' CSV HEADER;


Create index diagnosis_pi_idx on diagnosis(patient_id);
create index diagnosis_di_idx on diagnosis(disease_id);
create index prescription_pi_idx on prescription(patient_id);

