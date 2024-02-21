\c healthcare;
  
COPY Patient FROM '/tmp/m2bench/healthcare/table/Patient.csv' DELIMITER ',' CSV HEADER;

COPY Prescription FROM '/tmp/m2bench/healthcare/table/Prescription.csv' DELIMITER ',' CSV HEADER;

COPY Diagnosis FROM '/tmp/m2bench/healthcare/table/Diagnosis.csv' DELIMITER ',' CSV HEADER;


Create index diagnosis_pi_idx on diagnosis(patient_id);
create index diagnosis_di_idx on diagnosis(disease_id);
create index prescription_pi_idx on prescription(patient_id);

