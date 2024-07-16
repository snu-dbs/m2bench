// remove all nodes and edges first (maybe indexes can be used but it is fine)
MATCH (n:Disease) DETACH DELETE n;

// create nodes
USING PERIODIC COMMIT
LOAD CSV with headers FROM 'file:///Disease_network_nodes.csv' AS row FIELDTERMINATOR ','
CREATE(:Disease { disease_id: row.disease_id, term: row.term});

// create index
CREATE INDEX idx_disease_id FOR (x:Disease) ON (x.disease_id);
CALL db.awaitIndexes();

// create edges
USING PERIODIC COMMIT
LOAD CSV with headers FROM 'file:///Disease_network_edges.csv' AS row FIELDTERMINATOR ','
MATCH (c1 : Disease {disease_id: row.source_id} ), (c2: Disease {disease_id: row.destination_id})
CREATE (c1)-[:is_a]->(c2);
