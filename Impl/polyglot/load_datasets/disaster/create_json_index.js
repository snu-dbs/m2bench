db.Site.createIndex({ 'site_id': 1 });
db.Site.createIndex({ 'properties.type': 1 });
db.Site.createIndex({ 'properties.type': 1, 'geometry': '2dsphere' });

db.Site_centroid.createIndex({ 'site_id': 1 });
db.Site_centroid.createIndex({ 'properties.type': 1 });
db.Site_centroid.createIndex({ 'properties.type': 1, 'centroid': '2dsphere' });