\timing 

CREATE TEMPORARY TABLE T14A (
    date       INTEGER, 
    timestamp  INTEGER, 
    latitude   INTEGER, 
    longitude  INTEGER, 
    pm10_avg   DOUBLE PRECISION
);

CREATE TEMPORARY TABLE T14C (
    date       INTEGER, 
    timestamp  INTEGER, 
    coordinates GEOMETRY
);

INSERT INTO T14A
SELECT 
    t1.timestamp / 8 AS date, 
    t1.timestamp, 
    t1.latitude, 
    t1.longitude, 
    AVG(t2.pm10) AS pm10_avg
FROM FineDust_idx AS t1, FineDust_idx AS t2
WHERE (:Z1 <= t1.timestamp) 
  AND (t1.timestamp <= :Z2)
  AND (t1.timestamp = t2.timestamp)
  AND ((t1.latitude - 2) <= t2.latitude) 
  AND (t2.latitude <= (t1.latitude + 2))
  AND ((t1.longitude - 2) <= t2.longitude) 
  AND (t2.longitude <= (t1.longitude + 2))
GROUP BY t1.timestamp / 8, t1.timestamp, t1.latitude, t1.longitude;

WITH Ranked AS (
    SELECT 
        t1.date, 
        t1.timestamp, 
        ST_Point(-118.34501002237936 + (t1.longitude * 0.000216636), 
                34.011898718557454 + (t1.latitude * 0.000172998)) AS coordinates
        ROW_NUMBER() OVER (
            PARTITION BY t1.date 
            ORDER BY t1.timestamp ASC, t1.latitude ASC, t1.longitude ASC
        ) AS rn
    FROM T14A AS t1, 
        (SELECT date, MAX(pm10_avg) AS pm10_max FROM T14A GROUP BY date) AS t2
    WHERE (t1.pm10_avg = t2.pm10_max)
    AND (t1.date = t2.date)
)

INSERT INTO T14C
SELECT date, timestamp, coordinates
FROM Ranked
WHERE rn = 1;

SELECT COUNT(site_id)
FROM (
    SELECT 
        T14C.date, 
        T14C.timestamp, 
        (
            SELECT Site_centroid.data->>'site_id'
            FROM Site_centroid
            WHERE data->'properties'->>'type' = 'building'
            ORDER BY ST_GeomFromGeoJSON(Site_centroid.data->>'centroid') <-> T14C.coordinates::geography ASC
            LIMIT 1
        ) AS site_id
    FROM T14C
    ORDER BY T14C.date ASC
) AS T14;

-- Answer Validation
COPY (
    SELECT 
        T14C.date, 
        T14C.timestamp, 
        (
            SELECT Site_centroid.data->>'site_id'
            FROM Site_centroid
            WHERE data->'properties'->>'type' = 'building'
            ORDER BY ST_GeomFromGeoJSON(Site_centroid.data->>'centroid') <-> T14C.coordinates::geography ASC
            LIMIT 1
        ) AS site_id
    FROM T14C
    ORDER BY T14C.date ASC
) TO '/tmp/t14.csv' DELIMITER ',' CSV HEADER;

