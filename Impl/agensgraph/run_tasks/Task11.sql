SET graph_path = Road_network;

\timing
SELECT time, coordinates INTO TEMPORARY TABLE eqk_x FROM earthquake WHERE earthquake_id=41862;

WITH A AS (
    SELECT gps.gps_id, gps.coordinates FROM gps, eqk_x
    WHERE gps.time >= eqk_X.time AND gps.time < eqk_X.time + interval '1 hour' AND ST_DWithin(gps.coordinates::geography, eqk_x.coordinates::geography, 10000, false)
    ),
    gps_nodes AS (
    SELECT t1.gps_id AS gps_id, t2.site_id AS roadnode_id
    FROM A t1 LEFT JOIN LATERAL
            (SELECT (site.data->>'site_id')::int AS site_id FROM site WHERE site.data->'properties'->>'type'='roadnode'
            ORDER BY ST_GeomFromGeoJSON(Site.data->>'geometry') <-> (t1.coordinates)::geography ASC LIMIT 1) t2 on true
    ),
    B AS (
    SELECT shelter.shelter_id, ST_GeomFromGeoJSON(site_centroid.data->>'centroid') AS centroid
    FROM eqk_x, shelter, site_centroid
    WHERE shelter.site_id = (site_centroid.data->>'site_id')::int AND ST_DWithin(eqk_x.coordinates::geography, ST_GeomFromGeoJSON(site_centroid.data->>'centroid')::geography, 15000, false)
    ),
    shelter_nodes AS (
    SELECT t1.shelter_id AS shelter_id, t2.site_id AS roadnode_id
    FROM B t1 LEFT JOIN LATERAL
    		(SELECT (site.data->>'site_id')::int AS site_id FROM site WHERE site.data->'properties'->>'type'='roadnode'
            ORDER BY ST_GeomFromGeoJSON(Site.data->>'geometry') <-> (t1.centroid)::geography ASC LIMIT 1) t2 on true
    )
SELECT COUNT(*) FROM (
    SELECT src, dest, SUM(distance) as total_cost
    FROM (SELECT src.gps_id AS src, dest.shelter_id AS dest, (unnest(roads)->>'distance')::INT AS distance
          FROM gps_nodes AS src CROSS JOIN shelter_nodes AS dest
          CROSS JOIN lateral (MATCH (n: RoadNode), (m: RoadNode), path=DIJKSTRA((n)-[e:Road]->(m), e.distance)
                              WHERE n.site_id = (SELECT DISTINCT(gps_nodes.roadnode_id) FROM gps_nodes WHERE gps_nodes.roadnode_id = src.roadnode_id)
                              AND m.site_id = (SELECT DISTINCT(shelter_nodes.roadnode_id) FROM shelter_nodes WHERE shelter_nodes.roadnode_id = dest.roadnode_id)
                              RETURN n,m,edges(path) AS roads) AS graph
         ) AS gps_shelter_pair
      GROUP BY src, dest
      ) as C;
