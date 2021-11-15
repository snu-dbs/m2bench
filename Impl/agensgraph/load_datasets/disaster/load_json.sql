\c disaster
  
COPY Site (data) FROM program 'sed -e ''s/\\/\\\\/g'' /disaster/json/Site.json';
CREATE INDEX Site_geometry_idx ON Site USING GIST(ST_GeomFromGeoJSON(data ->> 'geometry'::text));
CREATE UNIQUE INDEX Site_id_idx ON Site USING btree (((data->>'site_id')::int));
CREATE INDEX Site_type_idx On Site USING btree (((data->'properties'->>'type')::text));
