//
// Created by mxmdb on 21. 6. 29..
//

#ifndef M2BENCH_AO_DISASTER_TASKS_H
#define M2BENCH_AO_DISASTER_TASKS_H

#endif //M2BENCH_AO_DISASTER_TASKS_H

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
        where  timestamp >= 1600182000+10800*3 and timestamp <= 1600182000+10800*4
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
        and -118.3450100223 <= B.longitude and B.longitude <= -118.23192603
        and A.latitude  <= B.latitude and  B.latitude  <= A.latitude +  0.000172998
        and A.longitude  <= B.longitude and  B.longitude  <= A.longitude +  0.000216636



 */

