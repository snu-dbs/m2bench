// remove all nodes and edges first (maybe indexes can be used but it is fine)
MATCH (n:Roadnode) DETACH DELETE n;

// import nodes
USING PERIODIC COMMIT
LOAD CSV with headers FROM 'file:///Roadnode.csv' AS row  FIELDTERMINATOR ','
CREATE(:Roadnode {roadnode_id: toInteger(row.roadnode_id), site_id: toInteger(row.site_id)});

// create indexes
CREATE INDEX idx_roadnode_id for (x:Roadnode) on (x.roadnode_id);  
CREATE INDEX idx_site_id for (x:Roadnode) on (x.site_id);  
CALL db.awaitIndexes();

// import edges
USING PERIODIC COMMIT
LOAD CSV with headers FROM 'file:///Road.csv' AS row  FIELDTERMINATOR ','
MATCH (c1 : Roadnode {roadnode_id: toInteger(row._from)} ), (c2: Roadnode {roadnode_id: toInteger(row._to)})
CREATE (c1)-[:Road { distance: toInteger(row.distance)} ]->(c2);

// gds call
CALL gds.graph.create(
  'road_network',            
  'Roadnode',             
  'Road' ,
   {
       relationshipProperties: 'distance'
   }
);