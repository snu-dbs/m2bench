db.Prescription.ensureIndex({ type: "persistent", fields: [ "patient_id" ] })
db.Patient.ensureIndex({ type: "persistent", fields : ["patient_id"] })
db.Diagnosis.ensureIndex({ type: "persistent", fields: ["patient_id"]})
db.Diagnosis.ensureIndex({ type: "persistent", fields: ["disease_id"]})
