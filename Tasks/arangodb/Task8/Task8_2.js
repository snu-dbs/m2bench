db._useDatabase("Healthcare")

res1 = db._query('For drug in Drug INSERT {_key:TO_STRING(drug.drug_id), drug_name:drug.drug_name} INTO drug_temp').getExtra();
res2 = db._query('Let B = (For drug in Drug For target in drug.targets RETURN DISTINCT {target_id: target.id, target_name: target.name}) For b in B INSERT {_key: TO_STRING(b.target_id), target_name:b.target_name} INTO target_temp').getExtra();  
res3 = db._query('For drug in Drug For target in drug.targets For dt in drug_temp For tt in target_temp FILTER dt._key == TO_STRING(drug.drug_id) and tt._key == TO_STRING(target.id) INSERT {_from: dt._id, _to: tt._id} INTO has_bond').getExtra();
res4 = db._query('LET T = (For p in Prescription FILTER p.patient_id == 18 RETURN DISTINCT {drug_id: p.drug_id}) LET R = (For drug in drug_temp For target in target_temp For p in T FILTER TO_STRING(p.drug_id) == drug._key For v1,e1 in 1..1 Any drug has_bond FILTER target._key == v1._key For v2,e2 in 1..1 Any target has_bond FILTER drug._key != v2._key COLLECT drug1=drug.drug_name, drug2=v2.drug_name aggregate common_target = LENGTH(1) sort common_target desc RETURN {drug1, drug2, common_target}) RETURN LENGTH(R)');

res5 = res4.getExtra();

print(res4)

print(res1['stats']['executionTime'] + res2['stats']['executionTime'] + res3['stats']['executionTime'] + res5['stats']['executionTime'])
