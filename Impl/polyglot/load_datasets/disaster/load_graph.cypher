USING PERIODIC COMMIT
LOAD CSV with headers FROM 'file:///Roadnode.csv' AS row  FIELDTERMINATOR ','
CREATE(:Roadnode {roadnode_id: toInteger(row.roadnode_id), site_id: toInteger(row.site_id)});

// comment the below line out if it is not a first trial and run "DROP INDEX idx_roadnode_id;"
CREATE INDEX idx_roadnode_id for (x:Roadnode) on (x.roadnode_id);  
CALL db.awaitIndex('idx_roadnode_id');

USING PERIODIC COMMIT
LOAD CSV with headers FROM 'file:///Road.csv' AS row  FIELDTERMINATOR ','
MATCH (c1 : Roadnode {roadnode_id: toInteger(row._from)} ), (c2: Roadnode {roadnode_id: toInteger(row._to)})
CREATE (c1)-[:Road { distance: toInteger(row.distance)} ]->(c2);

CALL gds.graph.create(
  'road_network',            
  'Roadnode',             
  'Road' ,
   {
       relationshipProperties: 'distance'
   }
);