SET graph_path = Road_network;
\timing

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

MATCH (n: RoadNode), (m: RoadNode), path=DIJKSTRA((n)-[e:Road]->(m), e.distance)
WHERE n.site_id = (
	SELECT CAST(Site.data->>'site_id' AS INT)
	FROM Site
	WHERE Site.data->'properties'->>'type' = 'roadnode'
	ORDER BY ST_GeomFromGeoJSON(Site.data->>'geometry') <-> ST_Point(:CLON, :CLAT)::geography ASC
	LIMIT 1
)
AND m.site_id =  (
	SELECT CAST(Site.data->>'site_id' AS INT)
	FROM Site
	WHERE Site.data->'properties'->>'type' = 'roadnode'
	ORDER BY ST_GeomFromGeoJSON(Site.data->>'geometry') <-> ((
		SELECT T15B.coordinates FROM T15B, (SELECT MAX(T15B.pm10_avg) as max_avg FROM T15B) as tc1 WHERE T15B.pm10_avg = tc1.max_avg LIMIT 1
	)::geography) ASC
LIMIT 1
)
RETURN COUNT(path)
LIMIT 1;

DROP TABLE T15A;
DROP TABLE T15B;
