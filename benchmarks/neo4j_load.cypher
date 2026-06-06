// Canonical LDBC -> Neo4j load (constraints + LOAD CSV). The CSVs must already be in
// Neo4j's import dir (neo4j_load.sh / prepare_ldbc.py copy them there). Used both for the
// one-off `prepare_ldbc.py --neo4j-load` and for the benchmark's timed LOAD step.
CREATE CONSTRAINT person_id IF NOT EXISTS FOR (p:Person)  REQUIRE p.id IS UNIQUE;
CREATE CONSTRAINT msg_id    IF NOT EXISTS FOR (m:Message) REQUIRE m.id IS UNIQUE;
CREATE CONSTRAINT place_id  IF NOT EXISTS FOR (pl:Place)  REQUIRE pl.id IS UNIQUE;

LOAD CSV WITH HEADERS FROM 'file:///place.csv' AS row
CALL { WITH row CREATE (:Place {id: toInteger(row.pl_placeid), name: row.pl_name,
       type: row.pl_type, containerId: toInteger(row.pl_containerplaceid)}) } IN TRANSACTIONS OF 10000 ROWS;

MATCH (city:Place) WHERE city.containerId IS NOT NULL
CALL { WITH city MATCH (country:Place {id: city.containerId})
       MERGE (city)-[:IS_PART_OF]->(country) } IN TRANSACTIONS OF 10000 ROWS;

LOAD CSV WITH HEADERS FROM 'file:///person.csv' AS row
CALL { WITH row CREATE (:Person {id: toInteger(row.p_personid), firstName: row.p_firstname,
       lastName: row.p_lastname, gender: row.p_gender, birthday: toInteger(row.p_birthday),
       creationDate: toInteger(row.p_creationdate), locationIP: row.p_locationip,
       browserUsed: row.p_browserused, placeId: toInteger(row.p_placeid)}) } IN TRANSACTIONS OF 10000 ROWS;

MATCH (p:Person)
CALL { WITH p MATCH (city:Place {id: p.placeId}) MERGE (p)-[:IS_LOCATED_IN]->(city) } IN TRANSACTIONS OF 10000 ROWS;

LOAD CSV WITH HEADERS FROM 'file:///message.csv' AS row
CALL { WITH row CREATE (:Message {id: toInteger(row.m_messageid),
       creationDate: toInteger(row.m_creationdate), creatorId: toInteger(row.m_creatorid),
       locationId: toInteger(row.m_locationid),
       replyOf: CASE row.m_c_replyof WHEN '' THEN null ELSE toInteger(row.m_c_replyof) END}) } IN TRANSACTIONS OF 10000 ROWS;

MATCH (m:Message)
CALL { WITH m MATCH (p:Person {id: m.creatorId}) MERGE (m)-[:HAS_CREATOR]->(p) } IN TRANSACTIONS OF 10000 ROWS;
MATCH (m:Message)
CALL { WITH m MATCH (pl:Place {id: m.locationId}) MERGE (m)-[:IS_LOCATED_IN]->(pl) } IN TRANSACTIONS OF 10000 ROWS;
MATCH (c:Message) WHERE c.replyOf IS NOT NULL
CALL { WITH c MATCH (m:Message {id: c.replyOf}) MERGE (c)-[:REPLY_OF]->(m) } IN TRANSACTIONS OF 10000 ROWS;

LOAD CSV WITH HEADERS FROM 'file:///knows.csv' AS row
WITH row WHERE toInteger(row.k_person1id) < toInteger(row.k_person2id)
CALL { WITH row MATCH (a:Person {id: toInteger(row.k_person1id)}), (b:Person {id: toInteger(row.k_person2id)})
       MERGE (a)-[:KNOWS {creationDate: toInteger(row.k_creationdate)}]->(b) } IN TRANSACTIONS OF 10000 ROWS;