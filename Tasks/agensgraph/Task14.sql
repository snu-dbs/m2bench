\timing 
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
		ORDER BY ST_GeomFromGeoJSON(Site_centroid.data->>'centroid') <-> T14C.coordinates::geography ASC
		LIMIT 1
	) AS site_id
	FROM T14C
	ORDER BY T14C.date ASC
) AS T14;

DROP TABLE T14A;
DROP TABLE T14C;
