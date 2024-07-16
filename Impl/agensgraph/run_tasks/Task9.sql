\timing
Create temporary table drug_matrix as
(Select data->'drug_id' as drug, jsonb_array_elements(data->'adverse_effect_list')->'adverse_effect_name' as adverse_effect, 1 as is_adverse_effect From Drug
Group by drug, adverse_effect);

CREATE INDEX on drug_matrix (drug);
CREATE INDEX on drug_matrix (adverse_effect);

Create temporary table similarity1 as
(Select a.drug as drug1, b.drug as drug2, sum(a.is_adverse_effect*b.is_adverse_effect) as val
from drug_matrix a, drug_matrix b
where a.adverse_effect = b.adverse_effect
group by drug1, drug2);

CREATE INDEX on similarity1 (drug1);
CREATE INDEX on similarity1 (drug2);

Create temporary table inv_norm as
(Select drug1, drug2, 1/sqrt(val) as val
from similarity1
where drug1 = drug2);

CREATE INDEX on inv_norm (drug1);
CREATE INDEX on inv_norm (drug2);

Create temporary table similarity2 as
(Select similarity1.drug1 as d1, inv_norm.drug2 as d2, sum(similarity1.val*inv_norm.val) as val
from similarity1, inv_norm
where similarity1.drug2 = inv_norm.drug1
group by d1, d2);

CREATE INDEX on similarity2 (d1);
CREATE INDEX on similarity2 (d2);

Create temporary table drug_similarity as
(Select similarity2.d2 as drug_1, inv_norm.drug2 as drug_2, sum(similarity2.val*inv_norm.val) as val
from similarity2, inv_norm
where similarity2.d1 = inv_norm.drug1
group by drug_1, drug_2);

CREATE INDEX on drug_similarity (drug_1);
CREATE INDEX on drug_similarity (drug_2);

select count(*) from
(
WITH A as (SELECT distinct(drug_id) as drug from Prescription where patient_id = 9)
SELECT drug_1, drug_2, val  from drug_similarity, A where drug_similarity.drug_1 = to_jsonb(A.drug)
) as res
