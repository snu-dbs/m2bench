\c disaster
  
SET graph_path=Road_network;

LOAD FROM Roadnodes AS source CREATE (n:Roadnode=to_jsonb(source));
CREATE PROPERTY INDEX ON Roadnode(roadnode_id);
CREATE PROPERTY INDEX ON Roadnode(site_id);

LOAD FROM Roads AS source
MATCH (n:Roadnode),(m:Roadnode)
WHERE n.roadnode_id=to_jsonb((source)._from)
AND m.roadnode_id=to_jsonb((source)._to)
CREATE (n)-[r:Road {distance:(source).distance}]->(m);
