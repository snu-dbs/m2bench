\timing
Create temporary table drug_temp as
(Select data->'drug_id' as drug_id, data->'drug_name' as drug_name From Drug);

Create temporary table target_temp as
(Select distinct(jsonb_array_elements(data->'targets')->'id') as target_id, jsonb_array_elements(data->'targets')->'name' as target_name From Drug);

Create temporary table has_bonds as
(Select data->'drug_id' as drug_id, jsonb_array_elements(data->'targets')->'id' as target_id From Drug);

DROP GRAPH drug_network CASCADE;

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

Select count(*) from
(
SELECT drug1, drug2, count(*) as common_target from
(MATCH (d1:drug_t)-[:has_bond]-(t:target_t)-[:has_bond]-(d2:drug_t)
WHERE d1.drug_id in (select distinct(to_jsonb(drug_id)) from Prescription where patient_id = 9)
RETURN d1.drug_name as drug1, d2.drug_name as drug2) as B
Group By drug1, drug2
Order By common_target DESC
) as res
