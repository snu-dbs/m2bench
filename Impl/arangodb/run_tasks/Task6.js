res = db._query(`
	Let param_patient_id = 18 
	Let A = (For prescription in Prescription
		Filter prescription.patient_id == param_patient_id
		Collect drug_id = prescription.drug_id
		Return {drug_id}
	)
	Let B= (For a in A
		For drug in Drug
		Filter a.drug_id == drug.drug_id
		For elem in drug.drug_interaction_list
		collect drug_name = elem.interaction_drug.drug_name
		Return drug_name)
	Return Length(B)`);
res2 = res.getExtra();
print (res)
print ( res2['stats']['executionTime'] )

