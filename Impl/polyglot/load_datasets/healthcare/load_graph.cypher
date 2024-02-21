USING PERIODIC COMMIT
LOAD CSV with headers FROM 'file:///Disease_network_nodes.csv' AS row FIELDTERMINATOR ','
CREATE(:Disease { disease_id: row.disease_id, term: row.term});

// comment the below line out if it is not a first trial and run "DROP INDEX idx_disease_id;"
CREATE INDEX idx_disease_id FOR (x:Disease) ON (x.disease_id);
CALL db.awaitIndex('idx_disease_id');

USING PERIODIC COMMIT
LOAD CSV with headers FROM 'file:///Disease_network_edges.csv' AS row FIELDTERMINATOR ','
MATCH (c1 : Disease {disease_id: row.source_id} ), (c2: Disease {disease_id: row.destination_id})
CREATE (c1)-[:is_a]->(c2);
