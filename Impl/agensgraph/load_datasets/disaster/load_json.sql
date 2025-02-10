\c disaster;

COPY Site (data) FROM PROGRAM 'sed -e ''s/\\/\\\\/g'' /tmp/m2bench/disaster/json/Site.json';
CREATE INDEX Site_geometry_idx ON Site USING GIST (ST_GeomFromGeoJSON(data ->> 'geometry'::TEXT));
CREATE UNIQUE INDEX Site_id_idx ON Site USING BTREE (((data->>'site_id')::INT));
CREATE INDEX Site_type_idx ON Site USING BTREE (((data->'properties'->>'type')::TEXT));

COPY Site_centroid (data) FROM PROGRAM 'sed -e ''s/\\/\\\\/g'' /tmp/m2bench/disaster/json/Site_centroid.json';
CREATE INDEX Site_centroid_geometry_idx ON Site_centroid USING GIST ((ST_GeomFromGeoJSON(data ->> 'centroid'::TEXT)::GEOGRAPHY));
CREATE UNIQUE INDEX Site_centroid_id_idx ON Site_centroid USING BTREE (((data->>'site_id')::INT));
CREATE INDEX Site_centroid_type_idx ON Site_centroid USING BTREE (((data->'properties'->>'type')::TEXT));
