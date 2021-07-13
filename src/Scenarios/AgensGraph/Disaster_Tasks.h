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
*     AND Earthquake.time >= Z1 AND Earthquake.time <= Z2 AND RoadNode.site_id = Site.site_id  //Graph


    SET graph_path = Road_network;

    Explain Analyze WITH eqk AS (
            SELECT earthquake_id AS id, time, ST_MakePoint(ST_X(coordinates), ST_Y(coordinates)) AS geom
    FROM earthquake
    WHERE time >= '2020-06-01 00:00:00.000' AND time <= '2020-06-01 02:00:00.000'
    ),
    roadnodes AS (
            SELECT eqk.id AS eqk_id, data->'site_id' AS site_id
    FROM eqk, site
    WHERE site.data->'properties'->>'type'='roadnode' AND ST_DistanceSphere(eqk.geom, ST_GeomFromGeoJSON(site.data->>'geometry')) <= 5000
    )
    SELECT subgraph
    FROM roadnodes, (MATCH (n:roadnode)-[r:road]->(m:roadnode) RETURN n,r,m) AS subgraph
    WHERE subgraph.n->'site_id'=roadnodes.site_id;
*/


/**
  *  [Task 11] Closest Shelter ([R, D, G]=> R)
  *  For a given earthquake information X, find the cost of the shortest path for each GPS coordinate and Shelter pair.
  *  (GPS coordinates are limited by 1 hour and 10km from the X. Shelters are limited by 15km from the X.)
  *
  *  A = SELECT GPS.gps_id, ST_ClosestObject(Site, roadnode, GPS.coordinates) AS roadnode_id FROM GPS, Site, RoadNode
  *      WHERE GPS.time >= X.time AND GPS.time <= X.time + 1 hour AND ST_Distance(GPS.coordinates, X.coordinates) <= 10km
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
        SELECT filtered_gps.gps_id AS gps_id, site.data->'site_id' AS site_id, ST_DistanceSphere(filtered_gps.geom, ST_GeomFromGeoJSON(site.data->>'geometry')) AS dist
        FROM (SELECT gps.gps_id AS gps_id, ST_MakePoint(ST_X(gps.coordinates), ST_Y(gps.coordinates)) AS geom
              FROM gps, eqk_x WHERE gps.time >= eqk_X.time AND gps.time<=eqk_X.time + interval '1 hour'
              AND ST_DistanceSphere(eqk_X.geom, ST_MakePoint(ST_X(gps.coordinates), ST_Y(gps.coordinates)))<=10000) AS filtered_gps, site
        WHERE site.data->'properties'->>'type'='roadnode'
	    ),
	    gps_node AS (
	    SELECT t1.gps_id, t1.site_id
	    FROM A t1, (SELECT  gps_id, MIN(dist) as mindist FROM A GROUP BY gps_id) t2
	    WHERE t1.gps_id = t2.gps_id AND t1.dist = t2.mindist
	    ),
	    B AS (
        SELECT shelter.shelter_id AS shelter_id, ST_Centroid(ST_GeomFromGeoJSON(site.data->>'geometry')) AS geom
        FROM eqk_x, shelter, site
        WHERE shelter.site_id = (site.data->>'site_id')::int AND ST_DistanceSphere(eqk_x.geom, ST_Centroid(ST_GeomFromGeoJSON(site.data->>'geometry'))) <=15000
	    ),
	    C AS (
        SELECT B.shelter_id AS shelter_id, site.data->'site_id' AS site_id, ST_DistanceSphere(B.geom, ST_Centroid(ST_GeomFromGeoJSON(site.data->>'geometry'))) AS dist
        FROM B, site WHERE site.data->'properties'->>'type'='roadnode'
        ),
        shelter_node AS (
        SELECT t1.shelter_id, t1.site_id
        FROM C t1, (SELECT shelter_id, MIN(dist) as mindist FROM C GROUP BY shelter_id) t2
        WHERE t1.shelter_id = t2.shelter_id AND t1.dist = t2.mindist
	    ),
	    D AS (
        SELECT src, dest, (unnest(roads)->>'distance')::INT AS distance FROM gps_node AS src CROSS JOIN shelter_node AS dest
        CROSS JOIN lateral (MATCH (n: RoadNode), (m: RoadNode), path=DIJKSTRA((n)-[e:Road]->(m), e.distance)
                            WHERE n.site_id = (SELECT gps_node.site_id FROM gps_node WHERE gps_node.site_id = src.site_id)
                            AND m.site_id = (SELECT distinct(shelter_node.site_id)
                            FROM shelter_node WHERE shelter_node.site_id = dest.site_id)
        RETURN n,m,edges(path) AS roads) AS graph
	    )
	    SELECT src, dest, SUM(distance) as total_cost
	    FROM D
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
		WHERE gps.time >= '2020-09-17 00:00:00.000' AND gps.time < '2020-09-17 00:01:00.000'
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
 * A =  SELECT Site.properties.description, COUNT(*) 
 *      FROM Earthquake, Site WHERE ST_Distance(Site.geometry, Earthquake.coordinates) <= 30km 
 *          AND Site.properties.type = 'building' 
 *          AND Earthquake.magnitude >= 4.5 
 *      GROUP BY Site.properties.description // Document
 *
 * 
 * [Query]
 * 
 * SELECT Site.data->'properties'->'description' AS description, COUNT(*) 
 * FROM Earthquake, Site
 * WHERE ST_Distance(ST_GeomFromGeoJSON(Site.data->>'geometry'), Earthquake.coordinates) <= 30000 
 *     AND Site.data->'properties'->>'type' = 'building'
 *     AND Earthquake.magnitude >= 4.5 
 * GROUP BY Site.data->'properties'->'description';
 */


/* 
 * [Task 14] Sources of Fine Dust.
 *
 * Analyze fine dust hotspot by date between time Z1 and Z2.
 * Print the nearest building with time of the hotspot.
 * Use window aggregation with a size of 5. (Document, Array) -> Document
 *
 * A =  SELECT date, timestamp, latitude, longitude, AVG(pm10) AS pm10_avg 
 *      FROM FineDust 
 *      WHERE timestamp >= Z1 
 *          AND timestamp <= Z2 
 *      WINDOW 1, 5, 5 // Array
 * B =  REDIMENSION(A, <pm10_avg: float>[date=0:*, timestamp=0:*, latitude=0:*, longitude=0:*]) // Array
 * C =  SELECT t1.date, t1.timestamp, (t1.latitude, t1.longitude) AS coordinates 
 *      FROM B AS t1, (
 *          SELECT date, MAX(pm10_avg) AS pm10_max 
 *          FROM B GROUP BY date
 *      ) AS t2 
 *      WHERE t1.pm10_avg = t2.pm10_max 
 *          AND t1.date = t2.date // Array 
 * D =  SELECT C.date, C.timestamp, ST_ClosestObject(Site, building, C.coordinates) AS site_id 
 *      FROM C, Site 
 *      ORDER BY C.date ASC // Document
 * 
 * 
 * [Query]
 * // TEMP TABLE FOR A 
 * CREATE TEMP TABLE T14A (date integer, timestamp integer, latitude float, longitude float, pm10_avg float);
 * 
 * // INSERT INTO T14A
 * INSERT INTO T14A
 * SELECT t1.timestamp / 10800000 as date, t1.timestamp, t1.latitude, t1.longitude, avg(t2.pm10) AS pm10_avg
 * FROM FineDust as t1, FineDust as t2
 * WHERE (Z1 <= t1.timestamp)
 *     AND (t1.timestamp <= Z2)
 *     AND ((t1.latitude - 0.000172998 * 2.01) <= t2.latitude)
 *     AND (t2.latitude <= (t1.latitude + 0.000172998 * 2.01))
 *     AND ((t1.longitude - 0.000216636 * 2.01) <= t2.longitude)
 *     AND (t2.longitude <= (t1.longitude + 0.000216636 * 2.01))
 *     AND (t1.timestamp = t2.timestamp)
 * GROUP BY t1.timestamp / 10800000, t1.timestamp, t1.latitude, t1.longitude;
 * 
 * // No B 
 * 
 * // C 
 * CREATE TEMP TABLE T14C (date integer, timestamp integer, coordinates geometry);
 * 
 * INSERT INTO T14C
 * SELECT t1.date, t1.timestamp, ST_Point(t1.longitude, t1.latitude) AS coordinates
 * FROM T14A as t1, (SELECT date, MAX(pm10_avg) as pm10_max FROM T14A GROUP BY date) as t2
 * WHERE (t1.pm10_avg = t2.pm10_max)
 *     AND (t1.date = t2.date);
 * 
 * // D 
 * SELECT T14C.date, T14C.timestamp, (
 *     SELECT Site.data->>'site_id'
 *     FROM Site
 *     ORDER BY ST_Distance(ST_GeomFromGeoJSON(Site.data->>'geometry'), T14C.coordinates) ASC
 *     LIMIT 1
 * ) AS site_id
 * FROM T14C;
 * 
 * DROP TABLE T14A;
 * DROP TABLE T14C;
 */


/* 
 * [Task15] Fine Dust Cleaning Vehicles.
 *
 * Recommend the route from the current coordinates by analyzing the hotspot between time Z1 and Z2.
 * Use window aggregation with a size of 5. (Graph, Document, Array) -> Relational
 *
 * A =  SELECT (latitude, longitude) AS coordinates, AVG(pm10) AS pm10_avg 
 *      FROM FineDust 
 *      WHERE timestamp >= Z1 
 *          AND timestamp <= Z2 
 *      WINDOW *, 5, 5 // Array
 * B =  SELECT ShortestPath(RoadNode, startNode: ST_ClosestObject(Site, roadnode, current_coordinates), endNode: ST_ClosestObject(Site, roadnode, A.coordinates)) 
 *      FROM A, RoadNode, Site 
 *      WHERE Site.site_id = RoadNode.site_id 
 *      ORDER BY A.pm10_avg DESC
 *      LIMIT 1 // Relational
 * 
 * 
 * [Query]
 * 
 * SET graph_path = Road_network;
 * 
 * // TEMP TABLE FOR A
 * CREATE TEMP TABLE T15A (coordinates geometry, pm10_avg float);
 * 
 * INSERT INTO T15A
 * SELECT ST_Point(t1.longitude, t1.latitude) AS coordinates, avg(t1.pm10) AS pm10_avg
 * FROM FineDust as t1, FineDust as t2
 * WHERE (Z1 <= t1.timestamp)
 *    AND (t1.timestamp <= Z2)
 *    AND ((t1.latitude - 0.000172998 * 2.01) <= t2.latitude)
 *    AND (t2.latitude <= (t1.latitude + 0.000172998 * 2.01))
 *    AND ((t1.longitude - 0.000216636 * 2.01) <= t2.longitude)
 *    AND (t2.longitude <= (t1.longitude + 0.000216636 * 2.01))
 * GROUP BY coordinates;
 * 
 * MATCH (n: RoadNode {roadnode_id: (
 *    SELECT Site.data->>'site_id'
 *    FROM Site
 *    ORDER BY ST_Distance(ST_GeomFromGeoJSON(Site.data->>'geometry'), ST_Point(current_longitude, current_latitude)) ASC
 *    LIMIT 1
 * )}), (m: RoadNode {roadnode_id: (
 *     SELECT Site.data->>'site_id'
 *     FROM Site
 *     ORDER BY ST_Distance(ST_GeomFromGeoJSON(Site.data->>'geometry'), (
 *         SELECT MAX(T15A.coordinates)
 *         FROM T15A
 *     )::geometry) ASC
 *     LIMIT 1
 * )}), path=DIJKSTRA((n)-[e:Road]->(m), e.distance) 
 * RETURN path;
 * 
 * DROP TABLE T15A;
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

