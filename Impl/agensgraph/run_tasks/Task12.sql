SET graph_path = Road_network;

-- explain analyze 
    WITH A AS (
        SELECT shelter.shelter_id, ST_GeomFromGeoJSON(site_centroid.data->>'centroid') AS centroid, COUNT(gps.gps_id) AS numGps
        FROM shelter, site_centroid, gps
        WHERE gps.time >= to_timestamp('2020-09-17 00:00:00', 'YYYY-MM-DD HH24:MI:SS') AND gps.time < to_timestamp('2020-09-17 01:00:00' , 'YYYY-MM-DD HH24:MI:SS') AND (site_centroid.data->>'site_id')::int = shelter.site_id AND site_centroid.data->'properties'->>'type'='building' AND site_centroid.data->'properties'->>'description'='hospital'
        AND ST_DWithin(ST_GeomFromGeoJSON(site_centroid.data->>'centroid')::geography, gps.coordinates::geography, 5000, false)
        GROUP BY shelter.shelter_id, ST_GeomFromGeoJSON(site_centroid.data->>'centroid')
        ORDER BY numGps DESC
        LIMIT 1
        ),
        B AS (
        SELECT A.shelter_id as shelter_id, A.centroid as shelter_geom, (Site.data->>'site_id')::INT AS roadnode_id
        FROM A, site WHERE site.data->'properties'->>'type'='roadnode'
        ORDER BY ST_GeomFromGeoJSON(Site.data->>'geometry') <-> (A.centroid)::geography ASC
        LIMIT 1
        ),
        filtered_buildings AS (
        SELECT (site_centroid.data->>'site_id')::INT AS building_id, ST_GeomFromGeoJSON(site_centroid.data->>'centroid') AS centroid
         FROM B, site_centroid WHERE site_centroid.data->'properties'->>'type'='building' AND site_centroid.data->'properties'->>'description'='school' AND ST_DWithin(ST_GeomFromGeoJSON(site_centroid.data->>'centroid')::geography, B.shelter_geom::geography, 1000, false)
         ),
         C AS (
         SELECT t1.building_id AS building_id, t2.site_id AS roadnode_id
         FROM filtered_buildings t1 LEFT JOIN LATERAL  (SELECT (site.data->>'site_id')::int AS site_id FROM site WHERE site.data->'properties'->>'type'='roadnode'
                                                        ORDER BY ST_GeomFromGeoJSON(Site.data->>'geometry') <-> (t1.centroid)::geography ASC LIMIT 1) t2 on true
         )
    SELECT COUNT(*) FROM (
        SELECT dest
        FROM (SELECT src, dest.building_id AS dest, (unnest(e)->>'distance')::INT AS distance
                FROM B AS src CROSS JOIN C AS dest
                CROSS JOIN LATERAL (MATCH (n: RoadNode), (m: RoadNode), path=DIJKSTRA((n)-[e:Road]->(m), e.distance)
                                    WHERE n.site_id = (select distinct(B.roadnode_id) from B where B.roadnode_id = src.roadnode_id)
                                    AND m.site_id = (select distinct(C.roadnode_id) from C where C.roadnode_id = dest.roadnode_id)
                                    RETURN n,m, edges(path) AS e) AS graph
                 ) AS shelter_building_pair
           GROUP BY dest
           ORDER BY SUM(distance), dest
           LIMIT 5
     ) as D;
