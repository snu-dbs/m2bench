USING PERIODIC COMMIT
LOAD CSV with headers FROM 'file:///Disease_network_edges.csv' AS row  FIELDTERMINATOR ','
MATCH (c1 : Disease {disease_id: row.source_id} ), (c2: Disease {disease_id: row.destination_id})
CREATE (c1)-[:is_a]->(c2);
