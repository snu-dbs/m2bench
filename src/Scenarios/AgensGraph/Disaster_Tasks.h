//
// Created by mxmdb on 21. 6. 29..
//

#ifndef M2BENCH_AO_DISASTER_TASKS_H
#define M2BENCH_AO_DISASTER_TASKS_H

#endif //M2BENCH_AO_DISASTER_TASKS_H


/** [Task 10] Road Network Filtering ([R, D, G]=> G)
* For the earthquakes which occurred between time Z1 and Z2, find the road network subgraph within 5km from the earthquakes' location.
*
* A = SELECT n1, r, n2 AS subgraph FROM Earthquake, Site, RoadNode
*     WHERE (n1:RoadNode) - [r:Road] -> (n2:RoadNode) AND ST_Distance(Site.geometry, Earthquake.coordinates) <= 5km
*     AND Earthquake.time >= Z1 AND Earthquake.time < Z2 AND RoadNode.site_id = Site.site_id  //Graph


    SET graph_path = Road_network;

    WITH eqk AS (
            SELECT earthquake_id AS id, time, ST_MakePoint(ST_X(coordinates), ST_Y(coordinates)) AS geom
            FROM earthquake
            WHERE time >= to_timestamp('2020-06-01 00:00:00' , 'YYYY-MM-DD HH24:MI:SS') AND time < to_timestamp('2020-06-01 02:00:00', 'YYYY-MM-DD HH24:MI:SS')
    ),
    roadnodes AS (
            SELECT eqk.id AS eqk_id, data->'site_id' AS site_id
            FROM eqk, site
            WHERE site.data->'properties'->>'type'='roadnode' AND ST_DistanceSphere(eqk.geom, ST_GeomFromGeoJSON(site.data->>'geometry')) <= 5000
    )
            SELECT COUNT(subgraph)
            FROM roadnodes, (MATCH (n:roadnode)-[r:road]->(m:roadnode) RETURN n,r,m) AS subgraph
            WHERE subgraph.n->'site_id'=roadnodes.site_id;
*/


/**
  *  [Task 11] Closest Shelter ([R, D, G]=> R)
  *  For a given earthquake information X, find the cost of the shortest path for each GPS coordinate and Shelter pair.
  *  (GPS coordinates are limited by 1 hour and 10km from the X. Shelters are limited by 15km from the X.)
  *
  *  A = SELECT GPS.gps_id, ST_ClosestObject(Site, roadnode, GPS.coordinates) AS roadnode_id FROM GPS, Site, RoadNode
  *      WHERE GPS.time >= X.time AND GPS.time < X.time + 1 hour AND ST_Distance(GPS.coordinates, X.coordinates) <= 10km
  *      AND RoadNode.site_id = Site.site_id //Relational
  *
  *  B = SELECT t.shelter_id, ST_ClosestObject(Site, roadnode, ST_Centroid(t.geometry)) AS roadnode_id
  *      FROM RoadNode, Site, (SELECT Shelter.shelter_id, Site.geometry FROM Site, Shelter WHERE ST_Distance(ST_Centroid(Site.geometry), X.coordinates) <= 15km AND Shelter.site_id = Site.site_id) AS t
  *      WHERE RoadNode.site_id = Site.site_id  //Relational
  *
  *  C = SELECT A.gps_id, B.shelter_id, ShortestPath(RoadNode, startNode:A.roadnode_id, endNode:B.roadnode_id) AS cost
  *      FROM A, B, RoadNode //Relational


    SELECT time, ST_MakePoint(ST_X(coordinates), ST_Y(coordinates)) AS geom INTO TEMP TABLE eqk_x FROM earthquake WHERE earthquake_id=41862;
    SET graph_path = Road_network;
    WITH A AS (
      SELECT gps.gps_id AS gps_id, ST_MakePoint(ST_X(gps.coordinates), ST_Y(gps.coordinates)) AS geom
      FROM gps, eqk_x
      WHERE gps.time >= eqk_X.time AND gps.time < eqk_X.time + interval '1 hour'
      AND ST_DistanceSphere(eqk_X.geom, ST_MakePoint(ST_X(gps.coordinates), ST_Y(gps.coordinates)))<=10000
      ),
      gps_nodes AS (
        SELECT t1.gps_id AS gps_id, t2.site_id AS site_id
        FROM A t1 LEFT JOIN LATERAL (SELECT site.data->'site_id' AS site_id FROM site WHERE site.data->'properties'->>'type'='roadnode' ORDER BY ST_DistanceSphere(t1.geom, ST_Centroid(ST_GeomFromGeoJSON(site.data->>'geometry'))) LIMIT 1) t2 on true
      ),
      B AS (
        SELECT shelter.shelter_id AS shelter_id, ST_Centroid(ST_GeomFromGeoJSON(site.data->>'geometry')) AS geom
        FROM eqk_x, shelter, site
        WHERE shelter.site_id = (site.data->>'site_id')::int AND ST_DistanceSphere(eqk_x.geom, ST_Centroid(ST_GeomFromGeoJSON(site.data->>'geometry'))) <=15000
      ),
      shelter_nodes AS (
        SELECT t1.shelter_id AS shelter_id, t2.site_id AS site_id
        FROM B t1 LEFT JOIN LATERAL (SELECT site.data->'site_id' AS site_id FROM site WHERE site.data->'properties'->>'type'='roadnode' ORDER BY ST_DistanceSphere(t1.geom, ST_Centroid(ST_GeomFromGeoJSON(site.data->>'geometry'))) LIMIT 1) t2 on true
      ),
      C AS (
        SELECT src, dest, (unnest(roads)->>'distance')::INT AS distance
        FROM gps_nodes AS src CROSS JOIN shelter_nodes AS dest
        CROSS JOIN lateral (MATCH (n: RoadNode), (m: RoadNode), path=DIJKSTRA((n)-[e:Road]->(m), e.distance)
                            WHERE n.site_id = (SELECT DISTINCT(gps_nodes.site_id) FROM gps_nodes WHERE gps_nodes.site_id = src.site_id)
                            AND m.site_id = (SELECT DISTINCT(shelter_nodes.site_id)
                            FROM shelter_nodes WHERE shelter_nodes.site_id = dest.site_id)
        RETURN n,m,edges(path) AS roads) AS graph
      )
      SELECT src, dest, SUM(distance) as total_cost
      FROM C
      GROUP BY src, dest;
**/


/**
 * [Task 12] New Shelter ([R, D, G]=> R)
 * For the shelter of which the number of GPS coordinates are the most within 5km from the shelter between time Z1 and Z2,
 * find five closest buildings from the shelter. The buildings are limited by 1km from the shelter.
 *
 * A = SELECT Shelter.shelter_id, Site.geometry
 *     FROM Shelter, Site, (SELECT Shelter.shelter_id, COUNT(GPS.gps_id) AS cnt FROM Shelter, GPS, Site
 *                          WHERE ST_Distance(GPS.coordinates, ST_Centroid(Site.geometry)) <= 5km AND Site.site_id = Shelter.site_id
 *                          AND GPS.time >= Z1 AND GPS.time <= Z2 GROUP BY Shelter.id ORDER BY cnt DESC LIMIT 1) AS t
 *     WHERE Shelter.shelter_id = t.shelter_id AND Site.site_id = Shelter.site_id  //Relational
 *
 * B = SELECT A.shelter_id, ST_ClosestObject(Site, roadnode, ST_centroid(A.geometry)) AS roadnode_id
 *     FROM A, RoadNode, Site
 *     WHERE Site.site_id = RoadNode.site_id  //Relational
 *
 * C = SELECT t.site_id, ST_ClosestObject(Site, roadnode, ST_Centroid(t.geometry)) AS roadnode_id
 *     FROM Site, RoadNode, (SELECT Site.site_id, Site.geometry FROM Site, A
 *                           WHERE ST_Distance(Site.geometry, ST_centroid(A.geometry)) <= 1km AND Site.properties.type = 'building') AS t
 *     WHERE RoadNode.site_id = Site.site_id  //Relational
 *
 * D = SELECT C.site_id, ShortestPath(RoadNode, startNode:B.roadnode_id, endNode:C.roadnode_id) AS cost
 *     FROM B, C, RoadNode
 *     ORDER BY cost LIMIT 5  //Relational


 SET graph_path = Road_network;
 WITH A AS (
        SELECT gps_id, ST_MakePoint(ST_X(gps.coordinates), ST_Y(gps.coordinates)) AS geom FROM gps
        WHERE gps.time >= to_timestamp('2020-09-17 00:00:00' , 'YYYY-MM-DD HH24:MI:SS') AND gps.time < to_timestamp('2020-09-17 01:00:00' , 'YYYY-MM-DD HH24:MI:SS')
  ),
	B AS (
		SELECT shelter.shelter_id AS shelter_id, shelter.site_id AS site_id, COUNT(A.gps_id) AS numGps
		FROM A, shelter, site
		WHERE (site.data->>'site_id')::INT = shelter.site_id AND ST_DistanceSphere(ST_Centroid(ST_GeomFromGeoJSON(site.data->>'geometry')), A.geom)<=5000
		GROUP BY shelter.shelter_id
		ORDER BY numGps DESC
		LIMIT 1
	),
	shelter_node AS (
		SELECT target_shelter.shelter_id AS shelter_id, site.data->'site_id' AS site_id, target_shelter.geom AS geom
		FROM (SELECT B.shelter_id AS shelter_id, ST_Centroid(ST_GeomFromGeoJSON(site.data->>'geometry')) AS geom FROM B, site WHERE B.site_id = (site.data->>'site_id')::INT) AS target_shelter, site
		WHERE site.data->'properties'->>'type'='roadnode'
		ORDER BY ST_DistanceSphere(target_shelter.geom, ST_Centroid(ST_GeomFromGeoJSON(site.data->>'geometry')))
		LIMIT 1
	),
	D AS (
		SELECT site.data->'site_id' AS building_id, ST_Centroid(ST_GeomFromGeoJSON(site.data->>'geometry')) AS geom
		FROM shelter_node, site WHERE site.data->'properties'->>'type' ='building' AND ST_DistanceSphere(geom, shelter_node.geom)<=1000
	),
	E AS (
		SELECT D.building_id AS building_id, site.data->'site_id' AS site_id, ST_DistanceSphere(D.geom, ST_Centroid(ST_GeomFromGeoJSON(site.data->>'geometry'))) AS dist
		FROM D, site WHERE site.data->'properties'->>'type'='roadnode'
	),
	building_node AS (
	SELECT t1.building_id, t1.site_id
	FROM E t1, (SELECT building_id, MIN(dist) as mindist FROM E GROUP BY building_id) t2
	WHERE t1.building_id = t2.building_id AND t1.dist = t2.mindist
	),
	G AS (
	SELECT source, dest, (unnest(e)->>'distance')::INT AS cost
	FROM shelter_node as source
	CROSS JOIN building_node as dest
	CROSS JOIN LATERAL  (MATCH (n: RoadNode), (m: RoadNode), path=DIJKSTRA((n)-[e:Road]->(m), e.distance)
						 WHERE n.site_id = (select distinct(shelter_node.site_id) from shelter_node where shelter_node.site_id = source.site_id )
						 AND m.site_id = (select distinct(building_node.site_id) from building_node where building_node.site_id = dest.site_id)
						 RETURN n,m, edges(path) AS e) AS graph
	)
	SELECT G.dest, SUM(G.cost) as total_cost FROM G
	GROUP BY G.dest
	ORDER BY total_cost;
 */


/*
 * [Task 13] Damage Statistics.
 *
 * For the earthquakes of which magnitude is greater than 4.5, find the building statistics.
 * The buildings are limited by 30km from the earthquake location. (Relational, Document) -> Document
 * 
 * [Query]
SELECT COUNT(*)
FROM (
  SELECT Site_centroid.data->'properties'->'description' AS description, COUNT(*)
  FROM Earthquake, Site_centroid
  WHERE ST_DWithin(ST_GeomFromGeoJSON(Site_centroid.data->>'centroid')::geography, Earthquake.coordinates::geography, 30000, false)
          AND Site_centroid.data->'properties'->>'type' = 'building'
          AND Earthquake.magnitude >= 4.5
  GROUP BY Site_centroid.data->'properties'->'description'
) AS T13;

 * [Optimized]
CREATE TEMP TABLE T13A (centroid geography, description text);

INSERT INTO T13A
SELECT ST_GeomFromGeoJSON(Site_centroid.data->>'centroid')::geography as centroid, Site_centroid.data->'properties'->'description' AS description
FROM Site_centroid
WHERE Site_centroid.data->'properties'->>'type' = 'building';

CREATE INDEX t13a_centroid ON T13A USING gist(centroid); 

SELECT COUNT(*)
FROM (
  SELECT T13A.description, COUNT(*)
  FROM Earthquake, T13A
  WHERE ST_DWithin(T13A.centroid, Earthquake.coordinates::geography, 30000, false)
      AND Earthquake.magnitude >= 4.5
  GROUP BY description
) AS T13;

DROP TABLE T13A;
 */


/* 
 * [Task 14] Sources of Fine Dust.
 *
 * Analyze fine dust hotspot by date between time Z1 and Z2.
 * Print the nearest building with time of the hotspot.
 * Use window aggregation with a size of 5. (Document, Array) -> Document
 * 
 * [Query]
CREATE TEMP TABLE T14A (date integer, timestamp integer, latitude integer, longitude integer, pm10_avg double precision);
CREATE TEMP TABLE T14C (date integer, timestamp integer, coordinates geometry);

INSERT INTO T14A
SELECT t1.timestamp / 8 as date, t1.timestamp, t1.latitude, t1.longitude, avg(t2.pm10) AS pm10_avg
FROM FineDust_idx as t1, FineDust_idx as t2
WHERE (:Z1 <= t1.timestamp) AND (t1.timestamp <= :Z2)
    AND (t1.timestamp = t2.timestamp)
    AND ((t1.latitude - 2) <= t2.latitude) AND (t2.latitude <= (t1.latitude + 2))
    AND ((t1.longitude - 2) <= t2.longitude) AND (t2.longitude <= (t1.longitude + 2))
GROUP BY t1.timestamp / 8, t1.timestamp, t1.latitude, t1.longitude;

INSERT INTO T14C
SELECT t1.date, t1.timestamp, ST_Point(-118.34501002237936 + (t1.longitude * 0.000216636), 34.011898718557454 + (t1.latitude * 0.000172998)) AS coordinates
FROM T14A as t1, (SELECT date, MAX(pm10_avg) as pm10_max FROM T14A GROUP BY date) as t2
WHERE (t1.pm10_avg = t2.pm10_max)
    AND (t1.date = t2.date);

SELECT COUNT(site_id)
FROM (
  SELECT T14C.date, T14C.timestamp, (
    SELECT Site_centroid.data->>'site_id'
    FROM Site_centroid
    WHERE data->'properties'->>'type' = 'building'
    ORDER BY ST_DistanceSphere(ST_GeomFromGeoJSON(Site_centroid.data->>'centroid'), T14C.coordinates) ASC
    LIMIT 1
  ) AS site_id
  FROM T14C
  ORDER BY T14C.date ASC
) AS T14;

DROP TABLE T14A;
DROP TABLE T14C;

 * [Optimized]
CREATE TEMP TABLE T14A (date integer, timestamp integer, latitude integer, longitude integer, pm10_avg double precision);
CREATE TEMP TABLE T14C (date integer, timestamp integer, coordinates geometry);
CREATE TEMP TABLE T14C2 (site_id integer, centroid geometry, description text);

INSERT INTO T14A
SELECT t1.timestamp / 8 as date, t1.timestamp, t1.latitude, t1.longitude, avg(t2.pm10) AS pm10_avg
FROM FineDust_idx as t1, FineDust_idx as t2
WHERE (:Z1 <= t1.timestamp) AND (t1.timestamp <= :Z2)
    AND (t1.timestamp = t2.timestamp)
    AND ((t1.latitude - 2) <= t2.latitude) AND (t2.latitude <= (t1.latitude + 2))
    AND ((t1.longitude - 2) <= t2.longitude) AND (t2.longitude <= (t1.longitude + 2))
GROUP BY t1.timestamp / 8, t1.timestamp, t1.latitude, t1.longitude;

INSERT INTO T14C
SELECT t1.date, t1.timestamp, ST_Point(-118.34501002237936 + (t1.longitude * 0.000216636), 34.011898718557454 + (t1.latitude * 0.000172998)) AS coordinates
FROM T14A as t1, (SELECT date, MAX(pm10_avg) as pm10_max FROM T14A GROUP BY date) as t2
WHERE (t1.pm10_avg = t2.pm10_max)
    AND (t1.date = t2.date);

INSERT INTO T14C2
SELECT CAST(Site_centroid.data->>'site_id' AS integer) AS site_id, ST_GeomFromGeoJSON(Site_centroid.data->>'centroid') as centroid, Site_centroid.data->'properties'->'description' AS description
FROM Site_centroid
WHERE Site_centroid.data->'properties'->>'type' = 'building';

SELECT COUNT(site_id)
FROM (
  SELECT T14C.date, T14C.timestamp, (
    SELECT site_id
    FROM T14C2
    ORDER BY ST_DistanceSphere(centroid, T14C.coordinates) ASC
    LIMIT 1
  ) AS site_id
  FROM T14C
  ORDER BY T14C.date ASC
) AS T14;

DROP TABLE T14A;
DROP TABLE T14C;
DROP TABLE T14C2;
 */


/* 
 * [Task15] Fine Dust Cleaning Vehicles.
 *
 * Recommend the route from the current coordinates by analyzing the hotspot between time Z1 and Z2.
 * Use window aggregation with a size of 5. (Graph, Document, Array) -> Relational
 *
 * [Query]
SET graph_path = Road_network;

CREATE TEMP TABLE T15A (longitude int, latitude int, pm10_sum double precision, pm10_count int);
CREATE TEMP TABLE T15B (coordinates geometry, pm10_avg double precision);

INSERT INTO T15A
SELECT longitude, latitude, sum(pm10) AS pm10_sum, count(pm10) AS pm10_count
FROM Finedust_idx
WHERE (:Z1 <= timestamp) AND (timestamp <= :Z2)
GROUP BY longitude, latitude;

INSERT INTO T15B
SELECT ST_Point(-118.34501002237936 + (t1.longitude * 0.000216636), 34.011898718557454 + (t1.latitude * 0.000172998)) AS coordinates, SUM(t2.pm10_sum) / SUM(t2.pm10_count) AS pm10_avg
FROM T15A as t1, T15A as t2
WHERE ((t1.latitude - 2) <= t2.latitude) AND (t2.latitude <= (t1.latitude + 2))
    AND ((t1.longitude - 2) <= t2.longitude) AND (t2.longitude <= (t1.longitude + 2))
GROUP BY coordinates;

// If you are not return the path, the result (time) will be short than that returning the path.
MATCH (n: RoadNode), (m: RoadNode), path=DIJKSTRA((n)-[e:Road]->(m), e.distance)
WHERE n.site_id = (
        SELECT CAST(Site.data->>'site_id' AS INT)
        FROM Site
        WHERE Site.data->'properties'->>'type' = 'roadnode'
        ORDER BY ST_DistanceSphere(ST_GeomFromGeoJSON(Site.data->>'geometry'), ST_Point(:CLON, :CLAT)) ASC
        LIMIT 1
    )
  AND m.site_id =  (
        SELECT CAST(Site.data->>'site_id' AS INT)
        FROM Site
        WHERE Site.data->'properties'->>'type' = 'roadnode'
        ORDER BY ST_DistanceSphere(ST_GeomFromGeoJSON(Site.data->>'geometry'), (
            SELECT T15B.coordinates FROM T15B, (SELECT MAX(T15B.pm10_avg) as max_avg FROM T15B) as tc1 WHERE T15B.pm10_avg = tc1.max_avg LIMIT 1
        )::geometry) ASC
        LIMIT 1
  )
RETURN COUNT(path)
LIMIT 1;

DROP TABLE T15A;
DROP TABLE T15B;

 * [Optimized]
SET graph_path = Road_network;

CREATE TEMP TABLE T15A (longitude int, latitude int, pm10_sum double precision, pm10_count int);
CREATE TEMP TABLE T15B (coordinates geometry, pm10_avg double precision);

INSERT INTO T15A
SELECT longitude, latitude, sum(pm10) AS pm10_sum, count(pm10) AS pm10_count
FROM Finedust_idx
WHERE (:Z1 <= timestamp) AND (timestamp <= :Z2)
GROUP BY longitude, latitude;

CREATE INDEX t15a_latlon ON T15A (latitude, longitude);

INSERT INTO T15B
SELECT ST_Point(-118.34501002237936 + (t1.longitude * 0.000216636), 34.011898718557454 + (t1.latitude * 0.000172998)) AS coordinates, SUM(t2.pm10_sum) / SUM(t2.pm10_count) AS pm10_avg
FROM T15A as t1, T15A as t2
WHERE ((t1.latitude - 2) <= t2.latitude) AND (t2.latitude <= (t1.latitude + 2))
    AND ((t1.longitude - 2) <= t2.longitude) AND (t2.longitude <= (t1.longitude + 2))
GROUP BY coordinates;

// If you are not return the path, the result (time) will be short than that returning the path.
MATCH (n: RoadNode), (m: RoadNode), path=DIJKSTRA((n)-[e:Road]->(m), e.distance)
WHERE n.site_id = (
        SELECT CAST(Site.data->>'site_id' AS INT)
        FROM Site
        WHERE Site.data->'properties'->>'type' = 'roadnode'
        ORDER BY ST_DistanceSphere(ST_GeomFromGeoJSON(Site.data->>'geometry'), ST_Point(:CLON, :CLAT)) ASC
        LIMIT 1
    )
  AND m.site_id =  (
        SELECT CAST(Site.data->>'site_id' AS INT)
        FROM Site
        WHERE Site.data->'properties'->>'type' = 'roadnode'
        ORDER BY ST_DistanceSphere(ST_GeomFromGeoJSON(Site.data->>'geometry'), (
            SELECT T15B.coordinates FROM T15B, (SELECT MAX(T15B.pm10_avg) as max_avg FROM T15B) as tc1 WHERE T15B.pm10_avg = tc1.max_avg LIMIT 1
        )::geometry) ASC
        LIMIT 1
  )
RETURN COUNT(path)
LIMIT 1;

DROP TABLE T15A;
DROP TABLE T15B;
 */


/**
 *  [Task16] Fine Dust Backtesting ([D, A]=> D).
 *  For a given timestamp Z, hindcast the pm10 values of the schools. (The Z is teh number between min, max of the timestamp dimension.)
 *
 *  Z1 = (Z/TimeInterval)*TimeInterval
 *  Z2 = {(Z+TimeInterval-1)/TimeInterval}*TimeInterval
 *
 *  A: SELECT avg(pm10) FROM FineDust
 *      WHERE timestamp >= Z1  and timestamp <= Z2 group by lat, lon
 *
 *  B: SELECT Site.site_id AS site_id, location,
 *      FROM Site, A
 *      WHERE
 *          WithIN(Box(lat, lon, lat+e1, lon+e2), ST_Centroid(Site.geometry))
 *          Site.properties.building = 'school' //Document


\timing
  With A as
    (
        Select  latitude, longitude, avg(pm10) as pm10
        From finedust_idx
        where  timestamp >= 3
        and timestamp <= 4
        group by latitude, longitude
    )
    , B as
    (  select site_id, sum((coo->>0)::FLOAT)/count(site_id) as longitude, sum((coo->>1)::FLOAT)/count(site_id) as latitude
        from
                (
                    select data->'site_id' as site_id, jsonb_array_elements(jsonb_array_elements(jsonb_array_elements(data->'geometry'->'coordinates'))) as coo
                    from site
                    where data->'properties'->>'type' = 'building' and data->'properties'->>'description'='school'
                )
               as centroid
        group by site_id
     )

Select count(*) from
        (
    Select site_id, pm10
    from A, B
    where
        34.01189870 <= B.latitude and B.latitude <= 34.91494826
        and -118.3450100223 <= B.longitude
        and B.longitude <= -118.23192603
        and A.latitude  <= (B.latitude - 34.01189870) / 0.000172998
        and  (B.latitude - 34.01189870) / 0.000172998  <= A.latitude +  1
        and A.longitude  <= ( B.longitude - (-118.3450100223 )) / 0.000216636
        and  (B.longitude - ( -118.3450100223)) / 0.000216636  <= A.longitude +  1
        ) as res ;


//// THE BELOW IS  DEPRECATED ///


    Explain Analyze With A as
    (
        Select  latitude, longitude, avg(pm10) as pm10
        From finedust
        where  timestamp >= 1600182000+10800*3
        and timestamp <= 1600182000+10800*4
        group by latitude, longitude
    )
    , B as
    (  select site_id, sum((coo->>0)::FLOAT)/count(site_id) as longitude, sum((coo->>1)::FLOAT)/count(site_id) as latitude
        from
                (
                    select data->'site_id' as site_id, jsonb_array_elements(jsonb_array_elements(jsonb_array_elements(data->'geometry'->'coordinates'))) as coo
                    from site
                    where data->'properties'->>'type' = 'building' and data->'properties'->>'description'='school'
                )
               as centroid
        group by site_id
     )
    Select site_id, pm10
    from A, B
    where
        34.01189870 <= B.latitude and B.latitude <= 34.91494826
        and -118.3450100223 <= B.longitude
        and B.longitude <= -118.23192603
        and A.latitude  <= B.latitude
        and  B.latitude  <= A.latitude +  0.000172998
        and A.longitude  <= B.longitude
        and  B.longitude  <= A.longitude +  0.000216636


 */
 
 

