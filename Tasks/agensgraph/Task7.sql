SET graph_path = disease_network;
\timing
WITH 
A as (select disease_id as disease_id from diagnosis where patient_id = 18),

B as (SELECT distinct(d3->'disease_id')::text::bigint as disease_id from 
	(MATCH (d1:disease)-[:is_a]->(d2:disease)<-[:is_a]-(d3:disease)
	where   d1.disease_id in (Select to_jsonb(disease_id) from A)
	RETURN   d3 ) as temp),
C as (Select distinct(patient_id) as patient_id from diagnosis, B 
	WHERE diagnosis.disease_id = B.disease_id AND diagnosis.patient_id != 18
	 and B.disease_id not in (select * from A)  
	
	) 
SELECT gender, count(gender) FROM patient, C WHERE patient.patient_id = C.patient_id GROUP BY gender; 

