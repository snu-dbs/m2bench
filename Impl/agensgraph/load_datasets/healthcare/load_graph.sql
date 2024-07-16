\c healthcare
  
SET graph_path = disease_network;

LOAD FROM Disease AS source CREATE (n:Disease=to_jsonb(source));
CREATE PROPERTY INDEX ON Disease(disease_id);

LOAD FROM Is_a AS source
MATCH (n:Disease),(m:Disease)
WHERE n.disease_id=to_jsonb((source).source_id)
AND m.disease_id=to_jsonb((source).destination_id)
CREATE (n)-[r:is_a]->(m);
