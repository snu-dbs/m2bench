USING PERIODIC COMMIT
LOAD CSV with headers FROM 'file:///Roadnode.csv' AS row  FIELDTERMINATOR ','
CREATE(:Roadnode {roadnode_id: toInteger(row.id), latitude: toFloat(row.latitude), longitude: toFloat(row.longitude)});

CREATE INDEX idx_roadnode_id for (x:Roadnode) on (x.roadnode_id);  

USING PERIODIC COMMIT
LOAD CSV with headers FROM 'file:///Road.csv' AS row  FIELDTERMINATOR ','
MATCH (c1 : Roadnode {roadnode_id: toInteger(row.from)} ), (c2: Roadnode {roadnode_id: toInteger(row.to)})
CREATE (c1)-[:Road { distance: toInteger(row.distance)} ]->(c2);

CALL gds.graph.create(
  'road_network',            
  'Roadnode',             
  'Road' ,
   {
       relationshipProperties: 'distance'
   }
);