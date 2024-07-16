SET graph_path = Road_network;
  
\timing
WITH eqk AS (
    SELECT earthquake_id, time, coordinates FROM earthquake
    WHERE time >= to_timestamp('2020-06-01 00:00:00' , 'YYYY-MM-DD HH24:MI:SS') AND time < to_timestamp('2020-06-01 02:00:00', 'YYYY-MM-DD HH24:MI:SS')
    ),
    roadnodes AS (
    SELECT eqk.earthquake_id AS eqk_id, data->'site_id' AS site_id
    FROM site, eqk
    WHERE site.data->'properties'->>'type'='roadnode' AND ST_DWithin(ST_GeomFromGeoJSON(site.data->>'geometry')::geography, eqk.coordinates::geography, 5000, false)
    )
SELECT COUNT(*) FROM (
    SELECT subgraph
    FROM roadnodes AS src
    CROSS JOIN LATERAL (
                MATCH (n:roadnode)-[r:road]->(m:roadnode)
                WHERE n.site_id = src.site_id
                RETURN n,r,m
                ) AS subgraph
    ) as A;
