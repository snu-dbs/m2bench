db._useDatabase("Healthcare")

res = db._query('Let A = (For diagnosis in Diagnosis Filter diagnosis.patient_id == 18 Return diagnosis.disease_id) Let B = (For a in A For disease in Disease_network_nodes FILTER TO_STRING(a) == disease._key For v1, e1 in 1..1 OUTBOUND disease Disease_network_edges For v2, e2 in 1..1 INBOUND v1 Disease_network_edges RETURN DISTINCT {disease_id: TO_NUMBER(v2._key)}) Let C = (For diagnosis in Diagnosis for b in B Filter diagnosis.disease_id == b.disease_id and diagnosis.patient_id != 18 and b.disease_id not in A RETURN distinct {patient_id: diagnosis.patient_id}) For p in Patient For c in C Filter c.patient_id == p.patient_id COLLECT gender=p.gender aggregate count = LENGTH(1) RETURN {gender, count}').getExtra();

print(res['stats']['executionTime'])
