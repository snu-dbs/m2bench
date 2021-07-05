//
// Created by mxmdb on 21. 6. 29..
//

#ifndef M2BENCH_AO_DISASTER_TASKS_H
#define M2BENCH_AO_DISASTER_TASKS_H

#endif //M2BENCH_AO_DISASTER_TASKS_H

/**
 * [Task 10] Road Network Filtering ([R, D, G]=> G)
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

