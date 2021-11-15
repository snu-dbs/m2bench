\c ecommerce;
  
SET graph_path = social_network;

--load vertices
LOAD FROM person AS source CREATE (n:person=to_jsonb(source));
CREATE PROPERTY INDEX ON person(person_id);

LOAD FROM hashtag AS source CREATE (n:hashtag=to_jsonb(source));
CREATE PROPERTY INDEX ON hashtag(tag_id);

--load edges
LOAD FROM follows AS source
MATCH (n:person), (m:person)
WHERE n.person_id = to_jsonb((source)._from) AND m.person_id = to_jsonb((source)._to)
CREATE (n)-[r:follows {created_time: (source).created_time}]->(m);

LOAD FROM interested_in AS source
MATCH (n:person), (m:hashtag)
WHERE n.person_id = to_jsonb((source)._from) AND m.tag_id = to_jsonb((source)._to)
CREATE (n)-[r:interested_in {created_time: (source).created_time}]->(m);
