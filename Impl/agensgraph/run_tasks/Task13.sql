\timing 
SELECT COUNT(*)
FROM (
	SELECT Site_centroid.data->'properties'->'description' AS description, COUNT(*)
	FROM Earthquake, Site_centroid
	WHERE ST_DWithin(ST_GeomFromGeoJSON(Site_centroid.data->>'centroid')::geography, Earthquake.coordinates::geography, 30000, false)
		AND Site_centroid.data->'properties'->>'type' = 'building'
		AND Earthquake.magnitude >= 4.5
	GROUP BY Site_centroid.data->'properties'->'description'
) AS T13;
