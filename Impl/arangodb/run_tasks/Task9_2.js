db._useDatabase("Healthcare")

res1 = db._query("Let A = (for d in Drug for ae in d.adverse_effect_list return DISTINCT {drug: d.drug_id, adverse_effect: ae.adverse_effect_name, is_adverse_effect: 1}) For a in A INSERT {drug: a.drug, adverse_effect: a.adverse_effect, is_adverse_effect: a.is_adverse_effect} INTO drug_matrix").getExtra();
res2 = db._query("for a in drug_matrix for b in drug_matrix filter a.adverse_effect == b.adverse_effect collect row=a.drug, col=b.drug aggregate res = sum(a.is_adverse_effect*b.is_adverse_effect) INSERT {drug1: row, drug2: col, val: res} INTO similarity1").getExtra();
res3 = db._query("for s in similarity1 filter s.drug1 == s.drug2 INSERT {drug1: s.drug1, drug2: s.drug2, val: 1/sqrt(s.val)} INTO inv_norm").getExtra();
res4 = db._query("for a in similarity1 for b in inv_norm filter a.drug2 == b.drug1 collect row=a.drug1, col=b.drug2 aggregate res = sum(a.val*b.val) INSERT {drug1: row, drug2: col, val: res} INTO similarity2").getExtra()
res5 = db._query("for a in similarity2 for b in inv_norm filter a.drug1 == b.drug1 collect row=a.drug2, col=b.drug2 aggregate res = sum(a.val*b.val) INSERT {drug1: row, drug2: col, val: res} INTO drug_similarity").getExtra()
res6 = db._query("Let A=(for p in Prescription filter p.patient_id == 9 return DISTINCT {drug: p.drug_id}) LET R =(for a in A for ds in drug_similarity filter ds.drug1 == a.drug return ds) RETURN length(R)")

res7 = res6.getExtra()

print(res6)
print(res1['stats']['executionTime']+res2['stats']['executionTime']+res3['stats']['executionTime']+res4['stats']['executionTime']+res5['stats']['executionTime']+res7['stats']['executionTime'])
