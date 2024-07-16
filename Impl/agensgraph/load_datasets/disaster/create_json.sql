\c disaster;
  
DROP TABLE IF EXISTS Site;
CREATE TABLE IF NOT EXISTS Site (
        data jsonb
);

DROP TABLE IF EXISTS Site_centroid;
CREATE TABLE Site_centroid (
        data jsonb
);
