USING PERIODIC COMMIT
LOAD CSV with headers FROM 'file:///person_node.csv' AS row  FIELDTERMINATOR '|'
CREATE(:Person {person_id: row.person_id, gender: row.gender, date_of_birth: row.date_of_birth, firstname: row.firstname, lastname: row.lastname, nationality: row.nationality, email: row.email});

USING PERIODIC COMMIT
LOAD CSV with headers FROM 'file:///hashtag_node.csv' AS row FIELDTERMINATOR ','
CREATE(:Shopping_Hashtag { tag_id: row.tag_id, content: row.content});


CREATE INDEX idx_person_id for (x:Person) on (x.person_id);
CREATE INDEX idx_hashtag_id for (x:Shopping_Hashtag) on (x.tag_id);


USING PERIODIC COMMIT
LOAD CSV with headers FROM 'file:///person_interestedIn_tag.csv' AS row  FIELDTERMINATOR ','
MATCH (c1 : Person {person_id: row.person_id} ), (c2: Shopping_Hashtag {tag_id: row.tag_id})
CREATE (c1)-[:interested_in {created_date: row.created_time}]->(c2);

USING PERIODIC COMMIT
LOAD CSV with headers FROM 'file:///person_follows_person.csv' AS row  FIELDTERMINATOR '|'
MATCH (c1 : Person {person_id: row._from} ), (c2: Person {person_id: row._to})
CREATE (c1)-[:follows {created_date : row.created_time}]->(c2);

