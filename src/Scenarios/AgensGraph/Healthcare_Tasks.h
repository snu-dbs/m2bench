//
// Created by mxmdb on 21. 6. 29..
//

#ifndef M2BENCH_AO_HEALTHCARE_TASKS_H
#define M2BENCH_AO_HEALTHCARE_TASKS_H

#endif //M2BENCH_AO_HEALTHCARE_TASKS_H

/**
 *
 * [Task6] Drug Interaction (R,D=>R)
 *  Find drugs that has known interaction with the prescribed drugs for a give patient
 *
 *      A: Select distinct(drug_id) as drug_id
 *          From Prescription
 *          Where patient_id = @param
 *
 *      B: Select interaction_drug.drug_name
 *          From Drug, A
 *          Unnest Drug.drug_interaction_list as interaction_drug
 *          Where A.drug_id == Drug.drug_id
 *          Group by interaction_drug.name
 *
 *      @param patient_id
 *

       With A as (
            Select distinct(drug_id) as drug_id
            From Prescription
            Where patient_id = 9
         )

        Select count(*)
        from
         (
            Select distinct(jsonb_array_elements(data->'drug_interaction_list')->'interaction_drug'->>'drug_name')
            From Drug, A
            Where A.drug_id = (data->>'drug_id')::INT
          ) as B
 *
 */

/**
 * [Task 7] Group of patients with similar disease (RxG=>R)
 * Find the number of female and male patients suffering from a similar disease with a given patient.
 *
 * A: SELECT disease_id FROM Diagnosis WHERE patient_id = @param
 * B: SELECT d3.disease_id FROM Disease Network, A
 *    WHERE (d1: Disease) - [:is_a] -> (d2: Disease) <- [:is_a] - (d3: Disease) AND d1.disease_id in A AND d3.disease_id not in A //table
 * C: SELECT patient_id FROM Diagnosis, B
 *    WHERE Diagnosis.disease_id = B.disease_id AND Diagnosis.patient_id != @param
 * D: SELECT gender, count(gender) FROM Patient, C WHERE Patient.patient_id = C.patient_id GROUP BY gender
 *
 * @param patient_id
 */

/*
 *
 *
 *
 *
   SET graph_path = disease_network;

   SELECT gender, count(gender) from Patient,
   (SELECT distinct diagnosis.patient_id from diagnosis,
   (MATCH (d1:disease)-[:is_a]->(d2:disease)<-[:is_a]-(d3:disease)
   WHERE d1.disease_id in (select to_jsonb(disease_id) from diagnosis where patient_id = 9)
   RETURN distinct d3.disease_id) as B
   WHERE to_jsonb(diagnosis.disease_id) = B.disease_id AND diagnosis.patient_id != 9
   AND diagnosis.disease_id not in (select disease_id from diagnosis where patient_id = 9)) as C
   WHERE Patient.patient_id = C.patient_id GROUP BY gender;
 *
 */

/**
 * [Task 8] Potential drug interaction (RxD=>R)
 * Find all potential interaction drugs with the given patient's prescription drug
 *
 * A: SELECT drug_id, drug_name FROM Drug
 * B: SELECT d.targets.id, d.targets.name FROM Drug UNNEST targets d
 * C: SELECT d.drug_id, d.target.id FROM Drug UNNEST targets d
 * D: Drug.toGraph(Node : A,B {label: drug, target} edge : C {label: has_bond} )//graph
 * E: SELECT d1.drug_name, d2.drug_name, count(t) AS common_target FROM D, Prescription
 *      WHERE (d1:drug)-[:has_bond]-(t:target)-[:has_bond]-(d2:drug)
 *      AND Prescription.patient_id = @param AND Prescription.drug_id = d1.drug_id
 *      GROUP BY d1.drug_name, d2.drug_name ORDER BY common_target DESC //table
 *
 * @param patient_id
 */

/*
    Create temporary table drug_temp as
    (Select data->'drug_id' as drug_id, data->'drug_name' as drug_name From Drug);

    Create temporary table target_temp as
    (Select distinct(jsonb_array_elements(data->'targets')->'id') as target_id, jsonb_array_elements(data->'targets')->'name' as target_name From Drug);

    Create temporary table has_bonds as
    (Select data->'drug_id' as drug_id, jsonb_array_elements(data->'targets')->'id' as target_id From Drug);

    Create GRAPH drug_network;

    SET graph_path = drug_network;

    LOAD FROM drug_temp AS source CREATE (n:drug_t=to_jsonb(source));
    CREATE PROPERTY INDEX ON drug_t(drug_id);

    LOAD FROM target_temp AS source CREATE (n:target_t=to_jsonb(source));
    CREATE PROPERTY INDEX ON target_t(target_id);

    LOAD FROM has_bonds AS source
    MATCH (n:drug_t), (m:target_t)
    WHERE n.drug_id=(source).drug_id
    AND m.target_id=(source).target_id
    CREATE (n)-[r:has_bond]->(m);

    SELECT drug1, drug2, count(*) as common_target from
    (MATCH (d1:drug_t)-[:has_bond]-(t:target_t)-[:has_bond]-(d2:drug_t)
    WHERE d1.drug_id in (select distinct(to_jsonb(drug_id)) from Prescription where patient_id = 9)
    RETURN d1.drug_name as drug1, d2.drug_name as drug2) as B
    Group By drug1, drug2
    Order By common_target DESC;
 */

/**
 *  [Task9] Drug similarity (R,D=>A)
 *  Find similar drugs for a given patient X's prescribed drug
 *
 *  A: SELECT drug_id as drug, adverse_effect_list.adverse_effect_name as adverse_effect, 1 as is_adverse_effect
 *          FROM Drug
 *          UNNEST adverse_effect_list //table
 *
 *  B: A.toArray  -> (<is_adverse_effect>[drug, adverse_effect]) //array
 *  C: Cosine_similarity(B) -> (<similarity coefficient>[drug1, drug2]) //array
 *  D: SELECT * FROM C WHERE drug1 in (Select drug_id from Prescription where patient_id = X)  //array
 *
 */

/*

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

WITH A as (SELECT distinct(drug_id) as drug from Prescription where patient_id = 9)
SELECT drug_1, drug_2, val  from drug_similarity, A where drug_similarity.drug_1 = to_jsonb(A.drug);
 */

