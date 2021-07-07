//
// Created by mxmdb on 21. 6. 29..
//

#ifndef M2BENCH_AO_DISASTER_TASKS_H
#define M2BENCH_AO_DISASTER_TASKS_H

#endif //M2BENCH_AO_DISASTER_TASKS_H

/**
*
*  [Task11] Closest Shelter ([R, G]=>R).
*  For a given earthquake information X, find the cost of the shortest path for each GPS coordinate and Shelter pair.
*  GPS coordinates are limited by 1 hour and 40km from the X. Shelters are limited by 40km from the X.
*

SET graph_path = Road_network;

 WITH A AS (
	SELECT filtered_gps.gps_id AS gps_id, site.data->'site_id' AS site_id, ST_DistanceSphere(filtered_gps.geom, ST_GeomFromGeoJSON(site.data->>'geometry')) AS dist
	FROM (SELECT gps.gps_id AS gps_id, ST_MakePoint(ST_X(gps.coordinates), ST_Y(gps.coordinates)) AS geom FROM gps, eqk_x
	 WHERE gps.time >= eqk_X.time AND gps.time<=eqk_X.time + interval '1 hour' AND ST_DistanceSphere(eqk_X.geom, ST_MakePoint(ST_X(gps.coordinates), ST_Y(gps.coordinates)))<=10000) as filtered_gps, site
	WHERE site.data->'properties'->>'type'='roadnode'
	),
	B AS (
	SELECT t1.gps_id, t1.site_id
	FROM A t1, (SELECT  gps_id, MIN(dist) as mindist FROM A GROUP BY gps_id) t2 WHERE t1.gps_id = t2.gps_id AND t1.dist = t2.mindist
	)
    Select * INTO temp table gps_node from B

 With C AS (
	SELECT shelter.shelter_id AS shelter_id, ST_Centroid(ST_GeomFromGeoJSON(site.data->>'geometry')) AS geom
	FROM eqk_x, shelter, site
	WHERE shelter.site_id = (site.data->>'site_id')::int AND ST_DistanceSphere(eqk_x.geom, ST_Centroid(ST_GeomFromGeoJSON(site.data->>'geometry'))) <=15000
	),
	D AS (
	SELECT C.shelter_id AS shelter_id, site.data->'site_id' AS site_id, ST_DistanceSphere(C.geom, ST_GeomFromGeoJSON(site.data->>'geometry')) AS dist
	FROM C, site WHERE site.data->'properties'->>'type'='roadnode'
	),
	E AS (
	SELECT t1.shelter_id, t1.site_id
	FROM D t1, (SELECT shelter_id, MIN(dist) as mindist FROM D GROUP BY shelter_id) t2
	WHERE t1.shelter_id = t2.shelter_id AND t1.dist = t2.mindist
	)
    Select * INTO temp table shelter_node from E


 SELECT path
    FROM gps_node as source
    cross join
    shelter_node as dest
    cross join lateral
    (MATCH (n: RoadNode), (m: RoadNode), path=DIJKSTRA((n)-[e:Road]->(m), e.distance) WHERE n.site_id = (select gps_node.site_id from gps_node where gps_node.site_id = source.site_id ) and m.site_id = (select distinct(shelter_node.site_id) from shelter_node where shelter_node.site_id = dest.site_id) RETURN n,m, PATH) AS graph;

**/


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

