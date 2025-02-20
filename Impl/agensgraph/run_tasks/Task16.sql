\timing
\o /tmp/t16_explain

CREATE TEMPORARY TABLE A AS (
    SELECT latitude, 
           longitude, 
           AVG(pm10) AS pm10
    FROM finedust_idx
    WHERE (timestamp >= :Z1)
      AND (timestamp <= :Z2)
    GROUP BY latitude, longitude
);

CREATE TEMPORARY TABLE B AS (
    SELECT site_id, 
           SUM((coo->>0)::FLOAT) / COUNT(site_id) AS longitude, 
           SUM((coo->>1)::FLOAT) / COUNT(site_id) AS latitude
    FROM (
        SELECT data->'site_id' AS site_id, 
               jsonb_array_elements(jsonb_array_elements(jsonb_array_elements(data->'geometry'->'coordinates'))) AS coo
        FROM site
        WHERE data->'properties'->>'type' = 'building' 
          AND data->'properties'->>'description' = 'school'
    ) AS centroid
    GROUP BY site_id
);

EXPLAIN ANALYZE SELECT COUNT(*) 
FROM (
    SELECT site_id, pm10
    FROM A, B
    WHERE 34.01189870 <= B.latitude 
      AND B.latitude <= 34.91494826
      AND -118.3450100223 <= B.longitude
      AND B.longitude <= -118.23192603
      AND A.latitude <= (B.latitude - 34.01189870) / 0.000172998
      AND (B.latitude - 34.01189870) / 0.000172998 <= A.latitude + 1
      AND A.longitude <= (B.longitude - (-118.3450100223)) / 0.000216636
      AND (B.longitude - (-118.3450100223)) / 0.000216636 <= A.longitude + 1
) AS res;

-- Answer Validation
-- COPY (
--     SELECT site_id, pm10
--     FROM A, B
--     WHERE 34.01189870 <= B.latitude 
--       AND B.latitude <= 34.91494826
--       AND -118.3450100223 <= B.longitude
--       AND B.longitude <= -118.23192603
--       AND A.latitude <= (B.latitude - 34.01189870) / 0.000172998
--       AND (B.latitude - 34.01189870) / 0.000172998 <= A.latitude + 1
--       AND A.longitude <= (B.longitude - (-118.3450100223)) / 0.000216636
--       AND (B.longitude - (-118.3450100223)) / 0.000216636 <= A.longitude + 1
-- ) TO '/tmp/t16.csv' DELIMITER ',' CSV HEADER;
